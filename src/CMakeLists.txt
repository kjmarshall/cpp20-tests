project(cpp20-tests)

#include(modules.cmake)

#add_modules_library(basicPlane-lib 
#"BasicPlane.Figures-Point.ixx"
#"BasicPlane.Figures-Rectangle.ixx"
#"BasicPlane.Figures-Rectangle.cpp"
#"BasicPlane.Figures.ixx")
#
#target_compile_features(basicPlane-lib PUBLIC cxx_std_20 )
#set_target_properties(basicPlane-lib PROPERTIES CXX_EXTENSIONS OFF)

# TODO KM: switching the order of the module cpp lets us build it????????????????????
add_executable(${PROJECT_NAME}
"BasicPlane.Figures-Point.ixx"
"BasicPlane.Figures-Rectangle.ixx"
"BasicPlane.Figures.ixx"
"BasicPlane.Figures-Rectangle.cpp"
"cpp20-tests.cpp"
)

#target_link_libraries(${PROJECT_NAME} PUBLIC basicPlane-lib)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20 )
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR} )