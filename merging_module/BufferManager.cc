#include "BufferManager.h"

void getColOffset(const char* row_data, int* col_offset_list, vector<int> return_datatype, vector<int> table_offlen);

void BufferManager::initBufferManager(){
    std::thread BufferManagerInterface(&BufferManager::bufferManagerInterface,this);
    BufferManagerInterface.detach();
}

void BufferManager::bufferManagerInterface(){
    int server_fd, client_fd;
	int opt = 1;
	struct sockaddr_in serv_addr, client_addr;
	socklen_t addrlen = sizeof(client_addr);
    static char cMsg[] = "ok";

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	serv_addr.sin_port = htons(SE_MERGING_CONTAINER_BM_TCP_PORT); 
 
	if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		perror("bind");
		exit(EXIT_FAILURE);
	} 

	if (listen(server_fd, NCONNECTION) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}

    KETILOG::WARNLOG(LOGTAG,"CSD Return Server Listening on 0.0.0.0:"+to_string(SE_MERGING_CONTAINER_BM_TCP_PORT));

	while(1){
		if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen)) < 0){
			perror("accept");
        	exit(EXIT_FAILURE);
		}

		std::string json = "";
        int njson;
		size_t ljson;

		recv(client_fd , &ljson, sizeof(ljson), 0);

        char buffer[ljson];
        memset(buffer, 0, ljson);
		
		while(1) {
			if ((njson = recv(client_fd, buffer, BUFF_SIZE-1, 0)) == -1) {
				perror("read");
				exit(1);
			}
			ljson -= njson;
		    buffer[njson] = '\0';
			json += buffer;

		    if (ljson == 0)
				break;
		}
		
        send(client_fd, cMsg, strlen(cMsg), 0);

		char data[BUFF_SIZE];
        char* dataiter = data;
		memset(data, 0, BUFF_SIZE);
        int ndata = 0;
        size_t ldata = 0;
        recv(client_fd , &ldata, sizeof(ldata),0);

		while(1) {
			if ((ndata = recv( client_fd , dataiter, ldata,0)) == -1) {
				perror("read");
				exit(1);
			}
            dataiter += ndata;
			ldata -= ndata;

		    if (ldata == 0)
				break;
		}

        send(client_fd, cMsg, strlen(cMsg), 0);

        pushResult(BlockResult(json.c_str(), data));
        
        close(client_fd);		
	}   
	close(server_fd);
}

void BufferManager::pushResult(BlockResult blockResult){
    int qid = blockResult.query_id;
    int wid = blockResult.work_id;

    if(DataBuffer_.find(blockResult.query_id) == DataBuffer_.end()){
        QueryBuffer* queryBuffer = new QueryBuffer(qid);
        DataBuffer_.insert(pair<int,QueryBuffer*>(qid,queryBuffer));
    }
    
    if((DataBuffer_[blockResult.query_id]->work_buffer_list.find(blockResult.work_id) 
            == DataBuffer_[blockResult.query_id]->work_buffer_list.end())){
        WorkBuffer* workBuffer = new WorkBuffer();

        workBuffer->table_alias = blockResult.table_alias;
        for(int i = 0; i < blockResult.column_alias.size(); i++){
            workBuffer->table_column.push_back(blockResult.column_alias[i]);
            workBuffer->table_data.insert({blockResult.column_alias[i],ColData{}});
        }

        DataBuffer_[qid]->tablename_workid_map[blockResult.table_alias] = wid;
        DataBuffer_[qid]->work_buffer_list[wid] = workBuffer;
        DataBuffer_[qid]->work_buffer_list[wid]->left_block_count = blockResult.total_block_count;
        DataBuffer_[qid]->work_buffer_list[wid]->work_buffer_queue.push_work(blockResult);

        std::thread mergeResultThread(&BufferManager::mergeResult,this, qid, wid);
        mergeResultThread.detach();
    }

    DataBuffer_[qid]->work_buffer_list[wid]->work_buffer_queue.push_work(blockResult);
}

void BufferManager::mergeResult(int qid, int wid){
    WorkBuffer* myWorkBuffer = DataBuffer_[qid]->work_buffer_list[wid];

    while (1){
        BlockResult result = myWorkBuffer->work_buffer_queue.wait_and_pop();
        unique_lock<mutex> lock(myWorkBuffer->mu);

        string col_name;
        
        if(result.length != 0){
            int col_type, col_offset, col_len, origin_row_len, col_count = 0;
            vector<int> new_row_offset;
            new_row_offset.assign(result.row_offset.begin(), result.row_offset.end());
            new_row_offset.push_back(result.length);

            if(KETILOG::IsLogLevelUnder(TRACE)){
                // 리턴 데이터 형식 확인 - Debug Code   
                for(int i = 0; i<result.return_datatype.size(); i++){
                    cout << result.return_datatype[i] << " ";
                }
                cout << endl;
                for(int i = 0; i<result.return_datatype.size(); i++){
                    cout << result.return_offlen[i] << " ";
                }
                cout << endl;  
            }

            for(int i=0; i<result.row_count; i++){
                origin_row_len = new_row_offset[i+1] - new_row_offset[i];
                char row_data[origin_row_len];
                memcpy(row_data,result.data+result.row_offset[i],origin_row_len);

                col_count = myWorkBuffer->table_column.size();
                int col_offset_list[col_count + 1];
                
                getColOffset(row_data, col_offset_list, result.return_datatype, result.return_offlen);
                col_offset_list[col_count] = origin_row_len;

                for(size_t j=0; j<myWorkBuffer->table_column.size(); j++){
                    col_name = myWorkBuffer->table_column[j];
                    col_offset = col_offset_list[j];
                    col_len = col_offset_list[j+1] - col_offset_list[j];
                    col_type = result.return_datatype[j];

                    switch (col_type){
                        case MySQL_BYTE:{
                            char tempbuf[col_len];
                            memcpy(tempbuf,row_data+col_offset,col_len);
                            int64_t my_value = *((int8_t *)tempbuf);
                            myWorkBuffer->table_data[col_name].intvec.push_back(my_value);
                            break;
                        }case MySQL_INT16:{
                            char tempbuf[col_len];
                            memcpy(tempbuf,row_data+col_offset,col_len);
                            int64_t my_value = *((int16_t *)tempbuf);
                            myWorkBuffer->table_data[col_name].intvec.push_back(my_value);     
                            break;
                        }case MySQL_INT32:{
                            char tempbuf[col_len];
                            memcpy(tempbuf,row_data+col_offset,col_len);
                            int64_t my_value = *((int32_t *)tempbuf);
                            myWorkBuffer->table_data[col_name].intvec.push_back(my_value);
                            break;
                        }case MySQL_INT64:{
                            char tempbuf[col_len];
                            memcpy(tempbuf,row_data+col_offset,col_len);
                            int64_t my_value = *((int64_t *)tempbuf);
                            myWorkBuffer->table_data[col_name].intvec.push_back(my_value);
                            break;
                        }case MySQL_FLOAT32:{
                            //아직 처리X
                            char tempbuf[col_len];//col_len = 4
                            memcpy(tempbuf,row_data+col_offset,col_len);
                            double my_value = *((float *)tempbuf);
                            myWorkBuffer->table_data[col_name].floatvec.push_back(my_value);
                            break;
                        }case MySQL_DOUBLE:{
                            //아직 처리X
                            char tempbuf[col_len];//col_len = 8
                            memcpy(tempbuf,row_data+col_offset,col_len);
                            double my_value = *((double *)tempbuf);
                            myWorkBuffer->table_data[col_name].floatvec.push_back(my_value);
                            break;
                        }case MySQL_NEWDECIMAL:{
                            //decimal(15,2)만 고려한 상황 -> col_len = 7 or 8 (integer:6/real:1 or 2 or 3)
                            char tempbuf[col_len];//col_len = 7 or 8 or 9
                            memcpy(tempbuf,row_data+col_offset,col_len);
                            bool is_negative = false;
                            if(std::bitset<8>(tempbuf[0])[7] == 0){//음수일때 not +1
                                is_negative = true;
                                for(int i = 0; i<7; i++){
                                    tempbuf[i] = ~tempbuf[i];//not연산
                                }
                                // tempbuf[6] = tempbuf[6] +1;//+1
                            }   
                            char integer[8];
                            memset(&integer, 0, 8);
                            for(int k=0; k<5; k++){
                                integer[k] = tempbuf[5-k];
                            }

                            int64_t ivalue = *((int64_t *)integer); 
                            double rvalue;
                            if(col_len == 7){
                                char real[1];
                                real[0] = tempbuf[6];
                                rvalue = *((int8_t *)real); 
                                rvalue *= 0.01;
                            }else if(col_len == 8){
                                char real[2];
                                real[0] = tempbuf[7];
                                real[1] = tempbuf[6];
                                rvalue = *((int16_t *)real); 
                                rvalue *= 0.0001;
                            }else if(col_len == 9){
                                char real[4];
                                real[0] = tempbuf[8];
                                real[1] = tempbuf[7];
                                real[2] = tempbuf[6];
                                real[3] = 0x00;
                                rvalue = *((int32_t *)real); 
                                rvalue *= 0.000001;
                            }else{
                                KETILOG::FATALLOG("","Mysql_newdecimal>else : " + to_string(col_len));
                            }
                            double my_value = ivalue + rvalue;
                            if(is_negative){
                                my_value *= -1;
                            }
                            myWorkBuffer->table_data[col_name].floatvec.push_back(my_value);
                            break;
                        }case MySQL_DATE:{
                            char tempbuf[col_len+1];
                            memcpy(tempbuf,row_data+col_offset,col_len);
                            tempbuf[3] = 0x00;
                            int64_t my_value = *((int *)tempbuf);
                            myWorkBuffer->table_data[col_name].intvec.push_back(my_value);
                            break;
                        }case MySQL_TIMESTAMP:{
                            //아직 처리X
                            char tempbuf[col_len];
                            memcpy(tempbuf,row_data+col_offset,col_len);
                            int my_value = *((int *)tempbuf);
                            myWorkBuffer->table_data[col_name].intvec.push_back(my_value);
                            break;
                        }case MySQL_STRING:{
                            char tempbuf[col_len+1];
                            memcpy(tempbuf,row_data+col_offset,col_len);
                            tempbuf[col_len] = '\0';
                            string my_value(tempbuf);
                            myWorkBuffer->table_data[col_name].strvec.push_back(my_value);
                            break;
                        }case MySQL_VARSTRING:{
                            char tempbuf[col_len];
                            if(col_len < 258){//0~257 (_1_,___256____)
                                memcpy(tempbuf,row_data+col_offset+1,col_len-1);
                                tempbuf[col_len-1] = '\0';
                            }else{//259~65535 (__2__,_____65535______)
                                memcpy(tempbuf,row_data+col_offset+2,col_len-2);
                                tempbuf[col_len-2] = '\0';
                            }
                            string my_value(tempbuf);
                            myWorkBuffer->table_data[col_name].strvec.push_back(my_value);
                            break;
                        }default:{
                            string msg = " error>> Type: " + to_string(col_type) + " is not defined!";
                            KETILOG::FATALLOG(LOGTAG, msg);
                        }
                    }
                    myWorkBuffer->table_data[col_name].isnull.push_back(false);
                    myWorkBuffer->table_data[col_name].row_count++;
                }
            }
        }

        myWorkBuffer->left_block_count -= result.result_block_count;
        myWorkBuffer->row_count += result.row_count;
        DataBuffer_[qid]->scanned_row_count += result.scanned_row_count;
        DataBuffer_[qid]->filtered_row_count += result.filtered_row_count;
        
        KETILOG::DEBUGLOG(LOGTAG,"# Merging Data{" + to_string(qid) + "|" + to_string(wid) + "|" + myWorkBuffer->table_alias + "} ... (Left Block : " + std::to_string(myWorkBuffer->left_block_count) + ")");

        if(myWorkBuffer->left_block_count == 0){ //Work Done
            string msg = "# Merging Data {" + to_string(qid) + "|" + to_string(wid) + "|" + myWorkBuffer->table_alias + "} Done";
            KETILOG::DEBUGLOG(LOGTAG,msg);

            for(auto it = myWorkBuffer->table_data.begin(); it != myWorkBuffer->table_data.end(); it++){
                    if((*it).second.floatvec.size() != 0){
                        (*it).second.type = TYPE_FLOAT;
                    }else if((*it).second.intvec.size() != 0){
                        (*it).second.type = TYPE_INT;
                    }else if((*it).second.strvec.size() != 0){
                        (*it).second.type = TYPE_STRING;
                    }
                }

            myWorkBuffer->status = WorkDone;
            myWorkBuffer->cond.notify_all();

            break;
        }
    }
}

int BufferManager::checkTableStatus(int qid, string tname){
    if(DataBuffer_.find(qid) == DataBuffer_.end()){
        return NonInitQuery;
    }else if(DataBuffer_[qid]->tablename_workid_map.find(tname) == DataBuffer_[qid]->tablename_workid_map.end()){
        return NonInitTable;
    }else{
        return DataBuffer_[qid]->work_buffer_list[DataBuffer_[qid]->tablename_workid_map[tname]]->status;
    }
}

int BufferManager::endQuery(StorageEngineInstance::Request qid){
    if(DataBuffer_.find(qid.query_id()) == DataBuffer_.end()){
        string msg = " error>> There's No Query ID {" + to_string(qid.query_id()) + "}";
        KETILOG::FATALLOG(LOGTAG, msg);
        return 0;
    }
    DataBuffer_.erase(qid.query_id());
    return 1;
}

TableData BufferManager::getTableData(int qid, string tname){ 
    TableData tableData;

    while(1){
        int status = CheckTableStatus(qid,tname);

        if(status == NonInitQuery || status == NonInitTable){
            KETILOG::DEBUGLOG(LOGTAG,"# Buffer Not Init " + to_string(qid) + ":" + tname);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }else{
            int wid = DataBuffer_[qid]->tablename_workid_map[tname];
            WorkBuffer* workBuffer = DataBuffer_[qid]->work_buffer_list[wid];
            unique_lock<mutex> buffer_lock(workBuffer->mu);

            int status = CheckTableStatus(qid,tname);

            if(status == NotFinished){
                KETILOG::DEBUGLOG(LOGTAG,"# Not Finished " + to_string(qid) + ":" + tname);

                workBuffer->cond.wait(buffer_lock);
                tableData.table_data = workBuffer->table_data;
                tableData.valid = true;
                tableData.row_count = workBuffer->row_count;
                tableData.scanned_row_count = DataBuffer_[qid]->scanned_row_count;
                tableData.filtered_row_count = DataBuffer_[qid]->filtered_row_count;

                KETILOG::DEBUGLOG(LOGTAG,"# Finished " + to_string(qid) + ":" + tname);
                if(KETILOG::IsLogLevelUnder(TRACE)){// Debug Code 
                    cout << "<get table data>" << endl;
                    for(auto i : workBuffer->table_data){
                        cout << i.first << "|" << i.second.row_count << "|" << i.second.type << endl;
                    }
                }
                break;
            }else if(status == WorkDone){
                KETILOG::DEBUGLOG(LOGTAG,"# Done " + to_string(qid) + ":" + tname);
                tableData.table_data = workBuffer->table_data;
                tableData.valid = true;
                tableData.row_count = workBuffer->row_count;
                tableData.scanned_row_count = DataBuffer_[qid]->scanned_row_count;
                tableData.filtered_row_count = DataBuffer_[qid]->scanned_row_count;

                if(KETILOG::IsLogLevelUnder(TRACE)){// Debug Code 
                    cout << "<get table data>" << endl;
                    for(auto i : workBuffer->table_data){
                        cout << i.first << "|" << i.second.row_count << "|" << i.second.type << endl;
                    }
                }
                if(workBuffer->table_data.size() == 0){
                    cout << "what ???" << endl;
                }
                break;
            }
        }
    }
    return tableData;
}

int BufferManager::saveTableData(int qid, string tname, TableData &table_data_, int offset, int length){
    string msg = "# Save Table {" + to_string(qid) + "|" + tname + "}";
    KETILOG::DEBUGLOG(LOGTAG,msg);

    int wid = DataBuffer_[qid]->tablename_workid_map[tname];
    WorkBuffer* workBuffer = DataBuffer_[qid]->work_buffer_list[wid];
    unique_lock<mutex> lock(workBuffer->mu);

    if(length <= table_data_.row_count){
        for(auto &coldata: table_data_.table_data){
            ColData column;
            if(coldata.second.type == TYPE_STRING){
                column.strvec.assign(coldata.second.strvec.begin()+offset, coldata.second.strvec.begin()+length); 
                column.isnull.assign(coldata.second.isnull.begin()+offset, coldata.second.isnull.begin()+length);
                column.row_count = length - offset;
            }else if(coldata.second.type == TYPE_INT){
                column.intvec.assign(coldata.second.intvec.begin()+offset, coldata.second.intvec.begin()+length); 
                column.isnull.assign(coldata.second.isnull.begin()+offset, coldata.second.isnull.begin()+length);
                column.row_count = length - offset;
            }else if(coldata.second.type == TYPE_FLOAT){
                column.floatvec.assign(coldata.second.floatvec.begin()+offset, coldata.second.floatvec.begin()+length); 
                column.isnull.assign(coldata.second.isnull.begin()+offset, coldata.second.isnull.begin()+length);
                column.row_count = length - offset;
            }else if(coldata.second.type == TYPE_EMPTY){
            }else{
                KETILOG::FATALLOG(LOGTAG,"save table row type check plz... ");
            }
            column.type = coldata.second.type;
            // workBuffer->table_data.insert({coldata.first, column});
            workBuffer->table_data[coldata.first] = column;
        }
        workBuffer->row_count = length - offset;
    }else{
        DataBuffer_[qid]->work_buffer_list[wid]->table_data = table_data_.table_data;
        DataBuffer_[qid]->work_buffer_list[wid]->row_count = table_data_.row_count;
    }

    DataBuffer_[qid]->work_buffer_list[wid]->status = WorkDone;
    DataBuffer_[qid]->work_buffer_list[wid]->cond.notify_all();
    
    // Debug Code   
    // for(auto it = DataBuffer_[qid]->work_buffer_list.begin(); it != DataBuffer_[qid]->work_buffer_list.end(); it++){
    //     cout << "workID: " << (*it).first << " tableName: " << (*it).second->table_alias << endl;
    // }
    // for(auto it = DataBuffer_[qid]->table_status.begin(); it != DataBuffer_[qid]->table_status.end(); it++){
    //     cout << "tableName: " << (*it).first << " workID: " << (*it).second.first << "status: " << (*it).second.second << endl;
    // }
    
    return 1;
}

void getColOffset(const char* row_data, int* col_offset_list, vector<int> return_datatype, vector<int> table_offlen){
    int col_type = 0, col_len = 0, col_offset = 0, new_col_offset = 0, tune = 0;
    int col_count = return_datatype.size();

    for(int i=0; i<col_count; i++){
        col_type = return_datatype[i];
        col_len = table_offlen[i];
        new_col_offset = col_offset + tune;
        col_offset += col_len;
        if(col_type == MySQL_VARSTRING){
            if(col_len < 256){//0~255 -> 길이표현 1자리
                char var_len[1];
                var_len[0] = row_data[new_col_offset];
                uint8_t var_len_ = *((uint8_t *)var_len);
                tune += var_len_ + 1 - col_len;
            }else{//0~65535 -> 길이표현 2자리
                char var_len[2];
                var_len[0] = row_data[new_col_offset];
                int new_col_offset_ = new_col_offset + 1;
                var_len[1] = row_data[new_col_offset_];
                uint16_t var_len_ = *((uint16_t *)var_len);
                tune += var_len_ + 2 - col_len;
            }
        }

        col_offset_list[i] = new_col_offset;
    }
}

void calculForReturnData(StorageEngineInstance::Snippet snippet){
    unordered_map<string, int> col_type, col_offlen;
    for (int i = 0; i < snippet.table_col_size(); i++){
        col_type.insert(make_pair(snippet.table_col(i), snippet.table_datatype(i)));
        col_offlen.insert(make_pair(snippet.table_col(i), snippet.table_offlen(i)));
    }
    
    vector<int> return_datatype, return_offlen;
    for (int i = 0; i < snippet.column_projection_size(); i++){
        //tpc-h 쿼리 동작만 수행하도록 작성 => 수정필요
        if(snippet.column_projection(i).value(0) == "CASE"){
            return_datatype.push_back(2);
            return_offlen.push_back(4);
        }if(snippet.column_projection(i).value(0) == "EXTRACT"){
            return_datatype.push_back(14);
            return_offlen.push_back(3);
        }if(snippet.column_projection(i).value(0) == "SUBSTRING"){
            return_datatype.push_back(254);
            return_offlen.push_back(2);
        }else{
            if(snippet.column_projection(i).value_size() == 1){
                return_datatype.push_back(col_type[snippet.column_projection(i).value(0)]);
                return_offlen.push_back(col_offlen[snippet.column_projection(i).value(0)]);
            }else{
                int multiple_count = 0;
                for (int j = 0; j < snippet.column_projection(i).value_size(); j++){
                    if(snippet.column_projection(i).value(j) == "*"){
                        multiple_count++;
                    }
                }
                if(multiple_count == 1){
                    return_datatype.push_back(246);
                    return_offlen.push_back(8);
                }else if(multiple_count == 2){
                    return_datatype.push_back(246);
                    return_offlen.push_back(9);
                }else{
                    return_datatype.push_back(col_type[snippet.column_projection(i).value(0)]);
                    return_offlen.push_back(col_offlen[snippet.column_projection(i).value(0)]);
                }
            }
        }
    }

    //return return_datatype, return_offlen
}