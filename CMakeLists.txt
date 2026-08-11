cmake_minimum_required(VERSION 3.16)
project(BO2Elysium LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(bo2_network STATIC
    Source/BO2/Network/P2PMesh.cpp
)
target_include_directories(bo2_network PUBLIC Source/BO2/Network)

add_executable(test_p2p_mesh tests/test_p2p_mesh.cpp)
target_link_libraries(test_p2p_mesh PRIVATE bo2_network)

enable_testing()
add_test(NAME BO2P2PBasics COMMAND test_p2p_mesh)
