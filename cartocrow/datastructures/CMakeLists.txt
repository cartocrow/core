set(SOURCES
	
)
set(HEADERS
	indexed_priority_queue.h
	indexed_priority_queue.hpp
	point_quad_tree.h
	point_quad_tree.hpp
	quad_tree.h
	quad_tree.hpp
)

add_library(datastructures ${SOURCES})

cartocrow_install_module(datastructures)
install(FILES ${HEADERS} DESTINATION ${CARTOCROW_INSTALL_DIR}/datastructures)
