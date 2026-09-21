cmake_minimum_required(VERSION 3.20)
project(bo2-elysium CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json 3.11 QUIET)
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
    FetchContent_MakeAvailable(json)
endif()

set(DATA_SOURCES src/data/data_registries.cpp)
set(COMBAT_SOURCES src/combat/combat.cpp)
set(CORE_SOURCES src/core/game.cpp)

add_executable(bo2_elysium ${DATA_SOURCES} ${COMBAT_SOURCES} ${CORE_SOURCES})

target_include_directories(bo2_elysium PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/BO2/Network
)
target_link_libraries(bo2_elysium PRIVATE nlohmann_json::nlohmann_json)

// Keep the P2P test but fix include paths
add_library(bo2_network STATIC Source/BO2/Network/P2PMesh.cpp)
target_include_directories(bo2_network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source/BO2/Network)

enable_testing()
add_executable(test_p2p_mesh tests/test_p2p_mesh.cpp)
target_link_libraries(test_p2p_mesh PRIVATE bo2_network)
add_test(NAME BO2P2PBasics COMMAND test_p2p_mesh)
