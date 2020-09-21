cmake_minimum_required(VERSION 3.13)
project(systemID_updater C)
set(CMAKE_C_STANDARD 99)

find_package(ZLIB)

add_executable(systemID_updater main.c eeprom/eeprom.c eeprom/eeprom.h crc/crc.c crc/crc.h)

include_directories(eeprom)
include_directories(crc)

target_link_libraries(systemID_updater z)

install(TARGETS systemID_updater DESTINATION bin)