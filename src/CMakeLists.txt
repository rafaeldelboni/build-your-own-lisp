add_executable(main main.c)
target_link_libraries(main editline mpc)

add_executable(doge_code doge_code.c)
target_link_libraries(doge_code mpc)

add_executable(doge_grammar doge_grammar.c)
target_link_libraries(doge_grammar mpc)
