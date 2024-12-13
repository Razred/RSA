#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cassert>
#include <cmath>
#include <string>

using namespace std;

uint64_t mod_exp(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

uint64_t gcd(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

bool is_prime(uint64_t n, int k = 5) {
    if (n <= 1 || n == 4) return false;
    if (n <= 3) return true;

    for (int i = 0; i < k; ++i) {
        uint64_t a = 2 + rand() % (n - 4);
        if (mod_exp(a, n - 1, n) != 1)
            return false;
    }
    return true;
}

uint64_t generate_prime(uint64_t lower, uint64_t upper) {
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dist(lower, upper);

    uint64_t prime;
    do {
        prime = dist(gen);
    } while (!is_prime(prime));
    return prime;
}

uint64_t mod_inverse(uint64_t a, uint64_t m) {
    int64_t m0 = m, t, q;
    int64_t x0 = 0, x1 = 1;

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

    if (x1 < 0) x1 += m0;

    return x1;
}

void generate_keys(uint64_t &n, uint64_t &e, uint64_t &d, uint64_t &p, uint64_t &q) {
    p = generate_prime(10000, 50000);
    q = generate_prime(10000, 50000);
    while (abs((int64_t)(p - q)) < 1000) {
        q = generate_prime(10000, 50000);
    }

    n = p * q;
    uint64_t phi = (p - 1) * (q - 1);

    do {
        e = generate_prime(2, phi - 1);
    } while (gcd(e, phi) != 1);

    d = mod_inverse(e, phi);
}

uint64_t encrypt_char(char ch, uint64_t e, uint64_t n) {
    return mod_exp(static_cast<uint64_t>(ch), e, n);
}

char decrypt_char(uint64_t encrypted_char, uint64_t d, uint64_t p, uint64_t q) {
    uint64_t dp = d % (p - 1);
    uint64_t dq = d % (q - 1);
    uint64_t qinv = mod_inverse(q, p);

    uint64_t m1 = mod_exp(encrypted_char, dp, p);
    uint64_t m2 = mod_exp(encrypted_char, dq, q);
    int64_t h = (qinv * (m1 - m2 + p)) % p;
    if (h < 0) h += p;

    uint64_t decrypted_char = m2 + h * q;
    return static_cast<char>(decrypted_char);
}

vector<uint64_t> encrypt_message(const string &message, uint64_t e, uint64_t n) {
    vector<uint64_t> encrypted;
    for (char ch : message) {
        encrypted.push_back(encrypt_char(ch, e, n));
    }
    return encrypted;
}

string decrypt_message(const vector<uint64_t> &encrypted_message, uint64_t d, uint64_t p, uint64_t q) {
    string decrypted;
    for (uint64_t encrypted_char : encrypted_message) {
        decrypted += decrypt_char(encrypted_char, d, p, q);
    }
    return decrypted;
}

int main() {
    srand(chrono::high_resolution_clock::now().time_since_epoch().count());

    uint64_t n, e, d, p, q;
    generate_keys(n, e, d, p, q);

    cout << "Generated keys:" << endl;
    cout << "Public key: (e = " << e << ", n = " << n << ")" << endl;
    cout << "Private key: (d = " << d << ", p = " << p << ", q = " << q << ")" << endl;

    string message;
    cout << "Enter a message to encrypt: ";
    getline(cin, message);

    vector<uint64_t> encrypted_message = encrypt_message(message, e, n);

    cout << "Encrypted message: ";
    for (uint64_t part : encrypted_message) {
        cout << part << " ";
    }
    cout << endl;

    string decrypted_message = decrypt_message(encrypted_message, d, p, q);
    cout << "Decrypted message: " << decrypted_message << endl;

    assert(message == decrypted_message);
    cout << "Encryption and decryption successful!" << endl;

    return 0;
}
