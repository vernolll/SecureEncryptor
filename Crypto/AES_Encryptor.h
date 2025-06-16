#pragma once
#include <string>

bool encrypt_file_aes(const std::string& input_path, const std::string& output_path, const std::string& password);
bool decrypt_file_aes(const std::string& input_path, const std::string& output_path, const std::string& password);
