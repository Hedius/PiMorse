project(mcrecv C)

cmake_minimum_required(VERSION 3.7)
set(CMAKE_C_STANDARD 99)

add_library(list SHARED src/list/list.h src/list/list.c)
target_link_libraries(list pthread)

add_library(morseReceive SHARED src/morseReceive.h src/morseToAsciiTable.h src/morseReceive.c)
target_link_libraries(morseReceive list wiringPi)

add_library(threading SHARED src/threading.h src/threading.c)
target_link_libraries(threading morseReceive list)

add_executable(mcrecv src/main.c)
target_link_libraries(mcrecv threading list)
