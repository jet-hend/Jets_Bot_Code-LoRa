cmake_minimum_required(VERSION 3.16)

project(Jets_Bot LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Export compile commands for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(ROBOTEX ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/subsystems/drivetrain.cpp 
    ${PROJECT_SOURCE_DIR}/subsystems/ESPRaido.cpp
    ${PROJECT_SOURCE_DIR}/subsystems/relayBoard.cpp
    ${PROJECT_SOURCE_DIR}/subsystems/customCANutil.cpp
    ${PROJECT_SOURCE_DIR}/subsystems/MPU6050util.cpp
    ${PROJECT_SOURCE_DIR}/commands/DirDrive.cpp 
)

# Collect all source files
file(GLOB_RECURSE SOURCES 
    "${PROJECT_SOURCE_DIR}/main.cpp"
    "${PROJECT_SOURCE_DIR}/subsystems/*.cpp"
    "${PROJECT_SOURCE_DIR}/commands/*.cpp"
)

# Create the executable
add_executable(Jets_Bot ${SOURCES})

# Link libraries
target_link_libraries(Jets_Bot 
    PRIVATE
        Threads::Threads
        Boost::system
        wiringPi
        i2c
)

# === Phoenix CTRE Libraries (ARM) ===
if(CMAKE_LIBRARY_ARCHITECTURE MATCHES "arm")
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/arm32)
else()
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/x86-64)  # fallback
endif()

target_link_libraries(Jets_Bot 
    PRIVATE
        ${PHOENIX_LIBS_DIR}/libCTRE_Phoenix.so
        ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixCCI.so
        ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixTools.so
)

# Copy Phoenix .so files to bin folder after build
add_custom_command(TARGET Jets_Bot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PHOENIX_LIBS_DIR}
        $<TARGET_FILE_DIR:Jets_Bot>
    COMMENT "Copying Phoenix libraries to output directory"
)

message(STATUS "✅ Jets_Bot configured successfully")
message(STATUS "Phoenix libs path: ${PHOENIX_LIBS_DIR}")