cmake_minimum_required(VERSION 3.15)

project(
    TextEditor
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

include(FetchContent)

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG 913151caa8905370229e361486b10c890aeaed45
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)
FetchContent_MakeAvailable(imgui)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/cpp)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/imgui ${PROJECT_NAME}_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src ${PROJECT_NAME}_LIST)
list(APPEND ${PROJECT_NAME}_LIST "${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/cpp/imgui_stdlib.cpp")
list(APPEND ${PROJECT_NAME}_LIST "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_win32.cpp")
list(APPEND ${PROJECT_NAME}_LIST "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_dx9.cpp")

add_executable(${PROJECT_NAME} WIN32 ${${PROJECT_NAME}_LIST})

if(NOT "${DXSDK_DIR}" STREQUAL "")
    target_include_directories( ${PROJECT_NAME} PUBLIC ${DXSDK_DIR}/Include)
    target_link_directories(${PROJECT_NAME} PUBLIC ${DXSDK_DIR}/Lib/x86)
elseif(NOT "$ENV{DXSDK_DIR}" STREQUAL "")
    target_include_directories( ${PROJECT_NAME} PUBLIC $ENV{DXSDK_DIR}/Include)
    target_link_directories( ${PROJECT_NAME} PUBLIC $ENV{DXSDK_DIR}/Lib/x86)
endif()

target_link_libraries(${PROJECT_NAME} d3d9 d3dx9)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)