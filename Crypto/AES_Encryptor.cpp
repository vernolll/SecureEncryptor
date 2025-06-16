#include "AES_Encryptor.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>

constexpr size_t AES_KEY_SIZE = 32; // 256 бит
constexpr size_t AES_IV_SIZE = 16; // AES-CBC использует 16-байтный IV



// Генерация ключа из пароля через SHA-256 (теперь ключ 32 байта)
static void derive_key_from_password(const std::string& password, unsigned char* key_out) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), password.length(), hash);
    memcpy(key_out, hash, AES_KEY_SIZE);
}

bool encrypt_file_aes(const std::string& input_path, const std::string& output_path, const std::string& password) {
    unsigned char key[AES_KEY_SIZE];
    unsigned char iv[AES_IV_SIZE];

    derive_key_from_password(password, key);

    if (!RAND_bytes(iv, AES_IV_SIZE)) {
        std::cerr << "Ошибка генерации IV." << std::endl;
        return false;
    }

    std::ifstream in(input_path, std::ios::binary);
    std::ofstream out(output_path, std::ios::binary);
    if (!in || !out) {
        std::cerr << "Ошибка открытия файлов." << std::endl;
        return false;
    }

    // Пишем IV в начало зашифрованного файла
    out.write(reinterpret_cast<char*>(iv), AES_IV_SIZE);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Ошибка создания контекста EVP_CIPHER_CTX." << std::endl;
        return false;
    }

    const EVP_CIPHER* cipher = EVP_aes_256_cbc();
    if (!cipher) {
        std::cerr << "Ошибка: EVP_aes_256_cbc() вернул nullptr!" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (!EVP_EncryptInit_ex(ctx, cipher, nullptr, key, iv)) {
        unsigned long err = ERR_get_error();
        char err_buf[256];
        ERR_error_string_n(err, err_buf, sizeof(err_buf));
        std::cerr << "Ошибка EVP_EncryptInit_ex: " << err_buf << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    const size_t buffer_size = 4096;
    std::vector<unsigned char> buffer(buffer_size);
    std::vector<unsigned char> encrypted(buffer_size + EVP_MAX_BLOCK_LENGTH);

    int out_len = 0;
    while (in.good()) {
        in.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        std::streamsize bytes_read = in.gcount();
        if (bytes_read > 0) {
            if (!EVP_EncryptUpdate(ctx, encrypted.data(), &out_len, buffer.data(), static_cast<int>(bytes_read))) {
                unsigned long err = ERR_get_error();
                char err_buf[256];
                ERR_error_string_n(err, err_buf, sizeof(err_buf));
                std::cerr << "Ошибка шифрования блока: " << err_buf << std::endl;
                EVP_CIPHER_CTX_free(ctx);
                return false;
            }
            out.write(reinterpret_cast<char*>(encrypted.data()), out_len);
        }
    }

    if (!EVP_EncryptFinal_ex(ctx, encrypted.data(), &out_len)) {
        std::cerr << "Ошибка финального шифрования." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    out.write(reinterpret_cast<char*>(encrypted.data()), out_len);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool decrypt_file_aes(const std::string& input_path, const std::string& output_path, const std::string& password) {
    unsigned char key[AES_KEY_SIZE];
    unsigned char iv[AES_IV_SIZE];

    derive_key_from_password(password, key);

    std::ifstream in(input_path, std::ios::binary);
    std::ofstream out(output_path, std::ios::binary);
    if (!in || !out) {
        std::cerr << "Ошибка открытия файлов." << std::endl;
        return false;
    }

    // Считываем IV из начала зашифрованного файла
    in.read(reinterpret_cast<char*>(iv), AES_IV_SIZE);
    if (in.gcount() != AES_IV_SIZE) {
        std::cerr << "Файл поврежден или не содержит IV." << std::endl;
        return false;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Ошибка создания контекста EVP_CIPHER_CTX." << std::endl;
        return false;
    }

    const EVP_CIPHER* cipher = EVP_aes_256_cbc();
    if (!cipher) {
        std::cerr << "Ошибка: EVP_aes_256_cbc() вернул nullptr!" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (!EVP_DecryptInit_ex(ctx, cipher, nullptr, key, iv)) {
        unsigned long err = ERR_get_error();
        char err_buf[256];
        ERR_error_string_n(err, err_buf, sizeof(err_buf));
        std::cerr << "Ошибка EVP_DecryptInit_ex: " << err_buf << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    const size_t buffer_size = 4096;
    std::vector<unsigned char> buffer(buffer_size);
    std::vector<unsigned char> decrypted(buffer_size + EVP_MAX_BLOCK_LENGTH);

    int out_len = 0;
    while (in.good()) {
        in.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        std::streamsize bytes_read = in.gcount();
        if (bytes_read > 0) {
            if (!EVP_DecryptUpdate(ctx, decrypted.data(), &out_len, buffer.data(), static_cast<int>(bytes_read))) {
                unsigned long err = ERR_get_error();
                char err_buf[256];
                ERR_error_string_n(err, err_buf, sizeof(err_buf));
                std::cerr << "Ошибка расшифровки блока: " << err_buf << std::endl;
                EVP_CIPHER_CTX_free(ctx);
                return false;
            }
            out.write(reinterpret_cast<char*>(decrypted.data()), out_len);
        }
    }

    if (!EVP_DecryptFinal_ex(ctx, decrypted.data(), &out_len)) {
        std::cerr << "Ошибка расшифровки: неверный пароль или поврежден файл." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    out.write(reinterpret_cast<char*>(decrypted.data()), out_len);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool load_encrypted_password(const std::string& filename, std::vector<unsigned char>& out) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    out.resize(size);
    file.read(reinterpret_cast<char*>(out.data()), size);
    return true;
}
