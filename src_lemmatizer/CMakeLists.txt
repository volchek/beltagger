cmake_minimum_required(VERSION 2.8)

project(lemmatizer)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB CPPS "*.cpp")

if(UNIX)
  SET(CMAKE_CXX_FLAGS "-std=c++0x -O3")
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

# If necessary, specify your path to Boost here
set(BOOST_ROOT "G:/boost/boost_1_55_0/")
set(BOOST_LIBRARYDIR "$ENV{BOOST_ROOT}/stage_${CMAKE_ADDRESS_MODEL}/lib")

find_package(Boost COMPONENTS program_options REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(${PROJECT_NAME} ${CPPS})
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()
