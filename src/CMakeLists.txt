add_library(lval STATIC lval.c)
target_link_libraries(lval mpc lenv)

add_library(lenv STATIC lenv.c)
target_link_libraries(lenv lval)

add_library(builtin STATIC builtin.c)
target_link_libraries(builtin mpc lval lenv)

add_library(lispy STATIC lispy.c)
target_link_libraries(lispy mpc lval builtin)

add_executable(main main.c)
target_link_libraries(main editline mpc lispy lval lenv builtin)

add_executable(doge doge.c)
target_link_libraries(doge mpc)
