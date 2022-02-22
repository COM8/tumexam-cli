cmake_minimum_required(VERSION 3.20)

set(MAIN_EXECUTABLE ${PROJECT_NAME})

add_subdirectory(logger)
add_subdirectory(ui)

glib_add_resource_file(TARGET ui_resources
                       RESOURCEDEF ${RESOURCE_DEF}
                       WORKING_DIR ${RESOURCE_DIR}
                       OUTFILE ui_resources.c
                       DEPENDS ${RESOURCE_DEPS})

add_executable(${MAIN_EXECUTABLE} main.cpp ${CMAKE_CURRENT_BINARY_DIR}/ui_resources.c)

target_link_libraries(${MAIN_EXECUTABLE} PRIVATE logger ui)
set_property(SOURCE main.cpp PROPERTY COMPILE_DEFINITIONS TUMEXAM_VERSION="${PROJECT_VERSION}" TUMEXAM_VERSION_NAME="${VERSION_NAME}")
install(TARGETS ${MAIN_EXECUTABLE} RUNTIME DESTINATION)
