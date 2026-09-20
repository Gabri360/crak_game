cmake_minimum_required(VERSION 3.15)
project(crak C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ------------------------------Vendor glfw
include(FetchContent)
FetchContent_declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)


# ------------------------------ Vendor cglm
FetchContent_Declare(
  cglm
  GIT_REPOSITORY https://github.com/recp/cglm.git
  GIT_TAG v0.9.4
)
set(CGLM_STATIC ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cglm)

#------------------------------- Vendor FreeType
FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
  GIT_TAG VER-2-13-3
)

set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(freetype)

#------------------------------ main program

set(GAME_SOURCES
  src/main.c
  src/gamestate.c
  src/state_play.c
  src/state_paused.c
  src/game_fun.c
  src/state_gameover.c
  src/state_statistics.c
)

set(VENDOR_SOURCES
  src/vendor/glad.c
  src/vendor/stb_image_impl.c
)

set(RENDER_SOURCES
  src/render/renderer.c
  src/render/shader.c
  src/render/text_renderer.c
)

set(UTILS_SOURCES
  src/utils/input.c
  src/utils/paths.c
  src/utils/history.c
)

add_executable(${PROJECT_NAME}
  ${GAME_SOURCES}
  ${VENDOR_SOURCES}
  ${RENDER_SOURCES}
  ${UTILS_SOURCES}
)

set_source_files_properties(
  src/main.c
  src/gamestate.c
  src/state_play.c
  src/state_paused.c
  src/state_gameover.c
  src/render/shader.c
  src/render/renderer.c
  src/render/text_renderer.c
  src/utils/input.c
  src/utils/paths.c
  src/utils/history.c
  src/game_fun.c
  PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-Werror;-Wconversion"
)


target_include_directories(${PROJECT_NAME} PRIVATE
  include
  src/headers
)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw cglm freetype)

if(UNIX AND NOT APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE dl)
endif()



#------------------------------- custom command
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
  COMMENT "Copying shaders/ and assets/ in Build"
)

add_custom_target(c
  COMMAND ${CMAKE_COMMAND} -E echo "Clean Build"
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/assets
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/shaders
  COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeCache.txt
  COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/Makefile
  COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/cmake_install.cmake
  COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CPackConfig.cmake
  COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CPackSourceConfig.cmake
  COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
  COMMENT "Reset configurazione CMake (mantiene le dipendenze scaricate in _deps/)"
)

add_custom_target(refresh_share
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/assets
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/shaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets

)

add_custom_target(clean_saves
  COMMAND ${CMAKE_COMMAND} -E remove -f
  "$ENV{HOME}/.local/share/${PROJECT_NAME}/scores.csv"
  COMMAND ${CMAKE_COMMAND} -E touch
  "$ENV{HOME}/.local/share/${PROJECT_NAME}/scores.csv"
  COMMENT "clean saves"
)

#-----------------------------install
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY shaders/ DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/shaders)
install(DIRECTORY assets/  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/assets)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  INSTALL_RESOURCES_DIR="${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}"
)


#-----------------------------uninstall
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)

add_custom_target(uninstall_crak
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}"
  COMMENT "Disinstallazione di ${PROJECT_NAME}"
)
