cmake_minimum_required(VERSION 3.16)

project(Svg2Path VERSION 0.0.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

find_package(juce REQUIRED)

option(UniversalBinary "Build universal binary for mac" OFF)

if (UniversalBinary)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
endif ()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
juce_add_gui_app(Svg2Path PRODUCT_NAME "Svg2Path")

target_sources(Svg2Path PRIVATE
        Source/Main.cpp
        Source/SvgParser.cpp
        Source/MainComponent.cpp)

juce_generate_juce_header(Svg2Path)

target_compile_definitions(Svg2Path PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0)

target_link_libraries(Svg2Path PRIVATE
        juce_gui_extra
        juce_build_tools)
