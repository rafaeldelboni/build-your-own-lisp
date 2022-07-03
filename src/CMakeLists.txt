add_library(lval STATIC lval.c)

add_library(lispy STATIC lispy.c)
target_link_libraries(lispy mpc lval)

add_executable(main main.c)
target_link_libraries(main editline mpc lispy lval)

add_executable(doge doge.c)
target_link_libraries(doge mpc)
