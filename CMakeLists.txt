cmake_minimum_required(VERSION 3.28)
project(3DEngine LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.1
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp
        src/events.cpp
        src/events.hpp
        src/configuration.hpp
        src/3DEngine/Engine3D.cpp
        src/3DEngine/Engine3D.hpp
        src/3DEngine/Camera.cpp
        src/3DEngine/Camera.hpp
        src/3DEngine/Object3D.cpp
        src/3DEngine/Object3D.hpp
        src/3DEngine/engineConfiguration.hpp
        src/3DEngine/functions.cpp
        src/3DEngine/functions.hpp
)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics)