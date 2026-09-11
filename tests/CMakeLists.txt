add_executable(test_vector test_vector.c)

target_link_libraries(test_vector PRIVATE cds)

add_test(NAME vector COMMAND test_vector)