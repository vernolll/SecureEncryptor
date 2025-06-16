#pragma once

#include <string>

bool encrypt_file_rsa(const std::string& input_path, const std::string& output_path, const std::string& public_key_path);
bool decrypt_file_rsa(const std::string& input_path, const std::string& output_path, const std::string& private_key_path);
