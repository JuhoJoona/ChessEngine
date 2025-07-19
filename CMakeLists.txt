cmake_minimum_required(VERSION 3.20)      # modern CMake, pre-C++20 features
project(chess_engine VERSION 0.1.0 LANGUAGES CXX)

# ──────────────────────────────  Global settings  ──────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS  OFF)


# Apply flags early so all targets inherit them
if (ENABLE_LTO AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    include(CheckIPOSupported)
    check_ipo_supported(RESULT has_ipo OUTPUT ipo_err)
    if (has_ipo) 
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endif()

if (ENABLE_SANITIZERS AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options   (-fsanitize=address,undefined)
endif()

# ───────────────────────────────  Helpers  ─────────────────────────────────────
function(add_layer_library layer_dir target_name)
    file(GLOB_RECURSE SRC ${layer_dir}/*.cpp)
    if(NOT SRC)
        # For header-only libraries, we don't need to create an empty source file
        add_library(${target_name} INTERFACE)
        target_include_directories(
            ${target_name} INTERFACE
            ${PROJECT_SOURCE_DIR}/src          # for sibling headers
        )
    else()
        add_library(${target_name} STATIC ${SRC})
        target_include_directories(
            ${target_name} PUBLIC
            ${PROJECT_SOURCE_DIR}/src          # for sibling headers
        )
    endif()
    set_target_properties(${target_name} PROPERTIES FOLDER "lib/${target_name}")
endfunction()

# Helper function to handle linking based on library type
function(target_link_libraries_smart target)
    get_target_property(target_type ${target} TYPE)
    if(target_type STREQUAL "INTERFACE_LIBRARY")
        target_link_libraries(${target} INTERFACE ${ARGN})
    else()
        target_link_libraries(${target} PUBLIC ${ARGN})
    endif()
endfunction()

# ───────────────────────────────  Layers  ──────────────────────────────────────
# util   
add_layer_library(src/util     util)

# game     
add_layer_library(src/core/game/board board)
target_link_libraries_smart(board util)

add_layer_library(src/core/game/move move)
target_link_libraries_smart(move board util)

add_layer_library(src/core/game/movegen movegen)
target_link_libraries_smart(movegen move board util)

add_layer_library(src/core/game game)
target_link_libraries_smart(game movegen move board util)

# eval   
add_layer_library(src/core/eval     eval)
target_link_libraries_smart(eval game)

# search  
add_layer_library(src/core/search   search)
target_link_libraries_smart(search game eval util)

# knowledge
add_layer_library(src/core/knowledge knowledge)
target_link_libraries_smart(knowledge game eval util)

# uci     
add_layer_library(src/uci      uci)
target_link_libraries_smart(uci search knowledge game eval util)

# engine 
add_layer_library(src/engine           engine)         # protocol-free façade
target_link_libraries_smart(engine search knowledge game eval util)

# ───────────────────────────────  Executable  ──────────────────────────────────
add_executable(chess-engine src/main.cpp)
target_link_libraries(chess-engine PRIVATE uci engine)

set_target_properties(chess-engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
    FOLDER "app"
)

# ───────────────────────────────  Tests  ───────────────────────────────────────

# ───────────────────────────────  Install  ─────────────────────────────────────
# Comment out if you truly never want 'make install'
install(TARGETS chess-engine DESTINATION bin)
