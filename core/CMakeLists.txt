find_library(CRYPT_LIB crypt REQUIRED)

add_library(smart_core STATIC
        src/DatabaseManager.cpp
        src/PasswordHasher.cpp
)

target_include_directories(smart_core PUBLIC include)

target_link_libraries(smart_core PUBLIC SQLiteCpp ${CRYPT_LIB})