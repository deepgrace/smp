#
# Copyright (c) 2011-present DeepGrace (complex dot invoke at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/deepgrace/smp
#

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS "-m64 -std=c++23 -s -Wall -O3")

include(GNUInstallDirs)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(FUPLE fuple)
set(INDEXER indexer)
set(LIST lists)
set(REFLECT reflect)
set(SMP smp)
set(VISITOR visitor)
set(INVOCABLE_NAME invocable_name)

add_executable(${FUPLE} fuple.cpp)
add_executable(${INDEXER} indexer.cpp)
add_executable(${LIST} lists.cpp)
add_executable(${REFLECT} reflect.cpp)
add_executable(${SMP} smp.cpp)
add_executable(${VISITOR} visitor.cpp)
add_executable(${INVOCABLE_NAME} invocable_name.cpp)

install(TARGETS ${FUPLE} ${INDEXER} ${LIST} ${REFLECT} ${SMP} ${VISITOR} ${INVOCABLE_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/bin)
