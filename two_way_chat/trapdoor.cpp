// trapdoor.cpp
#include "trapdoor.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>

// Performs modular exponentiation: (base^exp) % mod efficiently
// Used in both encryption, decryption, signing, and verification
ll modExp(ll base, ll exp, ll mod) {
    ll result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

// Checks whether a number is prime using trial division
bool isPrime(ll n) {
    if (n < 2) return false;
    for (ll i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

// Generates a random prime number between lower and upper bounds
ll generatePrime(ll lower, ll upper) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<ll> dist(lower, upper);

    ll num;
    do {
        num = dist(gen);
    } while (!isPrime(num)); // Keep generating until a prime is found
    return num;
}

// Computes the greatest common divisor (GCD) using Euclidean Algorithm
ll gcd(ll a, ll b) {
    return b == 0 ? a : gcd(b, a % b);
}

// Computes modular inverse of a under modulo m using Extended Euclidean Algorithm
// Returns x such that (a * x) % m == 1
ll modInverse(ll a, ll m) {
    ll m0 = m, t, q;
    ll x0 = 0, x1 = 1;
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

// Generates RSA keys: public (e, n), private (d, n)
RSAKey generateRSAKeys() {
    // Step 1: Generate two distinct random primes
    ll p = generatePrime(50, 200);
    ll q = generatePrime(50, 200);

    // Step 2: Compute n and phi(n)
    ll n = p * q;
    ll phi = (p - 1) * (q - 1);

    // Step 3: Choose public exponent e such that gcd(e, phi) = 1
    ll e;
    do {
        e = generatePrime(2, phi - 1);
    } while (gcd(e, phi) != 1);

    // Step 4: Compute private exponent d such that (e * d) % phi = 1
    ll d = modInverse(e, phi);

    return {n, e, d}; // Return RSA key structure
}

// Simple hash function to simulate SHA-256 (not secure for real use)
// Hashes input into a 32-bit hex string using base-31 polynomial rolling
std::string simpleSHA256(const std::string& input) {
    unsigned int hash = 0;
    for (char c : input)
        hash = (hash * 31 + c) % 1000000007;

    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << hash;
    return ss.str(); // Return hex string representation
}

// Signs a message using the private key (d, n)
ll signMessage(const std::string& message, ll d, ll n) {
    std::string hash = simpleSHA256(message); // Step 1: Hash the message
    ll hashNum = std::stoll(hash, nullptr, 16) % n; // Convert hash to a number mod n
    ll sign = modExp(hashNum, d, n); // Step 2: Sign the hash using private key
    std::cout << "sign using priv key " << sign << "\n";
    return sign; // Return the digital signature
}

// Verifies a digital signature using the public key (e, n)
bool verifySignature(const std::string& message, ll signature, ll e, ll n) {
    std::string hash = simpleSHA256(message); // Step 1: Hash the original message
    std::cout << "simple hash verification " << hash << "\n";

    ll expectedHash = std::stoll(hash, nullptr, 16) % n; // Convert hash to number
    ll decryptedSignature = modExp(signature, e, n); // Step 2: Decrypt signature with public key

    std::cout << "decryptedSignature " << decryptedSignature << "\n";
    std::cout << "expected decryptedSignature " << expectedHash << "\n"; 

    return decryptedSignature == expectedHash; // Step 3: Compare hashes
}
