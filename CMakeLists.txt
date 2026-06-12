cmake_minimum_required(VERSION 4.2.0)
project(dropper)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 23)

find_package(GLEW REQUIRED)
find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(glm REQUIRED)
find_package(slang REQUIRED)

add_executable(dropper)
target_sources(dropper PRIVATE
  src/main.cpp

  src/audio/sound.cpp
  src/controls/controls.cpp
  src/game/game.cpp
  src/game/level.cpp
  src/game/resources.cpp
  src/graphics/graphics.cpp
  src/graphics/primitives.cpp
  src/gui/menu.cpp
  src/gui/text.cpp
  src/levels/file.cpp
  src/levels/slab-event.cpp
  src/physics/collision.cpp
)

target_link_libraries(dropper PRIVATE GLEW::GLEW)
target_link_libraries(dropper PRIVATE SDL3::SDL3)
target_link_libraries(dropper PRIVATE SDL3_ttf::SDL3_ttf)
target_link_libraries(dropper PRIVATE glm::glm)
target_link_libraries(dropper PRIVATE slang::slang)
target_include_directories(dropper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

function(compileShader NAME)
set(FILE_NAME ${NAME}.slang)
set(VERT_NAME ${NAME}.vert.spv)
set(FRAG_NAME ${NAME}.frag.spv)

set(SLANG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${FILE_NAME})
set(VERT_FILE_SPV ${CMAKE_CURRENT_BINARY_DIR}/${VERT_NAME})
set(FRAG_FILE_SPV ${CMAKE_CURRENT_BINARY_DIR}/${FRAG_NAME})
add_custom_command(OUTPUT 
  ${VERT_FILE_SPV}
  ${FRAG_FILE_SPV}
  COMMAND slang::slangc ${SLANG_FILE} -entry vertexMain -target spirv -o ${VERT_FILE_SPV}
  COMMAND slang::slangc ${SLANG_FILE} -entry fragmentMain -target spirv -o ${FRAG_FILE_SPV}
  DEPENDS ${SLANG_FILE}
  COMMENT "Compiling ${SLANG_FILE} shaders"
)

add_custom_target(${NAME}ShaderTarget ALL DEPENDS 
  ${VERT_FILE_SPV}
  ${FRAG_FILE_SPV}
)

add_dependencies(dropper ${NAME}ShaderTarget)
endfunction()

compileShader(fallback)
compileShader(sprite)
compileShader(tube)
compileShader(obstacle)

function(copyFilePath PATH NAME)
add_custom_command(TARGET dropper POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory 
    "${CMAKE_CURRENT_BINARY_DIR}/${PATH}"
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/${PATH}/${NAME}"
    "${CMAKE_CURRENT_BINARY_DIR}/${PATH}/${NAME}"
  COMMENT "Copying files"
)
endfunction()

copyFilePath(assets/font mono-font.ttf)

copyFilePath(assets/audio gasp.wav)
copyFilePath(assets/audio crash.wav)
copyFilePath(assets/audio swoosh.wav)

copyFilePath(assets/levels level1)
copyFilePath(assets/levels level2)
