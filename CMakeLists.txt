cmake_minimum_required(VERSION 3.2)
project(arduino_rfid_musicplayer)

set(ENV{PATH} "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/usr/local/go/bin:/home/lmw/development/go/bin:/home/lmw/.local/bin")
set(PLATFORMIO_CMD "/home/lmw/.local/bin/platformio")

include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoavr/variants/leonardo")
include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoavr/cores/arduino")
include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoavr/libraries/SPI")
include_directories("$ENV{HOME}/.platformio/packages/toolchain-atmelavr/avr/include")
include_directories("$ENV{HOME}/.platformio/packages/toolchain-atmelavr/lib/gcc/avr/4.8.1/include")
include_directories("$ENV{HOME}/.platformio/packages/toolchain-atmelavr/lib/gcc/avr/4.8.1/include-fixed")
include_directories("$ENV{HOME}/.platformio/lib/MFRC522_ID63")

add_definitions(-DF_CPU=16000000L)
add_definitions(-DARDUINO_ARCH_AVR)
add_definitions(-DARDUINO_AVR_LEONARDO)
add_definitions(-DUSB_VID=0x2341)
add_definitions(-DUSB_PID=0x8036)
add_definitions(-DUSB_PRODUCT="Arduino Leonardo")
add_definitions(-DARDUINO=10606)
add_definitions(-DPLATFORMIO=020401)
add_definitions(-D__AVR_ATmega32U4__)

add_custom_target(
    PLATFORMIO_BUILD ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_UPLOAD ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target upload
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_CLEAN ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target clean
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(arduino_rfid_musicplayer
        src/main.cpp
        src/state.cpp src/state.h
        src/RFIDReader.cpp src/RFIDReader.h
        src/MP3Player.cpp src/MP3Player.h src/utils.cpp src/utils.h)
