cmake_minimum_required(VERSION 3.5.1)
set (CMAKE_CXX_STANDARD 20)
#set (CMAKE_BUILD_TYPE Release)

set(PNAME enzymeWalker)
project (${PNAME})

find_package(GSL REQUIRED)
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

set(INCLUDE_DIRS "../base/include" ${GSL_INCLUDE_DIRS})
include_directories(${INCLUDE_DIRS})

set(LIBS ${LIBS} ${GSL_LIBRARIES})

#file(GLOB SOURCES "../base/src/*.cpp")
set(SOURCES "../base/src/ParameterReader.cpp" "../base/src/progress_bar.cpp"
    "../base/src/Walker.cpp" "../base/src/WalkerBase.cpp"
    "../base/src/WalkerEnzyme.cpp")
set(SOURCES ${SOURCES} ${PNAME}.cpp)

add_executable(${PNAME} ${SOURCES})
target_link_libraries(${PNAME} ${LIBS})
set_property(TARGET ${PNAME} PROPERTY CXX_STANDARD 14)

install(TARGETS ${PNAME} DESTINATION $ENV{HOME}/bin)
