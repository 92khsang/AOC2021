cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_CXX_STANDARD 11)

set(TARGET Day25SeaCucumber)

add_executable(${TARGET})

target_compile_definitions("${TARGET}"
        PRIVATE
        INPUT_FILE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/input.txt"
        )

target_sources(${TARGET}
        PRIVATE
        "main.cpp"
        )
