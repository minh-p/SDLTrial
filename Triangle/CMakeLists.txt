cmake_minimum_required(VERSION 3.24)

project(
  Triangle
  VERSION 1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL3 REQUIRED CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

function(add_shaders TARGET_NAME)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
  set(SHADER_SOURCE_FILES ${ARGN})

  list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
  if(FILE_COUNT EQUAL 0)
    message(FATAL_ERROR "Cannot create a shaders target without any source files")
  endif()

  set(SHADER_PRODUCTS)

  foreach(SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
    cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
    cmake_path(GET SHADER_SOURCE FILENAME SHADER_NAME)

    set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.spv")

    add_custom_command(
      OUTPUT ${OUTPUT_FILE}
      COMMAND Vulkan::glslc ${SHADER_SOURCE} -o ${OUTPUT_FILE}
      DEPENDS ${SHADER_SOURCE}
      COMMENT "Compiling shader: ${SHADER_NAME}. ${OUTPUT_FILE}"
      VERBATIM
    )

    list(APPEND SHADER_PRODUCTS ${OUTPUT_FILE})
  endforeach()

  add_custom_target(${TARGET_NAME} ALL
    DEPENDS ${SHADER_PRODUCTS}
    COMMENT "Compiling all shaders for target: ${TARGET_NAME}"
  )
endfunction()

add_shaders(TriangleShaders shaders/vertex.vert shaders/fragment.frag)
add_executable(Triangle src/main.cpp)
add_dependencies(Triangle TriangleShaders)

target_link_libraries(Triangle PRIVATE SDL3)
install(TARGETS Triangle DESTINATION bin)
install(DIRECTORY ${CMAKE_BINARY_DIR}/shaders DESTINATION bin)
