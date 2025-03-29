#ifndef TRAPDOOR_H
#define TRAPDOOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <openssl/rsa.h>

// User structure to store keys
struct User {
    std::string username;
    RSA* privateKey;
    RSA* publicKey;
};

// Declare users map as extern to avoid "undefined identifier" error
extern std::unordered_map<std::string, User> users;

std::string computeSHA256(const std::string& data);
User generateKeys(std::string username);
std::vector<unsigned char> encryptMessage(const std::string& message, RSA* publicKey);
std::string decryptMessage(const std::vector<unsigned char>& encryptedMessage, RSA* privateKey);

#endif  // TRAPDOOR_H
