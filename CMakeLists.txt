cmake_minimum_required(VERSION 3.10)

# Set project name and language
project(ifj_projekt C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Add source files
set(SOURCES
        main.c
        scanner.c
        parser.c
        visualization.c
)

# Add header files
set(HEADERS
        scanner.h
        parser.h
        visualization.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Set output directory
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)