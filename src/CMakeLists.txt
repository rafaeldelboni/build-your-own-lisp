add_library(lispy STATIC lispy.c)
target_link_libraries(lispy mpc)

add_executable(main main.c)
target_link_libraries(main editline mpc lispy)

add_executable(doge doge.c)
target_link_libraries(doge mpc)
