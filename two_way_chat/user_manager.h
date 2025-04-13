#pragma once
#include "trapdoor.h"
#include <string>
#include <unordered_map>

struct User {
    std::string username;
    RSAKey key;
};

extern std::unordered_map<std::string, User> userDatabase;

bool loadUsersFromFile(const std::string& filename);
bool saveUsersToFile(const std::string& filename);
bool userExists(const std::string& username);
RSAKey getUserKey(const std::string& username);
void addUser(const std::string& username, const RSAKey& key);
