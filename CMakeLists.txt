cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project (PGURE-SVT)
set (PGURE-SVT_VERSION_MAJOR 0)
set (PGURE-SVT_VERSION_MINOR 3)
set (PGURE-SVT_VERSION_PATCH 0)

message(STATUS "Configuring PGURE-SVT ${PGURE-SVT_VERSION_MAJOR}.${PGURE-SVT_VERSION_MINOR}.${PGURE-SVT_VERSION_PATCH}")

set(OS unix)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_aux/Modules/")

option(BUILD_LIBRARY "Build the shared library" ON)
option(BUILD_PYTHON "Install Python wrapper as a package" ON)
option(BUILD_EXECUTABLE "Build a standalone executable" OFF)

include(CheckIncludeFileCXX)
include(CheckLibraryExists)
enable_language(C)

########################################

# Look for packages
find_package(OpenMP)
include(FindArmadillo)
include(FindTiff)
include(FindOpenBLAS)
include(FindATLAS)
include(FindBLAS)
include(FindLAPACK)
include(FindNLOPT)

if(NLOPT_FOUND)
	set(SVT_LIBS ${SVT_LIBS} ${NLOPT_LIBRARIES})
	set(SVT_INCL ${SVT_INCL} ${NLOPT_INCLUDE_DIR})
endif()

if(!TIFF_FOUND)
    if(BUILD_EXECUTABLE)
        message(SEND_ERROR "*** ERROR: libtiff not found; Standalone program will not be compiled")
    endif()
else()
	set(SVT_LIBS ${SVT_LIBS} ${TIFF_LIBRARIES})
	set(SVT_INCL ${SVT_INCL} ${TIFF_INCLUDE_DIR})
endif()

if(OpenBLAS_FOUND AND ATLAS_FOUND)	
	message(STATUS "*** WARNING: found both OpenBLAS and ATLAS; ATLAS will not be used")
endif()   
 
if(OpenBLAS_FOUND AND BLAS_FOUND)
	message(STATUS "*** WARNING: found both OpenBLAS and BLAS; BLAS will not be used")
endif()

if(OpenBLAS_FOUND)
	set(SVT_LIBS ${SVT_LIBS} ${OpenBLAS_LIBRARIES})      
else()      
    if(ATLAS_FOUND)
    	set(SVT_LIBS ${SVT_LIBS} ${ATLAS_LIBRARIES})        
    endif()      
    if(BLAS_FOUND)
    	set(SVT_LIBS ${SVT_LIBS} ${BLAS_LIBRARIES})
    endif()          
endif()

if(LAPACK_FOUND)
	set(SVT_LIBS ${SVT_LIBS} ${LAPACK_LIBRARIES})
endif()

if(ARMADILLO_FOUND)
	set(SVT_LIBS ${SVT_LIBS} ${ARMADILLO_LIBRARIES})
endif()

########################################

# Enable OpenMP
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fPIC -Wall -march=native -std=c++11")
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

########################################

# Build executable
if(BUILD_EXECUTABLE AND TIFF_FOUND)
    add_executable(PGURE-SVT src/medfilter.c src/exe-PGURE-SVT.cpp)
    target_link_libraries(PGURE-SVT ${SVT_LIBS})
    install(TARGETS PGURE-SVT DESTINATION bin)
endif()

# Build library
if(BUILD_LIBRARY)
    add_library(pguresvt SHARED src/medfilter.c src/lib-PGURE-SVT.cpp)
    target_link_libraries(pguresvt ${SVT_LIBS})
    install(TARGETS pguresvt DESTINATION lib)
endif()

# Install Python package
find_program(PYTHON "python")
if(BUILD_PYTHON AND PYTHON)
  
    set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
    set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
    set(DEPS        "${CMAKE_CURRENT_BINARY_DIR}/pguresvt/*.py")
    set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build/timestamp")
    configure_file(${SETUP_PY_IN} ${SETUP_PY})
    
    set(PYTHONLIBRARYPATH "${CMAKE_INSTALL_PREFIX}/lib")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pguresvt/pguresvt.py.in 
                   ${CMAKE_CURRENT_BINARY_DIR}/pguresvt/pguresvt.py)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pguresvt/hspysvt.py 
                   ${CMAKE_CURRENT_BINARY_DIR}/pguresvt/hspysvt.py)  
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pguresvt/__init__.py 
                   ${CMAKE_CURRENT_BINARY_DIR}/pguresvt/__init__.py)               
    
        
    add_custom_command(OUTPUT ${OUTPUT}
                       COMMAND ${PYTHON} ${SETUP_PY} build
                       COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}
                       DEPENDS ${DEPS})
                       
    add_custom_target(python ALL DEPENDS ${OUTPUT})
        
    install(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install)")
endif()







