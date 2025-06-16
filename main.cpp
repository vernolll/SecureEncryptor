#include <iostream>
#include <windows.h>
#include <string>
#include <filesystem>
#include <openssl/evp.h>
#include <openssl/err.h>
#include "PasswordInput.h"
#include "Crypto/AES_Encryptor.h"
#include "Crypto/RSA_Encryptor.h"
#include "Crypto/chacha20_Encryptor.h"


std::string load_default_algorithm(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return "AES";
    std::string alg;
    std::getline(file, alg);
    return alg;
}

void save_default_algorithm(const std::string& filename, const std::string& algorithm) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << algorithm;
    }
}

void encrypt_console_mode() {
    bool running = true;
    int choice = 0, ch = 0, chs = 0;

    std::string password;
    const std::string passwordFile = "encrypted_password.bin";
    const unsigned char key[16] = {
        0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21
    };

    if (std::filesystem::exists(passwordFile)) {
        password = read_encrypted_password(passwordFile, key);
        if (password.empty()) {
            std::cerr << "Ошибка при расшифровке сохранённого пароля!" << std::endl;
            return;
        }
    }
    else {
        password = get_password("ENTER DEFAULT PASSWORD: ");
        save_encrypted_password(password, key);
        std::cout << "Пароль сохранён." << std::endl;
    }


    while (running) {
        std::cout << "\n-ENCRYPTOR-" << std::endl;
        std::cout << "1. DEFAULT SETTINGS" << std::endl;
        std::cout << "2. WORK WITH FILES" << std::endl;
        std::cout << "3. WORK WITH FOLDERS" << std::endl;
        std::cout << "4. EXIT" << std::endl;
        std::cout << "Enter a number: ";
        std::cin >> choice;

        std::cout << "<---------------------------------------->\n" << std::endl;
        switch (choice) {
        case 1:
        {
            std::cout << "\n-DEFAULT SETTINGS- " << std::endl;
            std::cout << "1. CHOOSE DEFAULT PASSWORD" << std::endl;
            std::cout << "2. CHOOSE DEFAULT ALGORITHM" << std::endl;
            std::cout << "3. RETURN" << std::endl;
            std::cout << "Enter a number: ";
            std::cin >> ch;

            std::cout << "<---------------------------------------->\n" << std::endl;

            switch (ch) {
            case 1:
            {
                int ch;
                const std::string filename = "encrypted_password.bin";
                const unsigned char key[16] = {
                    0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef,
                    0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21
                };


                std::cout << "\n-DEFAULT PASSWORD-" << std::endl;
                if (std::filesystem::exists("encrypted_password.bin")) {
                    std::string decrypted_password = read_encrypted_password("encrypted_password.bin", key);

                    if (!decrypted_password.empty()) {
                        std::cout << "YOUR PREVIOUS PASSWORD IS: " << decrypted_password << std::endl;
                    }
                    else {
                        std::cout << "Error decrypting password." << std::endl;
                    }
                }

                std::string password = get_password("ENTER YOUR NEW PASSWORD FOR QUICK ENCRYPTION: ");
                save_encrypted_password(password, key);
                std::cout << "New password saved securely." << std::endl;

                std::cout << "ENTER \"0\" TO RETURN\n" << std::endl;
                std::cin >> ch;

                break;
            }
            case 2:
            {
                std::string algorithmFile = "default_algorithm.txt";
                std::string currentAlg = load_default_algorithm(algorithmFile);

                std::cout << "\n-DEFAULT ALGORITHM-" << std::endl;
                std::cout << "PREVIOUS DEFAULT ALGORITHM: " << currentAlg << std::endl;
                std::cout << "CHOOSE NEW ALGORITHM FOR QUICK ENCRYPTION:" << std::endl;
                std::cout << "1. CHACHA20" << std::endl;
                std::cout << "2. AES" << std::endl;
                std::cout << "3. RSA " << std::endl;
                std::cout << "4. RETURN" << std::endl;
                std::cout << "Enter a number: ";
                std::cin >> chs;

                std::cout << "<---------------------------------------->\n" << std::endl;

                switch (chs) {
                case 1:
                    save_default_algorithm(algorithmFile, "CHACHA20");
                    break;
                case 2:
                    save_default_algorithm(algorithmFile, "AES");
                    break;
                case 3:
                    save_default_algorithm(algorithmFile, "RSA");
                    break;
                case 4:
                    break;
                }
                break;
            }
            case 3:
            {
                break;
            }
            }
            break;

        }
        case 2:
        {
            std::cout << "\n-WORK WITH FILES-" << std::endl;
            std::cout << "1. ENCRYPT" << std::endl;
            std::cout << "2. DECRYPT" << std::endl;
            std::cout << "Enter a number: ";
            std::cin >> ch;

            int alg = 0;

            switch (ch) {
            case 1:
            {
                std::string inputPath;
                std::cout << "\n-WORK WITH FILES-" << std::endl;
                std::cout << "ENTER PATH TO THE FILE YOU WANT TO WORK WITH:" << std::endl;
                std::cout << "Enter path: ";
                std::cin.ignore();
                std::getline(std::cin, inputPath);

                std::filesystem::path inputFile(inputPath);
                if (!std::filesystem::exists(inputFile)) {
                    std::cerr << "File not found: " << inputPath << std::endl;
                    break;
                }

                std::cout << "\nYOU CHOSE FILE \"" << inputFile.filename() << "\"" << std::endl;
                std::cout << "YOU CHOSE TO ENCRYPT IT" << std::endl;
                std::cout << "CHOOSE ALGORITHM:" << std::endl;
                std::cout << "1. AES" << std::endl;
                std::cout << "2. RSA" << std::endl;
                std::cout << "3. CHACHA20" << std::endl;
                std::cout << "Enter a number: ";
                int alg;
                std::cin >> alg;

                std::string algorithm;
                switch (alg) {
                case 1: algorithm = "AES"; break;
                case 2: algorithm = "RSA"; break;
                case 3: algorithm = "CHACHA20"; break;
                default:
                    std::cerr << "Incorrect choice of algorithm." << std::endl;
                    break;
                }

                char del_f;
                std::cout << "\n-WORK WITH FILES-" << std::endl;
                std::cout << "YOU CHOSE FILE \"" << inputFile.filename() << "\"" << std::endl;
                std::cout << "YOU CHOSE TO ENCRYPT IT USING " << algorithm << std::endl;
                std::cout << "DO YOU WANT TO DELETE FILE AFTER ENCRYPTION (ONLY FOR REGULAR ONES)? (Y/N): ";
                std::cin >> del_f;

                std::string pass;
                std::cout << "ENTER PASSWORD FOR ENCRYPTION: ";
                std::cin.ignore();
                std::getline(std::cin, pass);

                const unsigned char key[16] = {
                                   0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef,
                                   0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21
                };

                std::vector<unsigned char> encrypted_password;
                if (!load_encrypted_password("encrypted_password.bin", encrypted_password)) {
                    std::cerr << "The saved password could not be uploaded." << std::endl;
                    break;
                }

                std::string saved_password = decrypt_password(encrypted_password, key);
                if (pass != saved_password) {
                    std::cerr << "WRONG PASSWORD" << std::endl;
                    break;
                }

                std::filesystem::path outputFile = inputFile;
                if (algorithm == "AES") outputFile += ".aes";
                else if (algorithm == "RSA") outputFile += ".rsa";
                else if (algorithm == "CHACHA20") outputFile += ".cha";

                bool success = false;
                if (algorithm == "AES") {
                    success = encrypt_file_aes(inputPath, outputFile.string(), pass);
                }
                else if (algorithm == "RSA") {
                    success = encrypt_file_rsa(inputPath, outputFile.string(), "public.pem");
                }
                else if (algorithm == "CHACHA20") {
                    success = encrypt_file_chacha20(inputPath, outputFile.string(), pass);
                }

                if (success) {
                    std::cout << "The file has been successfully encrypted: " << outputFile << std::endl;
                    if (del_f == 'Y' || del_f == 'y') {
                        std::filesystem::remove(inputFile);
                        std::cout << "The original file has been deleted." << std::endl;
                    }
                }
                else {
                    std::cerr << "Encryption error." << std::endl;
                }

                break;
            }
            case 2:
            {
                    std::string inputPath;
                    std::cout << "-WORK WITH FILES-" << std::endl;
                    std::cout << "ENTER PATH TO THE FILE YOU WANT TO DECRYPT: ";
                    std::cin.ignore();
                    std::getline(std::cin, inputPath);

                    std::filesystem::path inputFile(inputPath);
                    if (!std::filesystem::exists(inputFile)) {
                        std::cerr << "File not found: " << inputPath << std::endl;
                        break;
                    }

                    std::cout << "\nYOU CHOSE FILE \"" << inputFile.filename() << "\"" << std::endl;
                    std::cout << "YOU CHOSE TO DECRYPT IT" << std::endl;
                    std::cout << "CHOOSE ALGORITHM:" << std::endl;
                    std::cout << "1. AES" << std::endl;
                    std::cout << "2. RSA" << std::endl;
                    std::cout << "3. CHACHA20" << std::endl;
                    std::cout << "Enter a number: ";
                    int alg;
                    std::cin >> alg;

                    std::string algorithm;
                    switch (alg) {
                    case 1: algorithm = "AES"; break;
                    case 2: algorithm = "RSA"; break;
                    case 3: algorithm = "CHACHA20"; break;
                    default:
                        std::cerr << "Incorrect choice of algorithm." << std::endl;
                        break;
                    }

                    char del_f;
                    std::cout << "\n-WORK WITH FILES-" << std::endl;
                    std::cout << "YOU CHOSE FILE \"" << inputFile.filename() << "\"" << std::endl;
                    std::cout << "YOU CHOSE TO DECRYPT IT USING " << algorithm << std::endl;
                    std::cout << "DO YOU WANT TO DELETE FILE AFTER DECRYPTION? (Y/N): ";
                    std::cin >> del_f;

                    std::string pass;
                    std::cout << "ENTER PASSWORD TO DECRYPT" << std::endl;
                    std::cout << "Enter password: ";
                    std::cin.ignore();
                    std::getline(std::cin, pass);

                    const unsigned char key[16] = {
                                       0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef,
                                       0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21
                    };

                    std::vector<unsigned char> encrypted_password;
                    if (!load_encrypted_password("encrypted_password.bin", encrypted_password)) {
                        std::cerr << "The saved password could not be uploaded." << std::endl;
                        break;
                    }

                    std::string saved_password = decrypt_password(encrypted_password, key);
                    if (pass != saved_password) {
                        std::cerr << "WRONG PASSWORD" << std::endl;
                        break;
                    }

                    std::filesystem::path outputFile = inputFile;
                    outputFile.replace_extension(); // удаляет текущее расширение
                    //outputFile += "_decrypted.txt";

                    bool success = false;
                    if (algorithm == "AES") {
                        success = decrypt_file_aes(inputPath, outputFile.string(), pass);
                    }
                    else if (algorithm == "RSA") {
                        success = decrypt_file_rsa(inputPath, outputFile.string(), "private.pem"); // путь к приватному ключу
                    }
                    else if (algorithm == "CHACHA20") {
                        success = decrypt_file_chacha20(inputPath, outputFile.string(), pass);
                    }

                    if (success) {
                        std::cout << "The file has been successfully decrypted: " << outputFile << std::endl;
                        if (del_f == 'Y' || del_f == 'y') {
                            std::filesystem::remove(inputFile);
                            std::cout << "The encrypted file has been deleted." << std::endl;
                        }
                    }
                    else {
                        std::cerr << "Decryption error." << std::endl;
                    }

                    break;
                
                }
            }
            break;
        }
        case 3:
        {
            std::string inputPath;
            std::cout << "\n-WORK WITH FOLDERS-" << std::endl;
            std::cout << "ENTER WAY TO THE FOLDER YOU WANT TO WORK WITH: ";
            std::cin.ignore();
            std::getline(std::cin, inputPath);

            std::filesystem::path inputFile(inputPath);
            if (!std::filesystem::exists(inputFile)) {
                std::cerr << "File not found: " << inputPath << std::endl;
                break;
            }

            int num = 0;
            std::cout << "\nYOU CHOSE FOLDER \"" << inputFile.filename() << "\"" << std::endl;
            std::cout << "1. ENCRYPT" << std::endl;
            std::cout << "2. DECRYPT" << std::endl;
            std::cout << "Enter a number: "; // только если зашифрована
            std::cin >> num;

            switch (num) {
            case 1:
            {
                std::cout << "\n-WORK WITH FOLDERS-" << std::endl;
                std::cout << "YOU CHOSE TO ENCRYPT IT" << std::endl;
                std::cout << "CHOOSE ALGORITHM:" << std::endl;
                std::cout << "1. AES" << std::endl;
                std::cout << "2. RSA" << std::endl;
                std::cout << "3. CHACHA20" << std::endl;
                std::cout << "Enter a number: ";
                int alg;
                std::cin >> alg;
                std::cin.ignore(1000, '\n');

                std::string algorithm;
                switch (alg) {
                case 1: algorithm = "AES"; break;
                case 2: algorithm = "RSA"; break;
                case 3: algorithm = "CHACHA20"; break;
                default:
                    std::cerr << "Incorrect choice of algorithm." << std::endl;
                    break;
                }

                char del_f;
                std::cout << "\n-WORK WITH FOLDERS-" << std::endl;
                std::cout << "YOU CHOSE FOLDER \"" << inputFile.filename() << "\"" << std::endl;
                std::cout << "YOU CHOSE TO ENCRYPT IT USING " << algorithm << std::endl;
                std::cout << "DO YOU WANT TO DELETE ORIGINAL FOLDER AND ARCHIVE AFTER ENCRYPTION? (Y/N): ";
                std::cin >> del_f;
                std::cin.ignore(1000, '\n');

                std::string pass;
                std::cout << "ENTER PASSWORD TO ENCRYPT" << std::endl;
                std::cout << "Enter password: ";
                std::getline(std::cin, pass);

                const unsigned char key[16] = {
                    0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef,
                    0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21
                };

                std::vector<unsigned char> encrypted_password;
                if (!load_encrypted_password("encrypted_password.bin", encrypted_password)) {
                    std::cerr << "The saved password could not be uploaded." << std::endl;
                    break;
                }

                std::string saved_password = decrypt_password(encrypted_password, key);
                if (pass != saved_password) {
                    std::cerr << "WRONG PASSWORD" << std::endl;
                    break;
                }

                // Step 1: Archive folder into .zip
                std::string archivePath = inputPath + ".zip";
                std::string sourceFolder = inputFile.string();

                // PowerShell command with correct escaping
                std::string powershellCommand =
                    "powershell -Command \"Compress-Archive -Path '" + inputFile.string() +
                    "\\*' -DestinationPath '" + archivePath + "' -Force\"";

                std::cout << "Creating archive..." << std::endl;
                if (std::system(powershellCommand.c_str()) != 0) {
                    std::cerr << "Failed to create archive." << std::endl;
                    break;
                }

                // Step 2: Encrypt the .zip archive
                std::filesystem::path encryptedArchive = inputFile;
                if (algorithm == "AES") encryptedArchive += ".aes";
                else if (algorithm == "RSA") encryptedArchive += ".rsa";
                else if (algorithm == "CHACHA20") encryptedArchive += ".cha";

                bool success = false;
                if (algorithm == "AES") {
                    success = encrypt_file_aes(archivePath, encryptedArchive.string(), pass);
                }
                else if (algorithm == "RSA") {
                    success = encrypt_file_rsa(archivePath, encryptedArchive.string(), "public.pem");
                }
                else if (algorithm == "CHACHA20") {
                    success = encrypt_file_chacha20(archivePath, encryptedArchive.string(), pass);
                }

                if (success) {
                    if (std::filesystem::exists(archivePath)) {
                        std::filesystem::remove(archivePath);
                    }
                    std::cout << "[+] Folder was successfully encrypted as: " << encryptedArchive << std::endl;
                    if (del_f == 'Y' || del_f == 'y') {
                        std::filesystem::remove_all(inputFile);        // Delete original folder
                        std::filesystem::remove(archivePath);          // Delete archive
                        std::cout << "Original folder and archive deleted." << std::endl;
                    }
                }
                else {
                    std::cerr << "[-] Encryption failed." << std::endl;
                }

                break;
            }
            case 2:
            {
                std::cout << "\nYOU CHOSE FILE \"" << inputFile.filename() << "\"" << std::endl;
                std::cout << "YOU CHOSE TO DECRYPT IT" << std::endl;
                std::cout << "CHOOSE ALGORITHM:" << std::endl;
                std::cout << "1. AES" << std::endl;
                std::cout << "2. RSA" << std::endl;
                std::cout << "3. CHACHA20" << std::endl;
                std::cout << "Enter a number: ";
                int alg;
                std::cin >> alg;
                std::cin.ignore(1000, '\n');

                std::string algorithm;
                switch (alg) {
                case 1: algorithm = "AES"; break;
                case 2: algorithm = "RSA"; break;
                case 3: algorithm = "CHACHA20"; break;
                default:
                    std::cerr << "Incorrect choice of algorithm." << std::endl;
                    break;
                }

                char del_f;
                std::cout << "\n-WORK WITH FOLDERS-" << std::endl;
                std::cout << "YOU CHOSE TO DECRYPT USING " << algorithm << std::endl;
                std::cout << "DO YOU WANT TO DELETE THE ENCRYPTED FILE AND ARCHIVE AFTER DECRYPTION? (Y/N): ";
                std::cin >> del_f;
                std::cin.ignore(1000, '\n');

                std::string pass;
                std::cout << "ENTER PASSWORD TO DECRYPT" << std::endl;
                std::cout << "Enter password: ";
                std::getline(std::cin, pass);

                const unsigned char key[16] = {
                    0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef,
                    0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21
                };

                std::vector<unsigned char> encrypted_password;
                if (!load_encrypted_password("encrypted_password.bin", encrypted_password)) {
                    std::cerr << "The saved password could not be uploaded." << std::endl;
                    break;
                }

                std::string saved_password = decrypt_password(encrypted_password, key);
                if (pass != saved_password) {
                    std::cerr << "WRONG PASSWORD" << std::endl;
                    break;
                }

                // Step 1: Decrypt archive file (produces .zip)
                std::filesystem::path decryptedZipPath = inputFile;
                decryptedZipPath.replace_extension(".zip");

                bool success = false;
                if (algorithm == "AES") {
                    success = decrypt_file_aes(inputPath, decryptedZipPath.string(), pass);
                }
                else if (algorithm == "RSA") {
                    success = decrypt_file_rsa(inputPath, decryptedZipPath.string(), "private.pem");
                }
                else if (algorithm == "CHACHA20") {
                    success = decrypt_file_chacha20(inputPath, decryptedZipPath.string(), pass);
                }

                if (!success) {
                    std::cerr << "Decryption failed." << std::endl;
                    break;
                }

                std::cout << "[+] Archive decrypted successfully: \"" << decryptedZipPath << "\"" << std::endl;

                // Step 2: Extract .zip archive
                std::filesystem::path extractFolder = decryptedZipPath;
                extractFolder.replace_extension("");  // Remove ".zip" to get original folder name

                std::string powershellCommand =
                    "powershell -Command \"Expand-Archive -Path \\\"" + decryptedZipPath.string() +
                    "\\\" -DestinationPath \\\"" + extractFolder.string() + "\\\" -Force\"";

                std::cout << "Extracting archive..." << std::endl;
                if (std::system(powershellCommand.c_str()) != 0) {
                    std::cerr << "Failed to extract the archive." << std::endl;
                    break;
                }

                std::cout << "[+] Folder successfully decrypted and extracted to: " << extractFolder << std::endl;

                if (std::filesystem::exists(decryptedZipPath)) {
                    std::filesystem::remove(decryptedZipPath);
                }
                // Step 3: Optionally delete encrypted file and .zip
                if (del_f == 'Y' || del_f == 'y') {
                    std::filesystem::remove(inputFile);         // Encrypted file (.aes/.rsa/.cha)
                    std::filesystem::remove(decryptedZipPath);  // Decrypted .zip
                    std::cout << "Encrypted file and archive deleted." << std::endl;
                }

                break;
            }
            }
            break;
        }
        case 4:
        {
            std::cout << "Completion of the program..." << std::endl;
            running = false;
            break;
        }
        }
        }
    }


bool is_encrypt_context_menu_added() {
    HKEY hKey;
    std::string keyPath = "Directory\\shell\\EncryptWithMyApp";
    return RegOpenKeyExA(HKEY_CLASSES_ROOT, keyPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS;
}

bool add_encrypt_context_menu(const std::string& appPath) {
    struct RegistryEntry {
        std::string baseKey;   // базовый путь в реестре
        std::string label;     // надпись в контекстном меню
        std::string action;    // дополнительный параметр, передаваемый в программу (encrypt/decrypt)
    };

    std::vector<RegistryEntry> entries = {
        // Шифрование
        { "Directory\\shell\\EncryptWithMyApp", "Шифровать", "encrypt" },
        { "*\\shell\\EncryptWithMyApp",         "Шифровать", "encrypt" },

        // Расшифрование
        { "Directory\\shell\\DecryptWithMyApp", "Расшифровать", "decrypt" },
        { "*\\shell\\DecryptWithMyApp",         "Расшифровать", "decrypt" }
    };

    for (const auto& entry : entries) {
        HKEY hKey, hSubKey;
        std::string commandKey = entry.baseKey + "\\command";
        std::string command = "\"" + appPath + "\" " + entry.action + " \"%1\"";

        if (RegCreateKeyExA(HKEY_CLASSES_ROOT, entry.baseKey.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
            std::cerr << "Не удалось создать ключ меню: " << entry.baseKey << std::endl;
            return false;
        }

        RegSetValueExA(hKey, nullptr, 0, REG_SZ, (BYTE*)entry.label.c_str(), (DWORD)(entry.label.length() + 1));
        RegCloseKey(hKey);

        if (RegCreateKeyExA(HKEY_CLASSES_ROOT, commandKey.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hSubKey, nullptr) != ERROR_SUCCESS) {
            std::cerr << "Не удалось создать ключ команды: " << commandKey << std::endl;
            return false;
        }

        RegSetValueExA(hSubKey, nullptr, 0, REG_SZ, (BYTE*)command.c_str(), (DWORD)(command.length() + 1));
        RegCloseKey(hSubKey);
    }

    std::cout << "[+] Команды 'Шифровать' и 'Расшифровать' добавлены для файлов и папок." << std::endl;
    return true;
}

void encrypt_silent_mode(const std::string& inputPath) {
    const unsigned char key[16] = {
        0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21
    };

    std::vector<unsigned char> encrypted_password;
    if (!load_encrypted_password("encrypted_password.bin", encrypted_password)) {
        std::cerr << "Не удалось загрузить сохранённый пароль." << std::endl;
        return;
    }

    std::string password = decrypt_password(encrypted_password, key);

    std::string algorithm = load_default_algorithm("default_algorithm.txt");

    std::filesystem::path inputFile(inputPath);
    std::filesystem::path outputFile = inputFile;

    if (algorithm == "AES") outputFile += ".aes";
    else if (algorithm == "DES") outputFile += ".des";
    else if (algorithm == "RSA") outputFile += ".rsa";
    else if (algorithm == "CHACHA20") outputFile += ".cha";
    else {
        std::cerr << "Неизвестный алгоритм: " << algorithm << std::endl;
        return;
    }

    bool success = false;
    if (algorithm == "AES") {
        success = encrypt_file_aes(inputPath, outputFile.string(), password);
    }
    else if (algorithm == "CHACHA20") {
        success = encrypt_file_chacha20(inputPath, outputFile.string(), password);
    }
    else if (algorithm == "RSA") {
        success = encrypt_file_rsa(inputPath, outputFile.string(), "public.pem");
    }

    if (success) {
        std::cout << "Файл успешно зашифрован: " << outputFile << std::endl;
    }
    else {
        std::cerr << "Ошибка при шифровании." << std::endl;
    }
}

void decrypt_silent_mode(const std::string& inputPath) {
    const unsigned char key[16] = {
        0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21
    };

    std::vector<unsigned char> encrypted_password;
    if (!load_encrypted_password("encrypted_password.bin", encrypted_password)) {
        std::cerr << "Не удалось загрузить сохранённый пароль." << std::endl;
        return;
    }

    std::string password = decrypt_password(encrypted_password, key);

    std::filesystem::path inputFile(inputPath);
    std::string extension = inputFile.extension().string();
    std::filesystem::path outputFile = inputFile;
    outputFile.replace_extension("");  // удаляем .aes/.rsa/.cha и т.д.
    outputFile += "_decrypted.txt";

    bool success = false;
    if (extension == ".aes") {
        success = decrypt_file_aes(inputPath, outputFile.string(), password);
    }
    else if (extension == ".cha") {
        success = decrypt_file_chacha20(inputPath, outputFile.string(), password);
    }
    else if (extension == ".rsa") {
        success = decrypt_file_rsa(inputPath, outputFile.string(), "private.pem");
    }
    else {
        std::cerr << "Неизвестное расширение файла: " << extension << std::endl;
        return;
    }

    if (success) {
        std::cout << "Файл успешно расшифрован: " << outputFile << std::endl;
    }
    else {
        std::cerr << "Ошибка при расшифровке." << std::endl;
    }
}


int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);

    if (!is_encrypt_context_menu_added()) {
        if (!add_encrypt_context_menu(path)) {
            std::cerr << "Ошибка при добавлении команды в контекстное меню!" << std::endl;
        }
    }

    if (argc == 2) {
        std::string inputPath = argv[1];
        encrypt_silent_mode(inputPath);  // шифруем по умолчанию
    }
    else if (argc >= 3) {
        std::string action = argv[1];
        std::string targetPath = argv[2];

        if (action == "encrypt") {
            encrypt_silent_mode(targetPath);
        }
        else if (action == "decrypt") {
            decrypt_silent_mode(targetPath);
        }
        else {
            std::cerr << "Неизвестное действие: " << action << std::endl;
        }
    }
    else {
        encrypt_console_mode();  // обычный режим
    }

    return 0;
}
