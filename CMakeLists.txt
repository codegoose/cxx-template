cmake_minimum_required(VERSION 2.8.12)

project(app)

if(MINGW)
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
    SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(
    app
    src/app.cxx
    src/cg/os/win.cxx
    src/rc/app.rc
)

target_link_libraries(app ${CONAN_LIBS})