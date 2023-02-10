cmake_minimum_required(VERSION 3.12)

### Project name and a few useful settings. Other commands can pick up the results ###
project(
  razan
  DESCRIPTION "razan is a real-time, symmetrically encrypted messaging service, facilitated by a central server."
  LANGUAGES CXX C
  VERSION 1.0
)

### Set where CMake will output files ###
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


### Configure relevant toolchains used to compile ###
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED True)
add_compile_options(-Wall -Wextra -pedantic -g -Wall -fPIC -fno-exceptions)
add_compile_definitions(OPENSSL_PIC DEFAULT_PORT=4242 DEV_MODE)
set(LINK_LIBRARIES "/usr/local/lib64/libcrypto.a -lm -lpthread -lncurses -ldl -lstdc++ -lssl -lcrypto -lz")
include_directories(
	/usr/include/openssl
	include/
)

### Configure any related projects commands (e.g. creating documentation) ###
#find_package(Doxygen)
#if(Doxygen_FOUND)
#  execute_process(COMMAND doxygen .config.txt)
#else()
#  message(STATUS "Doxygen tool not found, not building additional source code docs")
#endif()


### Now actually compile ###
add_library(razan_logger src/logger.c)
add_library(razan_encryption src/encryption.cc)
add_library(razan_client_management src/client_management.cc)
add_library(razan_client_chat src/client_chat.cc)

add_executable(razan_server src/razan_server.cc) # Adding something we can run - Output name matches target name
target_link_libraries(razan_server razan_client_management razan_encryption razan_logger /usr/local/lib64/libcrypto.a -lm -lpthread -lncurses -ldl -lstdc++ -lssl -lcrypto -lz) # Make sure you link your targets with this command. It can also link libraries and
									      # even flags, so linking a target that does not exist will not give a configure-time error

add_executable(razan_client src/razan_client.cc)
target_link_libraries(razan_client razan_client_chat razan_client_management razan_encryption razan_logger /usr/local/lib64/libcrypto.a -lm -lpthread -lncurses -ldl -lstdc++ -lssl -lcrypto -lz)
set_target_properties(razan_client PROPERTIES OUTPUT_NAME razan)
