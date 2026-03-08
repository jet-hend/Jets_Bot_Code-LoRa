# Project's name
PROJECT ( ROBOTEX C CXX )

# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.1)

add_definitions(-std=c++17)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package (Threads REQUIRED)
find_package (Boost REQUIRED)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable(ROBOTEX ${PROJECT_SOURCE_DIR}/robot.cpp 
                    ${PROJECT_SOURCE_DIR}/subsystems/drivetrain.cpp 
                    ${PROJECT_SOURCE_DIR}/subsystems/ESPRadio.cpp
                    ${PROJECT_SOURCE_DIR}/subsystems/relayBoard.cpp
                    ${PROJECT_SOURCE_DIR}/subsystems/customCANutil.cpp
                    ${PROJECT_SOURCE_DIR}/subsystems/MPU6050util.cpp
                    ${PROJECT_SOURCE_DIR}/commands/DirDrive.cpp 
                    )

if (${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "arm-linux-gnueabihf") # Jetson TK1 / Pi
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/arm32)
elseif (${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "aarch64-linux-gnu") # Jetson TX2
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/arm64)
else()
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/x86-64)
endif()

target_link_libraries(ROBOTEX ${PHOENIX_LIBS_DIR}/libCTRE_Phoenix.so)
target_link_libraries(ROBOTEX ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixCCI.so)
target_link_libraries(ROBOTEX ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixTools.so)

target_link_libraries(ROBOTEX Threads::Threads)
target_link_libraries(ROBOTEX Boost::headers)
target_link_libraries(ROBOTEX wiringPi)
target_link_libraries(ROBOTEX i2c)

add_custom_command(TARGET ROBOTEX POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PHOENIX_LIBS_DIR}
        $<TARGET_FILE_DIR:ROBOTEX>)
