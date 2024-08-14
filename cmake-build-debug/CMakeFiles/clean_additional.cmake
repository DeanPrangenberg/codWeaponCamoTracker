# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\codWeaponCamoTracker_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\codWeaponCamoTracker_autogen.dir\\ParseCache.txt"
  "codWeaponCamoTracker_autogen"
  )
endif()
