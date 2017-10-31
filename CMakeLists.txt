#=============================================================================#
# cmake arduino cloned from https://github.com/ncasaril/arduino-cmake.git     #
#=============================================================================#
cmake_minimum_required(VERSION 2.8)
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

project(SmartCharger C CXX)
SET(VERSION 0.2)

#
#
include(gitRev.cmake)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
#
set(ARDUINO_DEFAULT_BOARD nano)        # Default Board ID, when not specified
set(ARDUINO_DEFAULT_PORT /dev/ttyUSB0) # Default Port, when not specified
set(ARDUINO_CPU          atmega328)

# this is needed for the nano, it should be set automatically, maybe something changed with sdk 1.8.x ?
set(ARDUINO_CPUMENU .menu.cpu.${ARDUINO_CPU})
#
print_board_list()
print_programmer_list()

PRINT_SETTINGS(${ARDUINO_DEFAULT_BOARD})

# Where you store your downloaded libs or manually installed
SET(CUSTOM_LIB_DIR /home/fx/Arduino/libraries)

link_directories( ${CUSTOM_LIB_DIR})
set(U8glib_RECURSE True)
#
# Be careful with the includes in the ino files
# #include <foo.h> // this is a comment
# will fail
# same with extra tabs or space after the #include "blah.h"
#
SET(libPrefix ${ARDUINO_DEFAULT_BOARD}_)

ADD_DEFINITIONS(-DUSING_CMAKE)

GET_GIT_REV( ${CMAKE_SOURCE_DIR} gitRev)
CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_BINARY_DIR}/chargerVersion.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}) 



#

generate_arduino_firmware(smartCharger 
                SKETCH .              # Folder to get the .ino from, must follow DIR_NAME/DIR_NAME.ino patten !
                LIBS ${libPrefix}Adafruit_INA219 
                LIBS ${libPrefix}Adafruit_ST7735 
                LIBS ${libPrefix}Adafruit_GFX 
                LIBS ${libPrefix}Wire 
                LIBS ${libPrefix}SPI 
                PORT ${ARDUINO_DEFAULT_PORT}
                )

# EOF
