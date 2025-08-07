cmake_minimum_required(VERSION 3.18)

project(app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server
  server.cpp
  server.h
  server_main.cpp
  socket.cpp
  socket.h
)
