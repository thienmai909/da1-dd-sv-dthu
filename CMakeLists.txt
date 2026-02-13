cmake_minimum_required(VERSION 3.15)
project(da1-dd-sv-dthu VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===================================
# MODELS LIBRARY
# ===================================
add_library(models
    src/models.cpp
)

target_include_directories(models PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

if(WIN32)
    target_link_libraries(models PRIVATE
        ${PROJECT_SOURCE_DIR}/libs/libsodium.lib
    )

    add_custom_command(TARGET models POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libs/libsodium.dll"
        $<TARGET_FILE_DIR:models>
    )
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SODIUM REQUIRED libsodium)

    target_include_directories(models PRIVATE ${SODIUM_INCLUDE_DIRS})
    target_link_libraries(models PRIVATE ${SODIUM_LIBRARIES})
endif()



# ===================================
# MAIN EXECUTABLE
# ===================================
add_executable(diemdanh
    src/main.cpp
)

target_link_libraries(diemdanh PRIVATE models)

# ===================================
# GOOGLE TEST
# ===================================
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# ===================================
# AUTO TEST DISCOVERY (1 model = 1 test file)
# ===================================
file(GLOB TEST_SOURCES tests/*.cpp)

foreach(test_file ${TEST_SOURCES})

    get_filename_component(test_name ${test_file} NAME_WE)

    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name} PRIVATE
        models
        GTest::gtest_main
    )

    if(WIN32)
        add_custom_command(TARGET ${test_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/libs/libsodium.dll"
            $<TARGET_FILE_DIR:${test_name}>
        )
    endif()

    include(GoogleTest)
    gtest_discover_tests(${test_name})

endforeach()
