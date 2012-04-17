cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-support")

project(cpfloat)
set(PROJECT_VERSION "0.0.1")
##########################################################################
# Command line options
##########################################################################

##########################################################################
# System information
##########################################################################
message(STATUS "Building for architecture: ${CMAKE_SYSTEM}")
##########################################################################
# Additional compiler flags
##########################################################################
include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-std=c++0x" C0X_SUPPORT)
if(C0X_SUPPORT)
  message(STATUS "CXX has c++0x support")
  set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++0x")
endif()

check_cxx_compiler_flag(-Wall FLAG_WALL)
if(FLAG_WALL)
  add_definitions(-Wall)
endif()

check_cxx_compiler_flag(-Wextra FLAG_WEXTRA)
if(FLAG_WEXTRA)
  add_definitions(-Wextra)
endif()

check_cxx_compiler_flag(-fdiagnostics-show-option FLAG_DIAGNOSTIC)
if(FLAG_DIAGNOSTIC)
  add_definitions(-fdiagnostics-show-option)
endif()

# temporal
add_definitions(-O0 -g)
##########################################################################
# Boost
##########################################################################
find_package( Boost 1.36.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "Boost is needed to build this application")
endif()
##########################################################################
# Gecode
##########################################################################
find_package(Gecode)
if(GECODE_FOUND)
  set(GECODE_CPFLOAT_READY No)
  message(STATUS "Found Gecode version: ${GECODE_VERSION}")
  message(STATUS "Using gecode version: ${GECODE_VERSION}")
  set(MIN_GECODE_REQUIRED 3.6.0)
  set(GECODE_KERNEL_CLASS "CPFloatVarImpBase")
  if ((${GECODE_VERSION} VERSION_EQUAL ${MIN_GECODE_REQUIRED}) OR
      (${GECODE_VERSION} VERSION_GREATER ${MIN_GECODE_REQUIRED}))
    message(STATUS "  Gecode library version is OK")
    # Detect if gecode was build to support the CPFloat variable type
    find_file(GECODE_VIMP gecode/kernel/var-imp.hpp)
    file(STRINGS ${GECODE_VIMP} GECODE_LINE_CPFLOAT_CLASS
      REGEX "^.*class ${GECODE_KERNEL_CLASS} .*")
    message(STATUS "Gecode class: ${GECODE_LINE_CPFLOAT_CLASS}")
    list(LENGTH GECODE_LINE_CPFLOAT_CLASS GECODE_CPFLOAT_SUPPORT)
    if(GECODE_CPFLOAT_SUPPORT)
      message(STATUS "  Gecode was built with support for the CPFLoat type of variable.")
      set(GECODE_CPFLOAT_READY Yes)
    endif()
  endif()
endif()
if(NOT GECODE_FOUND OR NOT GECODE_CPFLOAT_READY)
  message(STATUS "====================================================")
  message(STATUS "No compatible version of gecode was found.")
  message(STATUS "  To build this library you need at least Gecode version ${MIN_GECODE_REQUIRED}")
  message(STATUS "  To build the gecode you need please execute make GECODE_INSTALL.")
  message(STATUS "  and reconfigure with option -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}")
  message(STATUS "====================================================")
  include(ExternalProject)
  set(CONFIGURE_OPTIONS)
  list(APPEND CONFIGURE_OPTIONS "CXXFLAGS=-DGECODE_HAS_VAR_DISPOSE")
  list(APPEND CONFIGURE_OPTIONS "CFLAGS=-DGECODE_HAS_VAR_DISPOSE")

  set(VIS_INPUTS "${CMAKE_SOURCE_DIR}/cpfloat/cpfloat.vis")
  list(APPEND VIS_INPUTS ${EXTRA_VIS})
  message(STATUS "VIS sources: ${VIS_INPUTS}")
  list(APPEND CONFIGURE_OPTIONS  "--with-vis=${VIS_INPUTS}")

  list(APPEND CONFIGURE_OPTIONS "--with-architectures=${CMAKE_OSX_ARCHITECTURES}")
  list(APPEND CONFIGURE_OPTIONS "--disable-examples")
  list(APPEND CONFIGURE_OPTIONS "--prefix=${CMAKE_INSTALL_PREFIX}")
  ExternalProject_Add(GECODE_INSTALL
    SVN_REPOSITORY https://svn.gecode.org/svn/gecode/tags/release-3.7.0
    SVN_USERNAME anonymous
    SVN_PASSWORD user@gmail.com
    CONFIGURE_COMMAND ./configure ${CONFIGURE_OPTIONS}
    BUILD_IN_SOURCE Yes
    BUILD_COMMAND make -j${JOBS}
    INSTALL_COMMAND make install
  )
endif()
##########################################################################
# Gecode includes
##########################################################################
include_directories(${Gecode_INCLUDE_DIRS})
##########################################################################
# Mpfr
##########################################################################
find_path(MPFR_INCLUDE mpfr.h)
if(NOT MPFR_INCLUDE)
  message(FATAL_ERROR "Mpfr is needed to build this library")
else()
  include_directories(${MPFR_INCLUDE})
endif()
find_library(MPFR_LIB mpfr)
if(NOT MPFR_LIB)
  message(FATAL_ERROR "Mpfr is needed to build this library")
else()
  message(STATUS "Using mpfr at: ${MPFR_LIB}")
endif()
##########################################################################
# smath library building
##########################################################################
set(SMATH_SRCS
  IA.c bnr.c narrow_trig.c IA.h exp.c power.c IAasincos.c makefile
  smath.h IAexp.c narrow.c sound_ops.c IAsincos.c narrow_bool.c
  test_precision.c narrow_mult.c trig2pi.c
)
set(SMATH_SRCS_)
foreach(file ${SMATH_SRCS})
  list(APPEND SMATH_SRCS_ smath/${file})
endforeach()
add_library(smath ${SMATH_SRCS_})
##########################################################################
# Relation constraint system
##########################################################################
include_directories(${CMAKE_SOURCE_DIR})
set(CPFLOAT_SRCS
  cpfloat/cpfloat.hh
  cpfloat/main.cpp

#propagators
  cpfloat/prop-base/sin.cpp
  cpfloat/prop-base/sin.hh
  cpfloat/prop-base/cos.cpp
  cpfloat/prop-base/cos.hh
  cpfloat/prop-base/tan.cpp
  cpfloat/prop-base/tan.hh
  cpfloat/prop-base/asin.cpp
  cpfloat/prop-base/asin.hh
  cpfloat/prop-base/acos.cpp
  cpfloat/prop-base/acos.hh
  cpfloat/prop-base/atan.cpp
  cpfloat/prop-base/atan.hh
  cpfloat/prop-base/equality.cpp
  cpfloat/prop-base/equality.hh
  cpfloat/prop-base/addition.cpp
  cpfloat/prop-base/addition.hh
  cpfloat/prop-base/subtraction.cpp
  cpfloat/prop-base/subtraction.hh
  cpfloat/prop-base/times.cpp
  cpfloat/prop-base/times.hh
  cpfloat/prop-base/power.cpp
  cpfloat/prop-base/power.hh
)
add_library(gecodecpfloat ${CPFLOAT_SRCS})
target_link_libraries(gecodecpfloat smath mpfr ${Gecode_LIBRARIES})
##########################################################################
# Installation                                                           #
##########################################################################
#install(FILES ${BINREL_HDRS} DESTINATION include/binrel COMPONENT devel)
install(TARGETS gecodecpfloat
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(DIRECTORY cpfloat/ DESTINATION include/cpfloat
        FILES_MATCHING PATTERN "*.hh" PATTERN "*.hpp")
##########################################################################
# Uninstall
##########################################################################
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
##########################################################################
# Documentation
##########################################################################
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif()
##########################################################################
# Source distribution
##########################################################################
set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
add_custom_target(dist
  COMMAND git archive --format zip --output ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.zip --prefix=${ARCHIVE_NAME}/ master
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
##########################################################################
# Executables
##########################################################################

add_executable(arithmetic tests/arithmetic.cpp)
target_link_libraries(arithmetic gecodecpfloat ${Gecode_LIBRARIES})

add_executable(trigonometric tests/trigonometric.cpp)
target_link_libraries(trigonometric gecodecpfloat ${Gecode_LIBRARIES})
