#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <iomanip>
#include <sstream>

using namespace std;
using ll = long long;

// Function to compute (base^exp) % mod using Modular Exponentiation
ll modExp(ll base, ll exp, ll mod) {
    ll result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

// Function to check if a number is prime using simple trial division
bool isPrime(ll n) {
    if (n < 2) return false;
    for (ll i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

// Function to generate a random prime number
ll generatePrime(ll lower, ll upper) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<ll> dist(lower, upper);

    ll num;
    do {
        num = dist(gen);
    } while (!isPrime(num));
    return num;
}

// Function to compute GCD
ll gcd(ll a, ll b) {
    return b == 0 ? a : gcd(b, a % b);
}

// Function to compute Modular Inverse using Extended Euclidean Algorithm
ll modInverse(ll a, ll m) {
    ll m0 = m, t, q;
    ll x0 = 0, x1 = 1;
    
    if (m == 1) return 0;

    while (a > 1) {
        q = a / m;
        t = m;
        m = a % m;
        a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    return (x1 + m0) % m0;
}

// RSA Key Generation
struct RSAKey {
    ll n, e, d;
};

RSAKey generateRSAKeys() {
    ll p = generatePrime(50, 200);  // Generate a small prime for demonstration
    ll q = generatePrime(50, 200);

    ll n = p * q;
    ll phi = (p - 1) * (q - 1);

    ll e;
    do {
        e = generatePrime(2, phi - 1);
    } while (gcd(e, phi) != 1);

    ll d = modInverse(e, phi);

    return {n, e, d};
}

// RSA Encryption
ll encrypt(ll message, ll e, ll n) {
    return modExp(message, e, n);
}

// RSA Decryption
ll decrypt(ll cipher, ll d, ll n) {
    return modExp(cipher, d, n);
}

// Custom SHA-256 Hash Function (Simplified)
string simpleSHA256(const string& input) {
    unsigned int hash = 0;
    for (char c : input) {
        hash = (hash * 31 + c) % 1000000007;  // Simple hash (not secure like real SHA-256)
    }
    stringstream ss;
    ss << hex << setw(8) << setfill('0') << hash;
    return ss.str();
}

// Trapdoor Hash Signing (Signing the hash using RSA private key)
ll signMessage(const string& message, ll d, ll n) {
    string hash = simpleSHA256(message);
    ll hashNum = stoll(hash, nullptr, 16) % n; // Convert hash to number and reduce mod n
    return modExp(hashNum, d, n);
}

// Trapdoor Hash Verification (Verifying the signature with RSA public key)
bool verifySignature(const string& message, ll signature, ll e, ll n) {
    string hash = simpleSHA256(message);
    ll hashNum = stoll(hash, nullptr, 16) % n;
    ll decryptedHash = modExp(signature, e, n);
    return hashNum == decryptedHash;
}

int main() {
    // Generate RSA keys
    RSAKey key = generateRSAKeys();
    cout << "Public Key (n, e): (" << key.n << ", " << key.e << ")\n";
    cout << "Private Key (n, d): (" << key.n << ", " << key.d << ")\n";

    // Message to encrypt
    string msg = "SecureMessage";
    cout << "Original Message: " << msg << endl;

    // Compute SHA-256 hash of the message
    string hash = simpleSHA256(msg);
    cout << "SHA-256 Hash of Message: " << hash << endl;

    // Sign the hash using the private key (Trapdoor Hashing)
    ll signature = signMessage(msg, key.d, key.n);
    cout << "Signature: " << signature << endl;

    // Verify the signature using the public key
    bool isValid = verifySignature(msg, signature, key.e, key.n);
    cout << "Signature Verification: " << (isValid ? "Valid" : "Invalid") << endl;

    return 0;
}
