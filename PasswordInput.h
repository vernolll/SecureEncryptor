#pragma once
#include <string>
#include <vector>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <fstream>
#include <windows.h>


std::string get_password(const std::string& prompt = "Password: ");

std::vector<unsigned char> encrypt_password(const std::string& password, const unsigned char* key);

void save_encrypted_password(const std::string& password, const unsigned char* key);

std::string decrypt_password(const std::vector<unsigned char>& encrypted_password, const unsigned char* key);

std::string read_encrypted_password(const std::string& filename, const unsigned char* key);

bool load_encrypted_password(const std::string& filename, std::vector<unsigned char>& out);


