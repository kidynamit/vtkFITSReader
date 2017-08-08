cmake_minimum_required(VERSION 3.0.2)

project(vtkFitsReader)

find_package(VTK REQUIRED)

include(${VTK_USE_FILE})
include("FindCFitsIO.cmake")

set(SOURCE_DIR  "${CMAKE_SOURCE_DIR}/src")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/test")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# OpenGL
FIND_PACKAGE(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)


include_directories(
	include
	${CFITSIO_INCLUDE_DIR}
)

set(SOURCES 
    	${TEST_DIR}/fits.cxx
    	${SOURCE_DIR}/vtkFitsReader.cxx
    	${INCLUDE_DIR}/vtkFitsReader.h
)

add_executable(vtkFitsReader ${SOURCES})
target_link_libraries(vtkFitsReader 
	${VTK_LIBRARIES}
)