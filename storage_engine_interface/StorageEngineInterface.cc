#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "GenericQueryConnector.h"
#include "MergingModuleConnector.h"
#include "OffloadingModuleConnector.h"
#include "ip_config.h"

#include <grpcpp/grpcpp.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::Status;

using StorageEngineInstance::StorageEngineInterface;
using StorageEngineInstance::SnippetRequest;
using StorageEngineInstance::QueryStringResult;
using StorageEngineInstance::Response;
using StorageEngineInstance::CSDMetricList;

using namespace std;

void SendQueryStatus(const char* message){
  int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        // return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        // return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        // return -1;
    }

    // Send message to the server
    send(sock, message, strlen(message), 0);
    std::cout << "Query info sent" << std::endl;

    // Receive message from the server
    valread = read(sock, buffer, 1024);
    std::cout << buffer << std::endl;
}

// Logic and data behind the server's behavior.
class StorageEngineInterfaceServiceImpl final : public StorageEngineInterface::Service {
  Status OffloadingQueryInterface(ServerContext* context, ServerReader<SnippetRequest>* stream, QueryStringResult* result) override {
    SnippetRequest snippet_request;
    bool flag = true;

    if(KETILOG::IsLogLevel(METRIC)){
      const char* Qstart = "Query Start";
      SendQueryStatus(Qstart);
    }

    while (stream->Read(&snippet_request)) {      
      if(flag){
        string msg = "==:Set Snippet:== {" + to_string(snippet_request.snippet().query_id()) + "}";
        KETILOG::INFOLOG("Interface", msg);
        flag = false;
      } 

      // Check Recv Snippet
      {
        std::string test_json;
        google::protobuf::util::JsonPrintOptions options;
        options.always_print_primitive_fields = true;
        options.always_print_enums_as_ints = true;
        google::protobuf::util::MessageToJsonString(snippet_request,&test_json,options);
        std::cout << endl << test_json << std::endl << std::endl; 
      }

      if(snippet_request.type() == StorageEngineInstance::SnippetRequest::CSD_SCAN_SNIPPET){
        KETILOG::DEBUGLOG("Interface","# Send Snippet to Offloading Module");
        Offloading_Module_Connector offloadingModule(grpc::CreateChannel((std::string)LOCALHOST+":"+std::to_string(SE_MERGING_PORT), grpc::InsecureChannelCredentials()));
        offloadingModule.Scheduling(snippet_request.snippet());
      }else{
        KETILOG::DEBUGLOG("Interface","# Send Snippet to Merging Module");
        Merging_Module_Connector mergingModule(grpc::CreateChannel((std::string)LOCALHOST+":"+std::to_string(SE_MERGING_PORT), grpc::InsecureChannelCredentials()));
        mergingModule.Aggregation(snippet_request);
      }
    }

    Merging_Module_Connector mergingModule(grpc::CreateChannel((std::string)LOCALHOST+":"+std::to_string(SE_MERGING_PORT), grpc::InsecureChannelCredentials()));
    cout << "@ " << snippet_request.snippet().query_id() <<  snippet_request.snippet().table_alias() << endl;
    QueryStringResult result_ = mergingModule.GetQueryResult(snippet_request.snippet().query_id(), snippet_request.snippet().table_alias());

    cout << "@@ " << result_.query_result() << endl;

    return Status::OK;
  }

  Status GenericQueryInterface(ServerContext *context, const Request *request, Response *response) override {

    return Status::OK;
  }
};

void RunGRPCServer() {
  std::string server_address((std::string)LOCALHOST+":"+std::to_string(SE_INTERFACE_PORT));
  StorageEngineInterfaceServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());

  KETILOG::WARNLOG("Interface", "Interface Server listening on "+server_address);
  
  server->Wait();
}


int main(int argc, char** argv) {
  if (argc >= 2) {
      KETILOG::SetLogLevel(stoi(argv[1]));
  }else if (getenv("LOG_LEVEL") != NULL){
      string env = getenv("LOG_LEVEL");
      int log_level;
      if (env == "TRACE"){
          log_level = DEBUGG_LEVEL::TRACE;
      }else if (env == "DEBUG"){
          log_level = DEBUGG_LEVEL::DEBUG;
      }else if (env == "INFO"){
          log_level = DEBUGG_LEVEL::INFO;
      }else if (env == "WARN"){
          log_level = DEBUGG_LEVEL::WARN;
      }else if (env == "ERROR"){
          log_level = DEBUGG_LEVEL::ERROR;
      }else if (env == "FATAL"){
          log_level = DEBUGG_LEVEL::FATAL;
      }else{
          log_level = DEBUGG_LEVEL::INFO;
      }
      KETILOG::SetLogLevel(log_level);
  }else{
      KETILOG::SetDefaultLogLevel();
  }

  std::thread grpc_thread(RunGRPCServer);

  httplib::Server server;
  server.Get("/log-level", KETILOG::HandleSetLogLevel);
  server.listen("0.0.0.0", 40206);
  
  grpc_thread.join();

  return 0;
}