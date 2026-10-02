cmake_minimum_required(VERSION 3.11.3)

set(CMAKE_CXX_STANDARD 17)

project(Membot)

set(PROJECT_SRCS src/main_cli.cpp src/chatlogic.cpp src/chatbot.cpp src/graphedge.cpp src/graphnode.cpp)
add_executable(membot ${PROJECT_SRCS})
