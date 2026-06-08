add_library(smart_core STATIC
        src/DatabaseManager.cpp
)

target_include_directories(smart_core PUBLIC include)

target_link_libraries(smart_core PUBLIC SQLiteCpp)