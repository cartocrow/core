set(SOURCES
	core.cpp
	ellipse.cpp
	region_arrangement.cpp
	cubic_bezier.cpp
	rectangle_helpers.cpp
	vector_helpers.cpp
	transform_helpers.cpp
	polyline.cpp
	polyline_set.cpp
	segment_delaunay_graph_helpers.cpp
	stopwatch.cpp
)
set(HEADERS
	core.h
	ellipse.h
	boundary_map.h
	arrangement_map.h
	arrangement_map.hpp
	region_arrangement.h
	region_map.h
	cubic_bezier.h
	root_finding_helpers.h
	rectangle_helpers.h
	arrangement_helpers.h
	polyline.h
	general_polyline.h
	vector_helpers.h
	halfplane.h
	transform_helpers.h
	cropped_voronoi.h
	delaunay_voronoi_helpers.h
	segment_delaunay_graph_helpers.h
	segment_delaunay_graph_helpers_cgal_adapted.h
	polyline_set.h
	stopwatch.h
	polygon_helpers.h
)

add_library(core ${SOURCES})

cartocrow_install_module(core)
install(FILES ${HEADERS} DESTINATION ${CARTOCROW_INSTALL_DIR}/core)
