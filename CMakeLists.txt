cmake_minimum_required(VERSION 3.4)
project(tmap)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
add_executable(tmap
  src/AigNode.cpp
  src/AndInverterGraph.cpp
  src/AndNode.cpp
  src/Cut.cpp
  src/CutEngine.cpp
  src/CutSet.cpp
  src/LatchNode.cpp
  src/main.cpp
  src/TechMapper.cpp
)