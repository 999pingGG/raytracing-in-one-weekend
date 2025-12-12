cmake_minimum_required(VERSION 3.17)

project(rt C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(rt libs/cvkm/cvkm.h libs/stb/stb_image_write.h src/main.c)

if(MSVC)
    target_compile_options(rt PRIVATE /W4 /WX)
else()
    target_compile_options(rt PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(rt PRIVATE include libs/cvkm libs/stb)
