cmake_minimum_required(VERSION 3.28)
project(codWeaponCamoTracker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Update the CMAKE_PREFIX_PATH to point to the MSVC installation
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.0/msvc2022_64")

find_package(Qt6 COMPONENTS
  Core
  Gui
  Widgets
  REQUIRED)

add_executable(codWeaponCamoTracker WIN32
  main.cpp
  WeaponEditor.cpp
  WeaponEditor.h
  ui.cpp
  logic.cpp
  file.cpp
)

target_link_libraries(codWeaponCamoTracker
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
)

# Prevent console window
if (WIN32)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows -Wl,-e,mainCRTStartup")
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  endif()
endif()

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(DEBUG_SUFFIX)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(DEBUG_SUFFIX "d")
  endif()
  set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
  if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
    set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
      set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
    endif()
  endif()

  # Use windeployqt to deploy Qt dependencies
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${QT_INSTALL_PATH}/bin/windeployqt.exe" --release "$<TARGET_FILE:${PROJECT_NAME}>"
  )
endif()