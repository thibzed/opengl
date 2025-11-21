cmake_minimum_required(VERSION 3.10)
project(ProjetPerso)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
link_directories(lib)

set(OPENGL_LIBS
    glfw3
    opengl32
    gdi32
)

# Executable 1 : compute_traj
add_executable(compute_traj
    src/compute_traj.cpp
    src/force.cpp
)

set_target_properties(compute_traj PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Executable 2 : cercle
add_executable(cercle
    src/cercle_EBO.cpp
    src/glad.c
)

target_link_libraries(cercle ${OPENGL_LIBS})

set_target_properties(cercle PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Exec 3 : Shaders
add_executable(shader
    src/shaders.cpp
    src/glad.c
)

target_link_libraries(shader ${OPENGL_LIBS})

set_target_properties(shader PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Exec 4 : Textures
add_executable(textures
    src/texture.cpp
    src/glad.c
)

target_link_libraries(textures ${OPENGL_LIBS})

set_target_properties(textures PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

add_custom_target(run_textures
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/bin/textures.exe
    DEPENDS textures
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Exec 5 : 3D
add_executable(going_3D
    src/glad.c
    src/going_3D.cpp
)

target_link_libraries(going_3D ${OPENGL_LIBS})

set_target_properties(going_3D PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

add_custom_target(run_going_3D
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/bin/going_3D.exe
    DEPENDS textures
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


# Exec 6 : Sphere
add_executable(sphere
    src/glad.c
    src/sphere.cpp
)

target_link_libraries(sphere ${OPENGL_LIBS})

set_target_properties(sphere PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

add_custom_target(run_sphere
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/bin/sphere.exe
    DEPENDS sphere
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)