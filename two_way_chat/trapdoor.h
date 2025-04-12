// trapdoor.h
#pragma once
#include <string>
using ll = long long;

struct RSAKey {
    ll n, e, d;
};

ll modExp(ll base, ll exp, ll mod);
ll gcd(ll a, ll b);
ll modInverse(ll a, ll m);
bool isPrime(ll n);
ll generatePrime(ll lower, ll upper);
RSAKey generateRSAKeys();
std::string simpleSHA256(const std::string& input);
ll signMessage(const std::string& message, ll d, ll n);
bool verifySignature(const std::string& message, ll signature, ll e, ll n);
