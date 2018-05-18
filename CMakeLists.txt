cmake_minimum_required(VERSION 3.9)
project(steam_workaround_fsoverflow VERSION 0.0.1 DESCRIPTION "workaround for a steam bug that prevents installs on too big file systems")

add_library(swfsoverflow32 SHARED main.c)
target_link_libraries(swfsoverflow32 dl)
set_target_properties(swfsoverflow32 PROPERTIES SOVERSION 0)
set_target_properties(swfsoverflow32 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

add_library(swfsoverflow64 SHARED main.c)
target_link_libraries(swfsoverflow64 dl)
set_target_properties(swfsoverflow64 PROPERTIES SOVERSION 0)
set_target_properties(swfsoverflow64 PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
