#include "chacha20_Encryptor.h"

bool encrypt_file_chacha20(const std::string& input_path, const std::string& output_path, const std::string& password) {
    const size_t key_len = 32; // ChaCha20 key = 256 bits
    const size_t nonce_len = 12;

    // Генерация ключа из пароля (просто хэш или хардкод для примера)
    unsigned char key[key_len] = { 0 };
    for (size_t i = 0; i < password.size() && i < key_len; ++i)
        key[i] = static_cast<unsigned char>(password[i]);

    unsigned char nonce[nonce_len];
    if (!RAND_bytes(nonce, sizeof(nonce))) {
        std::cerr << "Ошибка генерации nonce." << std::endl;
        return false;
    }

    std::ifstream in(input_path, std::ios::binary);
    std::ofstream out(output_path, std::ios::binary);
    if (!in || !out) {
        std::cerr << "Ошибка открытия файлов." << std::endl;
        return false;
    }

    out.write(reinterpret_cast<char*>(nonce), nonce_len); // сохраняем nonce в начало

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_EncryptInit_ex(ctx, EVP_chacha20(), nullptr, key, nonce) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    std::vector<unsigned char> buffer(4096);
    std::vector<unsigned char> ciphertext(4096 + EVP_CIPHER_block_size(EVP_chacha20()));
    int len;

    while (in.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || in.gcount() > 0) {
        int out_len;
        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &out_len, buffer.data(), static_cast<int>(in.gcount())) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        out.write(reinterpret_cast<char*>(ciphertext.data()), out_len);
    }

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data(), &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    out.write(reinterpret_cast<char*>(ciphertext.data()), len);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool decrypt_file_chacha20(const std::string& input_path, const std::string& output_path, const std::string& password) {
    const size_t key_len = 32;
    const size_t nonce_len = 12;

    unsigned char key[key_len] = { 0 };
    for (size_t i = 0; i < password.size() && i < key_len; ++i)
        key[i] = static_cast<unsigned char>(password[i]);

    std::ifstream in(input_path, std::ios::binary);
    std::ofstream out(output_path, std::ios::binary);
    if (!in || !out) {
        std::cerr << "Ошибка открытия файлов." << std::endl;
        return false;
    }

    unsigned char nonce[nonce_len];
    in.read(reinterpret_cast<char*>(nonce), nonce_len);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_DecryptInit_ex(ctx, EVP_chacha20(), nullptr, key, nonce) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    std::vector<unsigned char> buffer(4096);
    std::vector<unsigned char> plaintext(4096 + EVP_CIPHER_block_size(EVP_chacha20()));
    int len;

    while (in.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || in.gcount() > 0) {
        int out_len;
        if (EVP_DecryptUpdate(ctx, plaintext.data(), &out_len, buffer.data(), static_cast<int>(in.gcount())) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        out.write(reinterpret_cast<char*>(plaintext.data()), out_len);
    }

    if (EVP_DecryptFinal_ex(ctx, plaintext.data(), &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    out.write(reinterpret_cast<char*>(plaintext.data()), len);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}
