cmake_minimum_required(VERSION 3.20)
project(TaskTracker)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler flags for better C++23 support
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add main executable
add_executable(TaskTracker
    main.cpp
    task.cpp
    app.cpp
)

# Add test executable for C++23 demos
# add_executable(TestCpp23
#     test.cpp
# )

# Link libraries if needed
target_link_libraries(TaskTracker)
# target_link_libraries(TestCpp23)

# Set target properties for both executables
set_target_properties(TaskTracker PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# set_target_properties(TestCpp23 PROPERTIES
#     CXX_STANDARD 23
#     CXX_STANDARD_REQUIRED ON
#     CXX_EXTENSIONS OFF
# )

# Set target properties
set_target_properties(TaskTracker PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
