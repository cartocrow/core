set(SOURCES
	boundary_map_reader.cpp
	gdal_conversion.cpp
	region_map_reader.cpp
)
set(HEADERS
	boundary_map_reader.h	
	gdal_conversion.h
	gdal_reader.h
	geometry_reader.h
	ipe_reader.h
	linear_object_reader.h
	multi_reader.h
	region_map_reader.h
)

add_library(reader ${SOURCES})
target_link_libraries(reader
	PUBLIC core
		   renderer
		   Ipe::ipelib
	       GDAL::GDAL
)

cartocrow_install_module(reader)
install(FILES ${HEADERS} DESTINATION ${CARTOCROW_INSTALL_DIR}/reader)
