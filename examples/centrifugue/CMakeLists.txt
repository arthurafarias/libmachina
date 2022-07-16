get_filename_component(CURRENT_DIR_PATH ${CMAKE_CURRENT_LIST_DIR} NAME)
get_filename_component(CURRENT_DIR ${CURRENT_DIR_PATH} NAME)

set(TARGET ${CURRENT_DIR}_example)
list(APPEND ${TARGET}_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include)
message(STATUS "Configuring: ${TARGET}")

file(GLOB_RECURSE ${TARGET}_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.c)

add_executable(${TARGET} ${${TARGET}_SOURCES})

target_include_directories(${TARGET} PUBLIC ${${TARGET}_INCLUDE_DIRECTORIES})
target_link_libraries(${TARGET} PUBLIC ${CMAKE_PROJECT_NAME})

install(TARGETS ${TARGET})