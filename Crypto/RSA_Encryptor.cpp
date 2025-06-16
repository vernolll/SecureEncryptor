#define _CRT_SECURE_NO_WARNINGS
#define OPENSSL_API_COMPAT 0x10100000L

#include "rsa_encryptor.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <openssl/applink.c>


bool encrypt_file_rsa(const std::string& input_path, const std::string& output_path, const std::string& public_key_path) {
    FILE* pubKeyFile = fopen(public_key_path.c_str(), "rb");
    if (!pubKeyFile) {
        std::cerr << "Ошибка открытия публичного ключа." << std::endl;
        return false;
    }

    RSA* rsa = PEM_read_RSA_PUBKEY(pubKeyFile, nullptr, nullptr, nullptr);
    fclose(pubKeyFile);
    if (!rsa) {
        std::cerr << "Ошибка чтения публичного ключа." << std::endl;
        return false;
    }

    std::ifstream input(input_path, std::ios::binary);
    std::ofstream output(output_path, std::ios::binary);
    if (!input || !output) {
        std::cerr << "Ошибка открытия входного или выходного файла." << std::endl;
        RSA_free(rsa);
        return false;
    }

    int rsa_len = RSA_size(rsa);
    std::vector<unsigned char> inbuf(rsa_len - 42); // для RSA_PKCS1_OAEP_PADDING
    std::vector<unsigned char> outbuf(rsa_len);

    while (!input.eof()) {
        input.read(reinterpret_cast<char*>(inbuf.data()), inbuf.size());
        std::streamsize read_bytes = input.gcount();

        int encrypted_len = RSA_public_encrypt(read_bytes, inbuf.data(), outbuf.data(), rsa, RSA_PKCS1_OAEP_PADDING);
        if (encrypted_len == -1) {
            std::cerr << "Ошибка RSA_public_encrypt: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
            RSA_free(rsa);
            return false;
        }

        output.write(reinterpret_cast<char*>(outbuf.data()), encrypted_len);
    }

    RSA_free(rsa);
    return true;
}

bool decrypt_file_rsa(const std::string& input_path, const std::string& output_path, const std::string& private_key_path) {
    FILE* privKeyFile = fopen(private_key_path.c_str(), "rb");
    if (!privKeyFile) {
        std::cerr << "Ошибка открытия приватного ключа." << std::endl;
        return false;
    }

    RSA* rsa = PEM_read_RSAPrivateKey(privKeyFile, nullptr, nullptr, nullptr);
    fclose(privKeyFile);
    if (!rsa) {
        std::cerr << "Ошибка чтения приватного ключа." << std::endl;
        return false;
    }

    std::ifstream input(input_path, std::ios::binary);
    std::ofstream output(output_path, std::ios::binary);
    if (!input || !output) {
        std::cerr << "Ошибка открытия входного или выходного файла." << std::endl;
        RSA_free(rsa);
        return false;
    }

    int rsa_len = RSA_size(rsa);
    std::vector<unsigned char> inbuf(rsa_len);
    std::vector<unsigned char> outbuf(rsa_len); // Достаточно большого размера

    while (input.read(reinterpret_cast<char*>(inbuf.data()), rsa_len)) {
        int decrypted_len = RSA_private_decrypt(rsa_len, inbuf.data(), outbuf.data(), rsa, RSA_PKCS1_OAEP_PADDING);
        if (decrypted_len == -1) {
            std::cerr << "Ошибка RSA_private_decrypt: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
            RSA_free(rsa);
            return false;
        }

        output.write(reinterpret_cast<char*>(outbuf.data()), decrypted_len);
    }

    RSA_free(rsa);
    return true;
}
