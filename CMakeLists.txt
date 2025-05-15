cmake_minimum_required (VERSION 3.8)

project (Bezier_renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin.dbg)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin.rel)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin.relwithdbg)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-16>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-16>")

find_package(Eigen3 REQUIRED)
find_package(SFML COMPONENTS Graphics Window CONFIG REQUIRED)

add_subdirectory(Bezier_curves)

add_executable (Bezier_renderer Renderer.cpp)
target_link_libraries(Bezier_renderer
Bezier_curves
Eigen3::Eigen
)