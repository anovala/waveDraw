# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\spline_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\spline_autogen.dir\\ParseCache.txt"
  "spline_autogen"
  )
endif()
