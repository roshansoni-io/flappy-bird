cmake_minimum_required(VERSION 3.16)

project(flappy_bird CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Path to a local prebuilt raylib (binaries + headers + minshell.html for web)
set(RAYLIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/raylib" CACHE PATH
    "Path to the prebuilt raylib (headers and libs)")

add_executable(flappy main.cpp)

target_include_directories(flappy PRIVATE "${RAYLIB_PATH}")

if(EMSCRIPTEN)
    # ---- Web build (WASM) --------------------------------------------
    #   cmake -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake .
    target_compile_definitions(flappy PRIVATE PLATFORM_WEB)

    target_compile_options(flappy PRIVATE -Wall -Wextra)

    target_link_libraries(flappy PRIVATE "${RAYLIB_PATH}/web/libraylib.web.a")

    target_link_options(flappy PRIVATE
        "-sUSE_GLFW=3"
        "-sALLOW_MEMORY_GROWTH=1"
        "--shell-file=${RAYLIB_PATH}/minshell.html"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
    )

    set_target_properties(flappy PROPERTIES SUFFIX ".html")
else()
    # ---- Native build --------------------------------------------------
    target_compile_options(flappy PRIVATE -Wall -Wextra)

    # Embed the absolute asset dir so textures load regardless of CWD.
    # (Web keeps the VFS-relative "assets" path.)
    target_compile_definitions(flappy PRIVATE ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")

    find_package(raylib QUIET)
    if(raylib_FOUND)
        target_link_libraries(flappy PRIVATE raylib)
    else()
        target_link_libraries(flappy PRIVATE
            "${RAYLIB_PATH}/libraylib.a"
            X11 m pthread dl
        )
    endif()
endif()
