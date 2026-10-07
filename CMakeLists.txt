cmake_minimum_required(VERSION 3.24)

project(NameGenerator VERSION 0.1.1 DESCRIPTION "Reusable procedural name generator" LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(NAME_GENERATOR_BUILD_TESTS "Build NameGenerator tests" ON)
option(NAME_GENERATOR_INSTALL "Enable install and package-export rules" ON)

add_library(name_generator src/NameGenerator.cpp)
add_library(NameGenerator::NameGenerator ALIAS name_generator)
set_target_properties(name_generator PROPERTIES EXPORT_NAME NameGenerator)
target_compile_features(name_generator PUBLIC cxx_std_20)
target_include_directories(name_generator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(EMSCRIPTEN)
    add_executable(name_generator_wasm src/NameGeneratorWasm.cpp)
    target_link_libraries(name_generator_wasm PRIVATE NameGenerator::NameGenerator)
    target_link_options(name_generator_wasm PRIVATE
        --bind
        -sMODULARIZE=1
        -sEXPORT_ES6=1
        -sEXPORT_NAME=createNameGenerator
        -sENVIRONMENT=web
        -sALLOW_MEMORY_GROWTH=1)
    set_target_properties(name_generator_wasm PROPERTIES
        OUTPUT_NAME name_generator
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/web/public/wasm")
else()
    add_executable(name_generator_cli src/NameGeneratorCli.cpp)
    target_link_libraries(name_generator_cli PRIVATE NameGenerator::NameGenerator)
endif()

if(MSVC)
    target_compile_options(name_generator PRIVATE /W4 /permissive- /utf-8 /Zc:__cplusplus)
else()
    target_compile_options(name_generator PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(NAME_GENERATOR_BUILD_TESTS)
    enable_testing()
    add_executable(name_generator_tests tests/NameGeneratorTests.cpp)
    target_link_libraries(name_generator_tests PRIVATE NameGenerator::NameGenerator)
    add_test(NAME name_generator_tests COMMAND name_generator_tests)
endif()

configure_package_config_file(
    cmake/NameGeneratorConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/NameGeneratorConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/NameGenerator")
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NameGeneratorConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion)

if(NAME_GENERATOR_INSTALL)
    install(TARGETS name_generator EXPORT NameGeneratorTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
    install(EXPORT NameGeneratorTargets
        FILE NameGeneratorTargets.cmake
        NAMESPACE NameGenerator::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/NameGenerator")
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/NameGeneratorConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/NameGeneratorConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/NameGenerator")
endif()
