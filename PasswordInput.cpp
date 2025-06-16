#include "PasswordInput.h"
#include <iostream>
#include <conio.h>

std::string get_password(const std::string& prompt) {
    std::string password;
    char ch;

    std::cout << prompt;

    while ((ch = _getch()) != '\r') {  // Enter key
        if (ch == '\b') {  // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        }
        else {
            password += ch;
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}

std::vector<unsigned char> encrypt_password(const std::string& password, const unsigned char* key) {
    std::vector<unsigned char> encrypted(password.size() + AES_BLOCK_SIZE); // запас на padding
    int out_len1 = 0, out_len2 = 0;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return {};

    if (!EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, key, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_set_padding(ctx, 1);  // Включаем padding

    if (!EVP_EncryptUpdate(ctx,
        encrypted.data(), &out_len1,
        reinterpret_cast<const unsigned char*>(password.data()), password.size())) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    if (!EVP_EncryptFinal_ex(ctx, encrypted.data() + out_len1, &out_len2)) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    encrypted.resize(out_len1 + out_len2);  // обрезаем до нужного размера
    EVP_CIPHER_CTX_free(ctx);
    return encrypted;
}

void save_encrypted_password(const std::string& password, const unsigned char* key) {
    std::vector<unsigned char> encrypted_password = encrypt_password(password, key);

    std::ofstream file("encrypted_password.bin", std::ios::binary);
    file.write(reinterpret_cast<const char*>(encrypted_password.data()), encrypted_password.size());
    file.close();
}

// Функция для расшифровки пароля
std::string decrypt_password(const std::vector<unsigned char>& encrypted_password, const unsigned char* key) {
    std::vector<unsigned char> decrypted(encrypted_password.size());
    int out_len1 = 0, out_len2 = 0;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return {};

    if (!EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, key, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_set_padding(ctx, 1);  // Включаем padding

    if (!EVP_DecryptUpdate(ctx,
        decrypted.data(), &out_len1,
        encrypted_password.data(), encrypted_password.size())) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    if (!EVP_DecryptFinal_ex(ctx, decrypted.data() + out_len1, &out_len2)) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    decrypted.resize(out_len1 + out_len2);
    EVP_CIPHER_CTX_free(ctx);

    return std::string(decrypted.begin(), decrypted.end());
}


// Функция для чтения зашифрованного пароля из файла
std::string read_encrypted_password(const std::string& filename, const unsigned char* key) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return "";  // Файл не найден, возвращаем пустую строку
    }

    std::vector<unsigned char> encrypted_password((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return decrypt_password(encrypted_password, key);
}

