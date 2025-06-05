# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appqt6project_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appqt6project_autogen.dir\\ParseCache.txt"
  "appqt6project_autogen"
  )
endif()
