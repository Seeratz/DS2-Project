#include "trapdoor.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    cout << "Secure Messaging System using Trapdoor Hashing & RSA\n";

    // Register Users
    cout << "Enter username for User A: ";
    string userA;
    cin >> userA;
    users[userA] = generateKeys(userA);

    cout << "Enter username for User B: ";
    string userB;
    cin >> userB;
    users[userB] = generateKeys(userB);

    // User A sends a message
    cout << userA << ", enter message to send: ";
    string message;
    cin.ignore();
    getline(cin, message);

    // Hashing before encryption
    string hashedMessage = computeSHA256(message);
    cout << "SHA-256 Hash: " << hashedMessage << endl;

    // Encrypt message using User B's public key
    vector<unsigned char> encryptedMessage = encryptMessage(message, users[userB].publicKey);
    if (encryptedMessage.empty()) {
        cerr << "Encryption failed. Exiting.\n";
        return 1;
    }
    cout << "Encrypted Message Sent to " << userB << endl;

    // User B receives the message
    cout << userB << " received the message.\nDecrypting...\n";
    string decryptedMessage = decryptMessage(encryptedMessage, users[userB].privateKey);

    cout << "Decrypted Message: " << decryptedMessage << endl;

    return 0;
}
