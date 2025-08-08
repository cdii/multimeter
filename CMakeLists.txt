cmake_minimum_required(VERSION 3.18)

project(app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Quick Network)

set(sanitizer_options "-fsanitize=thread")

add_executable(server
  server.cpp
  server.h
  server_main.cpp
  socket.cpp
  socket.h
  utils.h
)
target_compile_options(server PRIVATE ${sanitizer_options})
target_link_libraries(server PRIVATE ${sanitizer_options})

add_executable(client
  client.cpp
  client.h
  main.cpp
  qml.qrc
)
target_link_libraries(client PRIVATE Qt5::Core Qt5::Quick Qt5::Network)
