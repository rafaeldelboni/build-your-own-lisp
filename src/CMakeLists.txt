add_executable(main main.c)
target_link_libraries(main editline mpc)

add_executable(doge doge.c)
target_link_libraries(doge mpc)
