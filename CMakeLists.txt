#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# This work is free. You can redistribute it and/or modify it under the
# terms of the Do What The Fuck You Want To Public License, Version 2,
# as published by Sam Hocevar. See the COPYING file for more details.
#=============================================================================

cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project(Karrot CXX)

find_package(Boost REQUIRED COMPONENTS filesystem regex system)
include_directories(${Boost_INCLUDE_DIRS})

if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libstdc++")
  set(CMAKE_CXX_LINK_FLAGS "-std=c++11 -stdlib=libstdc++")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-std=c++11 HAVE_STD11)
  if(HAVE_STD11)
    set(CMAKE_CXX_FLAGS -std=c++11)
  else()
    check_cxx_compiler_flag(-std=c++0x HAVE_STD0X)
    if(HAVE_STD0X)
      set(CMAKE_CXX_FLAGS -std=c++0x)
    endif()
  endif()
endif()

add_executable(cmake_cov
  cov.cpp
  )
target_link_libraries(cmake_cov ${Boost_LIBRARIES})

add_executable(cmake_lint
  lint.cpp
  )
target_link_libraries(cmake_lint ${Boost_LIBRARIES})

