set(SHARED_SOURCES
	geojson_reader.cpp
)
set(NATIVE_SOURCES
	boundary_map_reader.cpp
	gdal_conversion.cpp
	region_map_reader.cpp
)
set(SHARED_HEADERS
	geojson_reader.h
	geometry_reader.h
	linear_object_reader.h
	multi_reader.h
)
set(NATIVE_HEADERS
	boundary_map_reader.h
	gdal_conversion.h
	gdal_reader.h
	ipe_reader.h
	region_map_reader.h
)

if (EMSCRIPTEN)
set(SOURCES
    ${SHARED_SOURCES}
)
set(HEADERS
    ${SHARED_HEADERS}
)
else()
set(SOURCES
    ${SHARED_SOURCES}
    ${NATIVE_SOURCES}
)
set(HEADERS
    ${SHARED_HEADERS}
    ${NATIVE_HEADERS}
)
endif()

add_library(reader ${SOURCES})
if(EMSCRIPTEN)
target_link_libraries(reader
	INTERFACE core
)
else()
target_link_libraries(reader
		PUBLIC core
		Ipe::ipelib
		GDAL::GDAL
)
endif()
cartocrow_install_module(reader)
install(FILES ${HEADERS} DESTINATION ${CARTOCROW_INSTALL_DIR}/reader)
