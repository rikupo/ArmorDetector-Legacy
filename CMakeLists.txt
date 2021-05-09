cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(detectArmor)

set(SOURCES
    main.cpp
    armorDetector/armorDetector.cpp
    armorDetector/armorDetector.hpp
)
set(CMAKE_CXX_FLAGS "-o3")
add_executable (${PROJECT_NAME} ${SOURCES})
find_package(OpenCV 3.4 REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries (${PROJECT_NAME} ${OpenCV_LIBS})
