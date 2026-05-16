cmake_minimum_required(VERSION 3.16)

project(Jets_Bot LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

include_directories(${PROJECT_SOURCE_DIR}/include)

# === Source files (explicit for now) ===
add_executable(Jets_Bot 
    main.cpp
    subsystems/drivetrain.cpp
    subsystems/ESPRadio.cpp
    subsystems/relayBoard.cpp
    subsystems/customCANutil.cpp
    subsystems/MPU6050util.cpp
    commands/MOVE_CMD/driveCMDs.cpp
)

target_link_libraries(Jets_Bot 
    PRIVATE
        Threads::Threads
        Boost::system
        wiringPi
        i2c
)

# Phoenix CTRE Libraries
if(CMAKE_LIBRARY_ARCHITECTURE MATCHES "arm")
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/arm32)
else()
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/x86-64)
endif()

target_link_libraries(Jets_Bot 
    PRIVATE
        ${PHOENIX_LIBS_DIR}/libCTRE_Phoenix.so
        ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixCCI.so
        ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixTools.so
)

add_custom_command(TARGET Jets_Bot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PHOENIX_LIBS_DIR}
        $<TARGET_FILE_DIR:Jets_Bot>
    COMMENT "Copying Phoenix libraries"
)

message(STATUS "✅ Jets_Bot configured successfully")