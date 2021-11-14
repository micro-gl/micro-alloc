#========================================================================================
# Copyright (2021), Tomer Shalev (tomer.shalev@gmail.com, https://github.com/HendrixString).
# All Rights Reserved.
#
# this should help find the micro-alloc headers-only package and define the micro-alloc target that was
# installed on your system and does not include CMakeLists.txt file, so you can easily link to it.
# If successful, the following will happen:
# 1. micro-alloc_INCLUDE_DIR will be defined
# 2. micro-alloc::micro-alloc target will be defined so you can link to it using target_link_libraries(..)
#========================================================================================
include(GNUInstallDirs)
include(FindPackageHandleStandardArgs)

find_path(micro-alloc_INCLUDE_DIR
        NAMES micro-alloc
        HINTS ${CMAKE_INSTALL_INCLUDEDIR}
        PATH_SUFFIXES clippers bitmaps samplers)
set(micro-alloc_LIBRARY "/dont/use")
find_package_handle_standard_args(micro-alloc DEFAULT_MSG
        micro-alloc_LIBRARY micro-alloc_INCLUDE_DIR)

if(micro-alloc_FOUND)
    message("micro-alloc was found !!!")
else(micro-alloc_FOUND)
    message("micro-alloc was NOT found !!!")
endif(micro-alloc_FOUND)

if(micro-alloc_FOUND AND NOT TARGET micro-alloc::micro-alloc)
    # build the target
    add_library(micro-alloc::micro-alloc INTERFACE IMPORTED)
    set_target_properties(
            micro-alloc::micro-alloc
            PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${micro-alloc_INCLUDE_DIR}")
endif()