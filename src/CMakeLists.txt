add_library(
  lib STATIC
  lib.c
)

add_executable(
  main
  main.c
)

target_link_libraries(
  main
  lib
)

add_executable(
  prompt
  prompt.c
)

target_link_libraries(
  prompt
  editline
)
