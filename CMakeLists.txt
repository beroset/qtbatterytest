cmake_minimum_required(VERSION 3.15)
project(battery-test)
find_package(Qt6 COMPONENTS Bluetooth REQUIRED)
add_library(asteroid STATIC
    watch.cpp
    watchconnection.cpp
    scanner.cpp
    batteryservice.cpp
)
target_compile_features(asteroid PUBLIC cxx_std_17)
set_target_properties(asteroid PROPERTIES AUTOMOC ON)
target_compile_options(asteroid PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(asteroid PRIVATE Qt6::Bluetooth)
install(TARGETS asteroid)

add_executable(battery-test main.cpp controller.cpp)
set_target_properties(battery-test PROPERTIES AUTOMOC ON)
target_include_directories(battery-test PRIVATE libasteroid/src)
target_compile_features(battery-test PUBLIC cxx_std_17)
target_link_libraries(battery-test PRIVATE Qt6::Bluetooth asteroid)
install(TARGETS battery-test)
