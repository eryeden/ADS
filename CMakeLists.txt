#For OpenGL testing

cmake_minimum_required(VERSION 3.5)
project(ADS)

#For auto-complete
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#output make commands
set(CMAKE_VERBOSE_MAKEFILE ON)


#COMPILE COMMANDS configuration
set(CMAKE_C_COMPILER  /usr/local/bin/gcc-5)
set(CMAKE_CXX_COMPILER /usr/local/bin/g++-5)
#CLIONに付属しているCMAKEは"/usr/local/bin"までパスが通っていないため、g++-5だけでは、コマンドのドライブができない。なので、リンクの場所までパスを張る。
#set(CMAKE_C_COMPILER  gcc-5)
#set(CMAKE_CXX_COMPILER g++-5)

#Libraries configurations ##############################################################
find_package(PkgConfig REQUIRED)

#For OpenGL libraries
#GLFW
pkg_search_module(GLFW REQUIRED glfw3)
#assimp
find_package(ASSIMP REQUIRED)

#for OSX
find_package(GLEW REQUIRED)

#for framework
find_package(OpenGL REQUIRED)
#######################################################################################


#Linker option configurations##########################################################
#Libraries for GLFW
set(GLFW_STATIC_LIBS "-L/usr/local/lib -lglfw3 -framework Cocoa -framework IOKit -framework CoreFoundation -framework CoreVideo -framework Carbon")
#For assimps
set(ASSIMPS_LIBS "-L/usr/local/lib -lassimp -lz")
#For glew
set(GLEW_LIBS "-lglew")
#For opengls
set(GL_PKGLIBS ${OPENGL_LIBRARIES})

set(GL_LIBS ${GLFW_STATIC_LIBS} ${ASSIMPS_LIBS} ${GLEW_LIBS} ${GL_PKGLIBS})


#######################################################################################

#Includes configuration ###############################################################
#OpenGL
set(GL_INCLUDE ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})
# Commons
set(INCLUDES ${PROJECT_SOURCE_DIR}/include)
set(COMMON_INCLUDE ${PROJECT_SOURCE_DIR}/common)

INCLUDE_DIRECTORIES(
        ${GL_INCLUDE}
        ${INCLUDES}
        ${COMMON_INCLUDE}
)
#######################################################################################

#Sources configuration ################################################################
file(GLOB  SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB COMMON_SRCS ${PROJECT_SOURCE_DIR}/common/*.cpp)

set(SRCS_ALL ${SRCS} ${COMMON_SRCS})
#######################################################################################

#C flags configurations ###############################################################

#Support C++14 (only g++ > 5.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#######################################################################################


add_executable(${PROJECT_NAME} ${SRCS_ALL})

target_link_libraries(${PROJECT_NAME} ${GL_LIBS})