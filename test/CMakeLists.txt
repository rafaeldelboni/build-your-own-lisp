# Download / Include Unity Test Runner
FetchContent_Declare(
  unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG        3852926c00f210905db5feb7a9fa5d5dbf7963ea)

FetchContent_MakeAvailable(unity)

# 1. Declare the test file
set(Tests
  lispy_test
  lval_test
  builtin_test)

# 2. Declare dependencies of each test file
set(lispy_test_deps lispy)
set(lval_test_deps lval)
set(builtin_test_deps builtin)

# 3. Add dependencies to a list, need to be in the same order as item 1
set(Dependencies
  lispy_test_deps
  lval_test_deps
  builtin_test_deps)

###################################################################

foreach(tst deps IN ZIP_LISTS Tests Dependencies)
  message(STATUS "test=${tst}, dependencies=${deps}")
  add_executable(
    ${tst}
    ${tst}.c)

  target_link_libraries(
    ${tst}
    unity)

  foreach(dep ${${deps}})
    message(STATUS "-> test=${tst}, dependency=${dep}")
    target_link_libraries(
      ${tst}
      ${dep})
  endforeach(dep)

  target_include_directories(
    ${tst} PUBLIC
    ${PROJECT_SOURCE_DIR}/test/unity)

  set_target_properties(
    ${tst}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_PATH}")

  add_test(${tst} "${TEST_OUTPUT_PATH}/${tst}")
endforeach()
