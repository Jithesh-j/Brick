cmake_minimum_required(VERSION 3.27)
project(ponggame)


set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(Apps)
include_directories(inc)
include_directories(inc/GL)
include_directories(inc/GLFW)
include_directories(inc/freetype)
include_directories(inc/glm)
include_directories(lib)
include_directories(glad)
include_directories(GLUT)
# Add the directory where the GLUT header files are located
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
include_directories(${FREEGLUT_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR} /System/Library/Frameworks/GLUT.framework/Headers)

# Link with the OpenGL and GLUT framework
link_directories(/System/Library/Frameworks)
link_libraries(/opt/homebrew/Cellar/glfw/3.4/lib/libglfw.3.dylib)

add_executable(ponggame
        lib/glad.c
#        lib/Letters.cpp
 #       lib/Draw.cpp
#        lib/Camera.cpp
#        lib/Mesh.cpp
#        lib/Quaternion.cpp
#        lib/Text.cpp
#        lib/IO.cpp
#        lib/GLXtras.cpp
        apps/bricks.cpp)

#target_link_libraries(ponggame "-framework OpenGL")
target_link_libraries(ponggame ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
target_link_libraries(ponggame "-framework OpenGL" "-framework GLUT")
target_link_libraries(ponggame ${FREEGLUT_LIBRARIES})