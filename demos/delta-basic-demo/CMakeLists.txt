add_executable(delta-basic-demo WIN32
    include/all_demos.h
    include/delta.h
    include/delta_basic_demo_application.h
    include/demo_application.h
    include/demo_factory.h
    include/demo_list.h
    include/empty_window_demo.h
    include/multiple_window_demo.h

    src/delta_basic_demo_application.cpp
    src/demo_application.cpp
    src/empty_window_demo.cpp
    src/main.cpp
    src/multiple_window_demo.cpp
)

target_link_libraries(delta-basic-demo PUBLIC
    delta-basic
)
