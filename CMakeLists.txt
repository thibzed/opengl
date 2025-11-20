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
