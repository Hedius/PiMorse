project(mcrecv C)

set(CMAKE_C_STANDARD 99)

add_library(libMorseReceive STATIC libMorseReceive.h MorseToAsciiTable.h libMorseReceive.c)
target_link_libraries(libMorseReceive wiringPi)

add_executable(mcrecv mcrecv.c)
target_link_libraries(mcrecv libMorseReceive)
