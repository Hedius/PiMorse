project(mcsend C)


cmake_minimum_required(VERSION 3.7)
set(CMAKE_C_STANDARD 99)

add_library(list SHARED src/list/list.h src/list/list.c)

add_library(mcSendLib SHARED src/mcSendLib.h src/mcSendLib.c)
target_link_libraries(mcSendLib list wiringPi)

add_library(threading SHARED src/threading.h src/threading.c)
target_link_libraries(threading mcSendLib list pthread)

add_executable(mcsend src/mcsend.c)
target_link_libraries(mcsend threading list wiringPi)
