cmake_minimum_required(VERSION 3.10)
project(AdaptiveImageRestoration)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(image_pipeline
    src/main.cpp
    src/degradation.cpp
    src/filters.cpp
    src/metrics.cpp
    src/pipeline.cpp
    src/io.cpp
    src/timer.cpp
)

target_link_libraries(image_pipeline ${OpenCV_LIBS})
