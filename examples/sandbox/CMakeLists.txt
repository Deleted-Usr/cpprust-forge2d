add_executable(
    forge2d-sandbox

    main.cpp
)

target_compile_features(
    forge2d-sandbox
    PRIVATE
        cxx_std_20
)

target_link_libraries(
    forge2d-sandbox

    PRIVATE
        forge2d::forge2d
)

forge2d_copy_shaders(forge2d-sandbox)
