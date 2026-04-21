cmake_minimum_required(VERSION 3.20)
project(MeridianRenderEngine VERSION 0.34.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

file(GLOB_RECURSE MERIDIAN_RUNTIME_HEADERS CONFIGURE_DEPENDS include/Meridian/Core/*.h include/Meridian/Renderer/*.h)
file(GLOB_RECURSE MERIDIAN_RUNTIME_SRC CONFIGURE_DEPENDS src/Core/*.cpp src/Renderer/*.cpp)
file(GLOB_RECURSE MERIDIAN_APPLICATION_HEADERS CONFIGURE_DEPENDS include/Meridian/Application/*.h)
file(GLOB_RECURSE MERIDIAN_APPLICATION_SRC CONFIGURE_DEPENDS src/Application/*.cpp)
file(GLOB_RECURSE MERIDIAN_EDITOR_HEADERS CONFIGURE_DEPENDS include/Meridian/EditorFramework/*.h)
file(GLOB_RECURSE MERIDIAN_EDITOR_SRC CONFIGURE_DEPENDS src/EditorFramework/*.cpp)

add_library(MeridianRuntime STATIC ${MERIDIAN_RUNTIME_HEADERS} ${MERIDIAN_RUNTIME_SRC})
target_include_directories(MeridianRuntime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(MeridianEditorFramework STATIC ${MERIDIAN_EDITOR_HEADERS} ${MERIDIAN_EDITOR_SRC})
target_include_directories(MeridianEditorFramework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MeridianEditorFramework PUBLIC MeridianRuntime)
if(WIN32)
    target_compile_definitions(MeridianEditorFramework PRIVATE UNICODE _UNICODE)
    target_link_libraries(MeridianEditorFramework PRIVATE user32 gdi32)
endif()

add_library(MeridianApplication STATIC ${MERIDIAN_APPLICATION_HEADERS} ${MERIDIAN_APPLICATION_SRC})
target_include_directories(MeridianApplication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MeridianApplication PUBLIC MeridianEditorFramework MeridianRuntime)
if(WIN32)
    target_compile_definitions(MeridianApplication PRIVATE UNICODE _UNICODE)
    target_link_libraries(MeridianApplication PRIVATE user32 gdi32)
endif()

add_executable(MeridianStudio tools/meridian_studio_main.cpp)
target_link_libraries(MeridianStudio PRIVATE MeridianApplication MeridianEditorFramework)

add_executable(MeridianHeadlessValidation tools/headless_validation_main.cpp)
target_link_libraries(MeridianHeadlessValidation PRIVATE MeridianRuntime)

add_executable(MeridianUnitTests tests/unit_tests.cpp)
target_link_libraries(MeridianUnitTests PRIVATE MeridianRuntime)

add_executable(MeridianRegressionTests tests/regression_tests.cpp)
target_link_libraries(MeridianRegressionTests PRIVATE MeridianRuntime)

add_test(NAME MeridianUnitTests COMMAND MeridianUnitTests)
add_test(NAME MeridianRegressionTests COMMAND MeridianRegressionTests)

install(TARGETS MeridianRuntime MeridianEditorFramework MeridianApplication MeridianStudio MeridianHeadlessValidation MeridianUnitTests MeridianRegressionTests
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_NAME "MeridianRenderEngine")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
include(CPack)
