#include "user_manager.h"
#include <fstream>
#include <sstream>

std::unordered_map<std::string, User> userDatabase;

bool loadUsersFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    userDatabase.clear();
    std::string username;
    ll n, e, d;
    while (file >> username >> n >> e >> d) {
        User user;
        user.username = username;
        user.key = RSAKey{n, e, d};
        userDatabase[username] = user;
    }

    file.close();
    return true;
}

bool saveUsersToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    for (const auto& [username, user] : userDatabase) {
        file << user.username << " " << user.key.n << " " << user.key.e << " " << user.key.d << "\n";
    }

    file.close();
    return true;
}

bool userExists(const std::string& username) {
    return userDatabase.find(username) != userDatabase.end();
}

RSAKey getUserKey(const std::string& username) {
    return userDatabase[username].key;
}

void addUser(const std::string& username, const RSAKey& key) {
    userDatabase[username] = User{username, key};
}
