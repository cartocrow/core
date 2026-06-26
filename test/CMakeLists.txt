set(TEST_SOURCES "cartocrow_test.cpp"
	"core/cubic_bezier.cpp"
	"core/core.cpp"
	"core/polygon_helpers.cpp"
	"renderer/ipe_renderer.cpp"
	"reader/ipe_reader.cpp"
)


add_executable(cartocrow_test cartocrow_test.cpp ${TEST_SOURCES})
target_link_libraries(cartocrow_test
	PRIVATE
	core
	renderer
	reader
)
