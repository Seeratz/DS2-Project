#include "trapdoor.h"
#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include <unordered_map>

using namespace std;

// Define users map
unordered_map<string, User> users;

// Function to generate RSA key pair
User generateKeys(string username) {
    User user;
    user.username = username;

    // Generate RSA key
    RSA* keypair = RSA_new();
    BIGNUM* e = BN_new();
    BN_set_word(e, RSA_F4);
    RSA_generate_key_ex(keypair, 2048, e, NULL);
    BN_free(e);

    user.privateKey = keypair;

    // Extract public key
    BIO* pubBio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(pubBio, keypair);
    user.publicKey = PEM_read_bio_RSAPublicKey(pubBio, NULL, NULL, NULL);
    BIO_free(pubBio);

    return user;
}

// Compute SHA-256 hash of a message
string computeSHA256(const string& message) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)message.c_str(), message.length(), hash);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// Encrypt message with public key
vector<unsigned char> encryptMessage(const string& message, RSA* publicKey) {
    vector<unsigned char> encrypted(RSA_size(publicKey));
    int encryptedLength = RSA_public_encrypt(
        message.length(), 
        (unsigned char*)message.c_str(), 
        encrypted.data(), 
        publicKey, 
        RSA_PKCS1_OAEP_PADDING
    );

    if (encryptedLength == -1) {
        cerr << "Error encrypting message: " << ERR_error_string(ERR_get_error(), NULL) << endl;
        return {};
    }

    return encrypted;
}

// Decrypt message with private key
string decryptMessage(const vector<unsigned char>& encryptedMessage, RSA* privateKey) {
    vector<unsigned char> decrypted(RSA_size(privateKey));
    int decryptedLength = RSA_private_decrypt(
        encryptedMessage.size(), 
        encryptedMessage.data(), 
        decrypted.data(), 
        privateKey, 
        RSA_PKCS1_OAEP_PADDING
    );

    if (decryptedLength == -1) {
        cerr << "Error decrypting message: " << ERR_error_string(ERR_get_error(), NULL) << endl;
        return "";
    }

    return string(decrypted.begin(), decrypted.begin() + decryptedLength);
}
