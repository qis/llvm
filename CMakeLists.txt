add_subdirectory(llvm)

add_custom_target(unwrap DEPENDS clang clangd)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/unwrap)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/unwrap/libtest.so
  COMMAND $<TARGET_FILE_DIR:clang>/clang++ -std=c++26 -fno-exceptions -fno-rtti -shared -fPIC
    ${CMAKE_SOURCE_DIR}/unwrap/test.cpp -o unwrap/libtest.so
  DEPENDS clang ${CMAKE_SOURCE_DIR}/unwrap/test.cpp
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "[lib] Compiling unwrap library ...")

file(GLOB_RECURSE unwrap_sources CONFIGURE_DEPENDS unwrap/[0-9][0-9][0-9].cpp)
foreach(source IN LISTS unwrap_sources)
  get_filename_component(name "${source}" NAME_WE)

  add_custom_target(unwrap-test-${name}-debug
    COMMAND $<TARGET_FILE_DIR:clang>/clang++ -std=c++26 -fno-exceptions -fno-rtti -O0 -g
      ${CMAKE_SOURCE_DIR}/unwrap/${name}.cpp -o unwrap/${name}-debug -Lunwrap -ltest -lc++ -Wl,-rpath,\\$$ORIGIN
    DEPENDS clang ${CMAKE_BINARY_DIR}/unwrap/libtest.so ${CMAKE_SOURCE_DIR}/unwrap/${name}.cpp
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "[${name}] Compiling unwrap test ...")

  add_custom_target(unwrap-test-${name}-debug-run
    COMMAND unwrap/${name}-debug
    DEPENDS unwrap-test-${name}-debug
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "[${name}] Running unwrap test ...")

  add_custom_target(unwrap-test-${name}-release
    COMMAND $<TARGET_FILE_DIR:clang>/clang++ -std=c++26 -fno-exceptions -fno-rtti -O3 -flto=thin
      ${CMAKE_SOURCE_DIR}/unwrap/${name}.cpp -o unwrap/${name}-release -Lunwrap -ltest -lc++ -Wl,-rpath,\\$$ORIGIN
    DEPENDS clang ${CMAKE_BINARY_DIR}/unwrap/libtest.so ${CMAKE_SOURCE_DIR}/unwrap/${name}.cpp
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "[${name}] Compiling unwrap test ...")

  add_custom_target(unwrap-test-${name}-release-run
    COMMAND unwrap/${name}-release
    DEPENDS unwrap-test-${name}-release
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "[${name}] Running unwrap test ...")
endforeach()

add_custom_target(unwrap-test-run
    COMMAND unwrap/000-debug
    COMMAND unwrap/000-release
    DEPENDS unwrap-test-000-debug
            unwrap-test-000-release
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running unwrap test ...")
