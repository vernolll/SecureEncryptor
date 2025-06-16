# An application for encrypting files and folders

## Description

This is an application for encrypting and decrypting files and folders using various encryption algorithms. The application provides a choice between AES, RSA and ChaCha20 algorithms.  The "fast encryption/decryption" feature is also implemented, which allows you to quickly encrypt or decrypt a file/folder using the default algorithm.

## Functionality

• **Encryption and decryption:** Encrypts and decrypts files and folders, ensuring data privacy.
• **Choosing an encryption algorithm:** Supports three encryption algorithms:
    •   **AES (Advanced Encryption Standard):** Symmetric encryption algorithm, providing high speed and security.
    •   **RSA (Rivest–Shamir–Adleman):** An asymmetric encryption algorithm that uses a pair of keys (public and private) for encryption and decryption.
    •   **ChaCha20:* Symmetric stream cipher, known for its high speed and security, especially on platforms without AES hardware support.
• **Fast encryption/decryption (context menu):** Adds an option to the context menu of files and folders in your operating system that allows you to quickly encrypt or decrypt a file/folder using the default encryption algorithm.
• **Default encryption algorithm:** Allows the user to select the default encryption algorithm to be used for fast encryption/decryption.  The algorithm can be selected in the menu of the console application.

## Dependencies
• OpenSSL (for cryptographic operations).
