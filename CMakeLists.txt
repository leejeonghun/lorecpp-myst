cmake_minimum_required(VERSION 3.28)
project(mystic)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 CONFIG REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} include)

add_executable(mystic
    src/main.cpp 
    src/uhanx.cpp
    src/mystsub.cpp
    src/mystmain.cpp
    src/mysttitl.cpp
    src/mystmenu.cpp
    src/mystbatt.cpp
    src/mystent.cpp
    src/mystcret.cpp
    src/mystend.cpp
    src/mystspec.cpp
    src/mysttalk.cpp
    src/mysttal1.cpp
    src/mysttal2.cpp
    src/mysttime.cpp
    src/voice.cpp
    src/adlib.cpp
    src/sounddrv.cpp
)

if(MSVC)
    target_compile_definitions(mystic PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(mystic PRIVATE /utf-8)
    set_target_properties(mystic PROPERTIES WIN32_EXECUTABLE "$<CONFIG:Release>")
endif()

target_link_libraries(mystic ${SDL2_LIBRARIES})

set_target_properties(mystic PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"

    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:mystic>"
)
