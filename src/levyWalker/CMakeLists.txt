cmake_minimum_required(VERSION 3.5.2)
project (levyWalker)

find_package(GSL REQUIRED)

set(INCLUDE_DIRS "../base/include" ${GSL_INCLUDE_DIRS})
include_directories(${INCLUDE_DIRS})

set(LIBS ${LIBS} ${GSL_LIBRARIES})

# file(GLOB SOURCES "../base/src/*.cpp")
set(SOURCES "../base/src/Walker.cpp" "../base/src/LevyWalker.cpp" "../base/src/ParameterReader.cpp" levyWalker.cpp)

add_executable(levyWalker ${SOURCES})
target_link_libraries(levyWalker ${LIBS})

install(TARGETS levyWalker DESTINATION $ENV{HOME}/bin)
