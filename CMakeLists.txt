cmake_minimum_required(VERSION 3.28)
project(mystic)

set(CMAKE_CXX_STANDARD 17)

if(NOT EMSCRIPTEN)
    find_package(SDL2 CONFIG REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS} src)
else()
    include_directories(src)
endif()

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
    target_sources(mystic PRIVATE win32/mystic.rc)
    target_compile_definitions(mystic PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(mystic PRIVATE /utf-8)
    set_target_properties(mystic PROPERTIES WIN32_EXECUTABLE "$<CONFIG:Release>")
endif()

if(EMSCRIPTEN)
    target_compile_options(mystic PRIVATE -sUSE_SDL=2 -sASYNCIFY -sASSERTIONS=2 -sUSE_PTHREADS=1)
    target_link_options(mystic PRIVATE 
        -sUSE_SDL=2 
        -sUSE_SDL_MIXER=2
        -sUSE_SDL_IMAGE=2
        -sALLOW_MEMORY_GROWTH=1 
        -sASYNCIFY
        -sASSERTIONS=2
        -sSAFE_HEAP=1
        -sUSE_PTHREADS=1
        -sPTHREAD_POOL_SIZE=4
        --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets
    )
    set_target_properties(mystic PROPERTIES
        OUTPUT_NAME "index"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/docs"
        SUFFIX ".html"
    )
else()
    target_link_libraries(mystic ${SDL2_LIBRARIES})
endif()

if(NOT EMSCRIPTEN)
    set_target_properties(mystic PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"

        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:mystic>"
    )
endif()
