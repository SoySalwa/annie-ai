cmake_minimum_required(VERSION 3.10)
project(AnnieAI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(CURL REQUIRED)

add_executable(annieai 
src/main.cc 
)

target_include_directories(annieai PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(annieai PRIVATE ${CURL_LIBRARIES})

install(TARGETS annieai DESTINATION bin)
