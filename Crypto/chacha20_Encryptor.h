#pragma once

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>

bool encrypt_file_chacha20(const std::string& input_path, const std::string& output_path, const std::string& password);
bool decrypt_file_chacha20(const std::string& input_path, const std::string& output_path, const std::string& password);
