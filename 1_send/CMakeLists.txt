project(mcsend C)

set(CMAKE_C_STANDARD 99)

add_library(mcSendLib STATIC mcSendLib.h mcSendLib.c)
target_link_libraries(mcSendLib wiringPi)

add_executable(mcsend mcsend.c)
target_link_libraries(mcsend mcSendLib)
