# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.29

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = G:\Jet_brains_IDEs\CLion\bin\cmake\win\x64\bin\cmake.exe

# The command to remove a file.
RM = G:\Jet_brains_IDEs\CLion\bin\cmake\win\x64\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "D:\MyRepo\c++ QT\codWeaponCamoTracker"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "D:\MyRepo\c++ QT\codWeaponCamoTracker\cmake-build-release"

# Utility rule file for codWeaponCamoTracker_autogen.

# Include any custom commands dependencies for this target.
include CMakeFiles/codWeaponCamoTracker_autogen.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/codWeaponCamoTracker_autogen.dir/progress.make

CMakeFiles/codWeaponCamoTracker_autogen: codWeaponCamoTracker_autogen/timestamp

codWeaponCamoTracker_autogen/timestamp: C:/Qt/6.6.1/mingw_64/./bin/moc.exe
codWeaponCamoTracker_autogen/timestamp: C:/Qt/6.6.1/mingw_64/./bin/uic.exe
codWeaponCamoTracker_autogen/timestamp: CMakeFiles/codWeaponCamoTracker_autogen.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir="D:\MyRepo\c++ QT\codWeaponCamoTracker\cmake-build-release\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target codWeaponCamoTracker"
	G:\Jet_brains_IDEs\CLion\bin\cmake\win\x64\bin\cmake.exe -E cmake_autogen "D:/MyRepo/c++ QT/codWeaponCamoTracker/cmake-build-release/CMakeFiles/codWeaponCamoTracker_autogen.dir/AutogenInfo.json" Release
	G:\Jet_brains_IDEs\CLion\bin\cmake\win\x64\bin\cmake.exe -E touch "D:/MyRepo/c++ QT/codWeaponCamoTracker/cmake-build-release/codWeaponCamoTracker_autogen/timestamp"

codWeaponCamoTracker_autogen: CMakeFiles/codWeaponCamoTracker_autogen
codWeaponCamoTracker_autogen: codWeaponCamoTracker_autogen/timestamp
codWeaponCamoTracker_autogen: CMakeFiles/codWeaponCamoTracker_autogen.dir/build.make
.PHONY : codWeaponCamoTracker_autogen

# Rule to build all files generated by this target.
CMakeFiles/codWeaponCamoTracker_autogen.dir/build: codWeaponCamoTracker_autogen
.PHONY : CMakeFiles/codWeaponCamoTracker_autogen.dir/build

CMakeFiles/codWeaponCamoTracker_autogen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\codWeaponCamoTracker_autogen.dir\cmake_clean.cmake
.PHONY : CMakeFiles/codWeaponCamoTracker_autogen.dir/clean

CMakeFiles/codWeaponCamoTracker_autogen.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "D:\MyRepo\c++ QT\codWeaponCamoTracker" "D:\MyRepo\c++ QT\codWeaponCamoTracker" "D:\MyRepo\c++ QT\codWeaponCamoTracker\cmake-build-release" "D:\MyRepo\c++ QT\codWeaponCamoTracker\cmake-build-release" "D:\MyRepo\c++ QT\codWeaponCamoTracker\cmake-build-release\CMakeFiles\codWeaponCamoTracker_autogen.dir\DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/codWeaponCamoTracker_autogen.dir/depend

