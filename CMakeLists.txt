cmake_minimum_required(VERSION 2.8.4)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME bioZAP)
project(${PROJECT_NAME})

set(${CMAKE_PROJECT_NAME}_ARDLIBS  Keypad LiquidCrystal_I2C DS1803 AD9850 )

set(${CMAKE_PROJECT_NAME}_SRCS bioZAP_func.cpp)
set(${CMAKE_PROJECT_NAME}_HDRS multiZAP_def.h bioZAP_func.h bioZAP.h)
set(${CMAKE_PROJECT_NAME}_BOARD nano)
set(nano.build.mcu atmega328p)
set(nano.upload.protocol arduino)
set(nano.upload.speed 57600)

set(${CMAKE_PROJECT_NAME}_PORT COM3)


generate_arduino_library(${CMAKE_PROJECT_NAME})
