#include "../include/PasswordHasher.h"
#include <crypt.h>
#include <stdexcept>
#include <string_view>

std::string PasswordHasher::hashPassword(const std::string &password) {
    const char* setting = "$2b$12$0000000000000000000000";

    struct crypt_data data{};

    char* hashed = crypt_r(password.c_str(), setting, &data);

    if (!hashed) {
        throw std::runtime_error("Hashing error");
    }

    return std::string(hashed);
}

bool PasswordHasher::verifyPassword(const std::string &password, const std::string &hashedPassword) {
    struct crypt_data data{};

    char* newHash = crypt_r(password.c_str(), hashedPassword.c_str(), &data);

    if (!newHash) {
        return false;
    }

    return std::string_view(newHash) == hashedPassword;
}
