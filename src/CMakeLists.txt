add_executable(splat
    app.cpp
    util.cpp
    camera.cpp
    external/miniply/miniply.cpp
)

target_link_libraries(splat
    ${LIBRARIES}
)