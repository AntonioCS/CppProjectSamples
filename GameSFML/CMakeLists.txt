cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(project-sfml CXX)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
else()
  if (CMAKE_CXX_STANDARD VERSION_LESS 20)
    message(FATAL_ERROR "CMAKE_CXX_STANDARD of 20 or later required.")
  endif()
endif()
if (NOT DEFINED CMAKE_CXX_EXTENSIONS)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if (MSVC)
	add_definitions(-D_UNICODE -DUNICODE -DWIN32_LEAN_AND_MEAN -DNOMINMAX -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
	add_definitions(-D_ATL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
endif()

add_executable(
    ${CMAKE_PROJECT_NAME}
    src/main.cpp
)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sfml-main sfml-system sfml-network sfml-graphics sfml-window)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
