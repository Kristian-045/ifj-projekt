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

# Add source files for the main executable
set(SOURCES
        main.c
        scanner.c
        parser.c
        visualization.c
        symtable.c
        semantic_analysis.c
        frame_list.c
        codegenerator.c
        builtinfun.c
)

# Add header files
set(HEADERS
        scanner.h
        parser.h
        visualization.h
        symtable.h
        semantic_analysis.h
        frame_list.h
        codegenerator.h
        builtinfun.h
)

# Create main executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link the math library to the main executable
target_link_libraries(${PROJECT_NAME} m)

# Set output directory for main executable
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Create symtable test executable
add_executable(symtable_test_${PROJECT_NAME} tests/symtable_test.c symtable.c)
add_executable(semantic_test_${PROJECT_NAME} tests/semantic_test.c scanner.c parser.c visualization.c semantic_analysis.c symtable.c frame_list.c)
target_link_libraries(semantic_test_${PROJECT_NAME} m)
# Link the math library to the symtable test executable
target_link_libraries(symtable_test_${PROJECT_NAME} m)

# Add test source files (excluding main.c)
set(TEST_SOURCES
        tests/test1.c
)

# Create a test executable without including main.c
add_executable(test_${PROJECT_NAME} ${TEST_SOURCES} scanner.c parser.c visualization.c)

# Link the math library to the test executable
target_link_libraries(test_${PROJECT_NAME} m)

# Set output directory for test executable
set_target_properties(test_${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)
