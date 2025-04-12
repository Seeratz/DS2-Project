// trapdoor.cpp
#include "trapdoor.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>

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

bool isPrime(ll n) {
    if (n < 2) return false;
    for (ll i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

ll generatePrime(ll lower, ll upper) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<ll> dist(lower, upper);

    ll num;
    do {
        num = dist(gen);
    } while (!isPrime(num));
    return num;
}

ll gcd(ll a, ll b) {
    return b == 0 ? a : gcd(b, a % b);
}

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

RSAKey generateRSAKeys() {
    ll p = generatePrime(50, 200);
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

std::string simpleSHA256(const std::string& input) {
    unsigned int hash = 0;
    for (char c : input)
        hash = (hash * 31 + c) % 1000000007;

    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << hash;
    return ss.str();
}

ll signMessage(const std::string& message, ll d, ll n) {
    std::string hash = simpleSHA256(message);
    ll hashNum = std::stoll(hash, nullptr, 16) % n;
    // std::cout << modExp(hashNum, d, n);
    ll sign = modExp(hashNum, d, n);
    std::cout << "sign using priv key " << sign << "\n";
    //return modExp(hashNum, d, n);
    return sign;
}

bool verifySignature(const std::string& message, ll signature, ll e, ll n) {
    std::string hash = simpleSHA256(message);
    std::cout << "simple hash verification " << hash << "\n";
    ll expectedHash = std::stoll(hash, nullptr, 16) % n;
    ll decryptedSignature = modExp(signature, e, n);
    std::cout << "decryptedSignature " << decryptedSignature << "\n";
    std::cout << "expected decryptedSignature " << expectedHash << "\n"; 
    return decryptedSignature == expectedHash;
}
