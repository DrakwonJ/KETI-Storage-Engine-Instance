# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build

# Include any dependencies generated for this target.
include CMakeFiles/ss_grpc_proto.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ss_grpc_proto.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ss_grpc_proto.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ss_grpc_proto.dir/flags.make

storage_engine_instance.pb.cc: /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/proto/storage_engine_instance.proto
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating storage_engine_instance.pb.cc, storage_engine_instance.pb.h, storage_engine_instance.grpc.pb.cc, storage_engine_instance.grpc.pb.h"
	/bin/protoc-3.19.4.0 --grpc_out /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build --cpp_out /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build -I /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/proto --plugin=protoc-gen-grpc="/bin/grpc_cpp_plugin" /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/proto/storage_engine_instance.proto

storage_engine_instance.pb.h: storage_engine_instance.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate storage_engine_instance.pb.h

storage_engine_instance.grpc.pb.cc: storage_engine_instance.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate storage_engine_instance.grpc.pb.cc

storage_engine_instance.grpc.pb.h: storage_engine_instance.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate storage_engine_instance.grpc.pb.h

CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.o: CMakeFiles/ss_grpc_proto.dir/flags.make
CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.o: storage_engine_instance.grpc.pb.cc
CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.o: CMakeFiles/ss_grpc_proto.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.o -MF CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.o.d -o CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.o -c /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build/storage_engine_instance.grpc.pb.cc

CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build/storage_engine_instance.grpc.pb.cc > CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.i

CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build/storage_engine_instance.grpc.pb.cc -o CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.s

CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.o: CMakeFiles/ss_grpc_proto.dir/flags.make
CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.o: storage_engine_instance.pb.cc
CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.o: CMakeFiles/ss_grpc_proto.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.o -MF CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.o.d -o CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.o -c /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build/storage_engine_instance.pb.cc

CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build/storage_engine_instance.pb.cc > CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.i

CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build/storage_engine_instance.pb.cc -o CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.s

# Object files for target ss_grpc_proto
ss_grpc_proto_OBJECTS = \
"CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.o" \
"CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.o"

# External object files for target ss_grpc_proto
ss_grpc_proto_EXTERNAL_OBJECTS =

libss_grpc_proto.a: CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.grpc.pb.cc.o
libss_grpc_proto.a: CMakeFiles/ss_grpc_proto.dir/storage_engine_instance.pb.cc.o
libss_grpc_proto.a: CMakeFiles/ss_grpc_proto.dir/build.make
libss_grpc_proto.a: CMakeFiles/ss_grpc_proto.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library libss_grpc_proto.a"
	$(CMAKE_COMMAND) -P CMakeFiles/ss_grpc_proto.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ss_grpc_proto.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ss_grpc_proto.dir/build: libss_grpc_proto.a
.PHONY : CMakeFiles/ss_grpc_proto.dir/build

CMakeFiles/ss_grpc_proto.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ss_grpc_proto.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ss_grpc_proto.dir/clean

CMakeFiles/ss_grpc_proto.dir/depend: storage_engine_instance.grpc.pb.cc
CMakeFiles/ss_grpc_proto.dir/depend: storage_engine_instance.grpc.pb.h
CMakeFiles/ss_grpc_proto.dir/depend: storage_engine_instance.pb.cc
CMakeFiles/ss_grpc_proto.dir/depend: storage_engine_instance.pb.h
	cd /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build /root/workspace/Storage-Engine-Instance/storage-engine-instance-container/mergingcontainer/cmake/build/CMakeFiles/ss_grpc_proto.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ss_grpc_proto.dir/depend

