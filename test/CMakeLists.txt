set(TEST_SOURCES "cartocrow_test.cpp"
	"core/cubic_bezier.cpp"
	"core/core.cpp"
	"core/polygon_helpers.cpp"
	"core/region_arrangement.cpp"
	"core/region_map.cpp"
	"renderer/ipe_renderer.cpp"
)


add_executable(cartocrow_test cartocrow_test.cpp ${TEST_SOURCES})
target_link_libraries(cartocrow_test
	PRIVATE
	core
	renderer
	reader
)
