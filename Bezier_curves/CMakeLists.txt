add_library(Bezier_curves
Bezier_curves.cpp
Bezier_curves.hpp
)

set_target_properties(Bezier_curves PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET Bezier_curves PROPERTY CXX_STANDARD 20)

target_link_libraries(Bezier_curves
Eigen3::Eigen
SFML::Graphics
SFML::Window
)

target_include_directories(Bezier_curves PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS Bezier_curves)