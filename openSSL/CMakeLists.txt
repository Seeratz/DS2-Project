cmake_minimum_required(VERSION 3.15)
project(SecureMessagingSystem)

# Enable C++17 or later
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set vcpkg toolchain
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/Users/Habib/Downloads/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Add source files
add_executable(SecureMessaging main.cpp trapdoor.cpp)

# Link OpenSSL
target_link_libraries(SecureMessaging PRIVATE OpenSSL::SSL OpenSSL::Crypto)
