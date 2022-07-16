cmake_minimum_required(VERSION 3.0.0)
project(machina VERSION 0.2.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

option(${CMAKE_PROJECT_NAME}_BUILD_EXAMPLES On "Build Examples")

include(CTest)
enable_testing()

find_package(Threads REQUIRED)

list(APPEND ${CMAKE_PROJECT_NAME}_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include)

file(GLOB_RECURSE ${CMAKE_PROJECT_NAME}_SOURCES src/*.c)
file(GLOB_RECURSE ${CMAKE_PROJECT_NAME}_HEADERS include/*.h)

configure_file(share/${CMAKE_PROJECT_NAME}.pc.in ${CMAKE_PROJECT_NAME}.pc @ONLY)

add_library(${CMAKE_PROJECT_NAME}_object OBJECT ${${CMAKE_PROJECT_NAME}_SOURCES} ${PROJECT_NAME}.pc)

add_library(${CMAKE_PROJECT_NAME}_shared SHARED $<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}_object>)
add_library(${CMAKE_PROJECT_NAME}_static STATIC $<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}_object>)

target_include_directories(${CMAKE_PROJECT_NAME}_object PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:include/${CMAKE_PROJECT_NAME}>
)

target_include_directories(${CMAKE_PROJECT_NAME}_shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:include/${CMAKE_PROJECT_NAME}>
)

target_include_directories(${CMAKE_PROJECT_NAME}_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:include/${CMAKE_PROJECT_NAME}>
)

set_target_properties(${CMAKE_PROJECT_NAME}_shared ${CMAKE_PROJECT_NAME}_static ${CMAKE_PROJECT_NAME}_object PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME}_object Threads::Threads)

install(TARGETS ${CMAKE_PROJECT_NAME}_shared)
install(TARGETS ${CMAKE_PROJECT_NAME}_static)

install(FILES ${${CMAKE_PROJECT_NAME}_HEADERS} DESTINATION include/${CMAKE_PROJECT_NAME})
install(FILES ${${CMAKE_PROJECT_NAME}_HEADERS} DESTINATION include/${CMAKE_PROJECT_NAME})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pc DESTINATION share/pkgconfig COMPONENT config)

include(CPackDebianOptions)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

if (${${CMAKE_PROJECT_NAME}_BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()