add_executable(test_vector test_vector.c)

target_link_libraries(test_vector PRIVATE cds)

add_test(NAME vector COMMAND test_vector)
 
add_executable(test_list test_list.c)

target_link_libraries(test_list PRIVATE cds)

add_test(NAME list COMMAND test_list)