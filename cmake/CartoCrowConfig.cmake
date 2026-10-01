include(CMakeFindDependencyMacro)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

if (EMSCRIPTEN)
    message(STATUS "Using CartoCrow for WebAssembly / Emscripten")
    add_compile_definitions(
            CGAL_ALWAYS_ROUND_TO_NEAREST
            CGAL_DISABLE_GMP=ON
    )
    add_compile_options(-sUSE_BOOST_HEADERS=1)
    add_compile_options(-fwasm-exceptions)
    add_link_options(-g -fwasm-exceptions -lembind)
    if (NOT DEFINED EMSCRIPTEN_INCLUDE_DIR)
        message(FATAL_ERROR "Please set EMSCRIPTEN_INCLUDE_DIR to the folder with all header files needed for Emscripten compilation")
    endif()
    include_directories(${EMSCRIPTEN_INCLUDE_DIR})
else()
    find_dependency(CGAL REQUIRED)
    find_path(CGAL_INCLUDE_DIR CGAL/Exact_predicates_inexact_constructions_kernel.h)
    include_directories(${CGAL_INCLUDE_DIR})
    link_libraries(${CGAL_LIBRARIES})
    find_dependency(GMP REQUIRED)

    find_dependency(Qt5Widgets REQUIRED)
    find_dependency(Ipelib REQUIRED)
    find_dependency(GDAL REQUIRED)

    include_directories(${CavalierContours_INCLUDE_DIR})
endif()

# Add the targets file
include("${CMAKE_CURRENT_LIST_DIR}/CartoCrowTargets.cmake")
