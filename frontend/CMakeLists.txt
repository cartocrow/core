set(SOURCES
    cartocrow.cpp
)

add_executable(cartocrow ${SOURCES})

find_package(nlohmann_json REQUIRED)

target_link_libraries(
    cartocrow
    PRIVATE
    core
    reader
    necklace_map
    renderer
    CGAL::CGAL
    nlohmann_json::nlohmann_json
)

install(TARGETS cartocrow DESTINATION ${INSTALL_BINARY_DIR})
