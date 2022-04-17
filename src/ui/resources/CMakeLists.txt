cmake_minimum_required(VERSION 3.20)

set(RESOURCE_DEF ${CMAKE_CURRENT_SOURCE_DIR}/ui_resources.c.xml PARENT_SCOPE)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(RESOURCE_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/theme.css
                  ${CMAKE_CURRENT_SOURCE_DIR}/tumexam-cli.svg
                  ${CMAKE_CURRENT_SOURCE_DIR}/tumexam-cli.desktop PARENT_SCOPE)

install(FILES tumexam-cli.desktop DESTINATION /usr/share/applications)
install(FILES tumexam-cli.svg DESTINATION /usr/local/share/icons/hicolor/scalable/apps)
