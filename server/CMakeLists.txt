add_executable(my_server main.cpp)

target_link_libraries(my_server PRIVATE Crow::Crow smart_core)