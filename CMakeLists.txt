cmake_minimum_required(VERSION 3.12)
project(GPU_VRAD)

set(CMAKE_CXX_STANDARD 14)

include_directories(gpua_rad)

add_executable(GPU_VRAD
        gpua_rad/basetypes.h
        gpua_rad/bsp.cpp
        gpua_rad/bsp.h
        gpua_rad/bspfile.h
        gpua_rad/bspflags.h
        gpua_rad/datamap.h
        gpua_rad/main.cpp
        gpua_rad/poly.cpp
        gpua_rad/poly.h
        gpua_rad/prettyprint.cpp
        gpua_rad/prettyprint.h
        gpua_rad/rad.cpp
        gpua_rad/rad.h
        gpua_rad/raytrace.cpp
        gpua_rad/raytrace.h
        gpua_rad/script.cpp
        gpua_rad/script.h
        gpua_rad/timer.cpp
        gpua_rad/timer.h
        gpua_rad/trace.cpp
        gpua_rad/trace.h
        gpua_rad/vector.cpp
        gpua_rad/vector.h
        gpua_rad/vmatrix.h
        gpua_rad/worldsize.h)
