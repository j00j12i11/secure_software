#include <iostream>
#include <gmpxx.h>

using namespace std;

struct RSA_PK{
    mpz_class e;
    mpz_class N;
};
struct RSA_SK{
    mpz_class e;
    mpz_class d;
    mpz_class N;
    mpz_class p;
    mpz_class q;
};

// 다음의 함수들을 구현
void RSA_keygen(RSA_PK& pk, RSA_SK& sk, int key_size);
mpz_class RSA_encrypt(RSA_PK pk, mpz_class msg);
mpz_class RSA_decrypt(RSA_SK sk, mpz_class ct);
mpz_class RSA_decrypt_crt(RSA_SK sk, mpz_class ct);
mpz_class RSA_sign(RSA_SK sk, mpz_class msg);
bool RSA_verify(RSA_PK pk, mpz_class sig, mpz_class msg);

// wrapper
mpz_class mod(mpz_class n, mpz_class d);
mpz_class urandomb(gmp_randstate_t state, unsigned int b);
mpz_class nextprime(mpz_class n);
mpz_class lcm(mpz_class a, mpz_class b);
mpz_class gcd(mpz_class a, mpz_class b);
mpz_class mod_inv(mpz_class x, mpz_class mod);
mpz_class powm(mpz_class base, mpz_class exp, mpz_class mod);
int cmp(mpz_class a, mpz_class b);

mpz_class random_prime(unsigned int bit);

int main() {
    mpz_class msg, ct, dt1, dt2, sig;
    RSA_PK pk;
    RSA_SK sk;

    msg = {"0x185351235"};

    RSA_keygen(pk, sk, 2048);
    
    cout << " -- " << endl;
    ct = RSA_encrypt(pk, msg);
    cout << " --- " << endl;
    dt1 = RSA_decrypt(sk, ct);
    cout << " ---- " << endl;
    dt2 = RSA_decrypt_crt(sk, ct);
    cout << " ----- " << endl;

    cout << "RSA encryption decryption" << endl;
    if (cmp(msg, dt1) == 0) 
        cout << "success" << endl;
    else 
        cout << "fail" << endl;

    cout << "RSA encryption decryption with CRT" << endl;
    if (cmp(msg, dt2) == 0) 
        cout << "success" << endl;
    else 
        cout << "fail" << endl;

    cout << "RSA signing and verifying" << endl;
    sig = RSA_sign(sk, msg);
    if (RSA_verify(pk, sig, msg))
        cout << "success" << endl;
    else 
        cout << "fail" << endl;

    // complie -> gcc RSA_test.cpp -lstdc++ -lgmpxx -lgmp -o RSA_test
    return 0;
}

mpz_class mod(mpz_class n, mpz_class d){
    mpz_class r;
    mpz_mod(r.get_mpz_t(), n.get_mpz_t(), d.get_mpz_t());
    return r;
}
mpz_class urandomb(gmp_randstate_t state, unsigned int b){
    mpz_class r;
    mpz_rrandomb(r.get_mpz_t(), state, b);
    return r;
}
mpz_class nextprime(mpz_class n){
    mpz_class r;
    mpz_nextprime(r.get_mpz_t(), n.get_mpz_t());
    return r;
}
mpz_class lcm(mpz_class a, mpz_class b){
    mpz_class r;
    mpz_lcm(r.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
    return r;
}
mpz_class gcd(mpz_class a, mpz_class b){
    mpz_class r;
    mpz_gcd(r.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
    return r;
}
mpz_class mod_inv(mpz_class x, mpz_class mod){
    mpz_class r;
    if (mpz_invert(r.get_mpz_t(), x.get_mpz_t(), mod.get_mpz_t()) == 0)
        cout << "error" << endl;
    return r;
}
mpz_class powm(mpz_class base, mpz_class exp, mpz_class mod){
    mpz_class r;
    mpz_powm(r.get_mpz_t(), base.get_mpz_t(), exp.get_mpz_t(), mod.get_mpz_t());
    return r;
}
int cmp(mpz_class a, mpz_class b){
    return mpz_cmp(a.get_mpz_t(), b.get_mpz_t());
}

mpz_class random_prime(unsigned int bit){
    gmp_randstate_t state;
    unsigned long seed;

    seed = time(NULL);

    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);

    mpz_class r = urandomb(state, bit);
    mpz_class p = nextprime(r);

    if (mpz_sizeinbase(p.get_mpz_t(), 2) != bit)
        return random_prime(bit);
    else
        return p;
}
/*
mpz_class random_prime(unsigned int bit){
    mpz_class num;
    unsigned long seed;
    gmp_randstate_t state;

    mpz_class prime;
    do {
        seed = time(NULL);
        gmp_randinit_mt(state);
        gmp_randseed_ui(state, seed);

        num = urandomb(state, bit);

        prime = nextprime(num);
    } while(mpz_sizeinbase(prime.get_mpz_t(), 2) != bit);
    
    return prime;
}*/
void RSA_keygen(RSA_PK& pk, RSA_SK& sk, int key_size){
    mpz_class p, q, N, ell, e, d;

    p = random_prime(key_size/2);
    q = random_prime(key_size/2);
    while (cmp(p, q) == 0) q = random_prime(key_size/2);

    N = p * q;

    ell = lcm(p-1, q-1);
    e = 0x10001;
    if (gcd(e, ell) != 1)
        e = nextprime(e);
    
    d = mod_inv(e, ell);

    pk.e = e; pk.N = N;
    sk.d = d; sk.e = e; sk.N = N; sk.p = p; sk.q = q;
}
mpz_class RSA_encrypt(RSA_PK pk, mpz_class msg){
    return powm(msg, pk.e, pk.N);
}
mpz_class RSA_decrypt(RSA_SK sk, mpz_class ct){
    return powm(ct, sk.d, sk.N);
}
mpz_class RSA_decrypt_crt(RSA_SK sk, mpz_class ct){
    mpz_class m_1, m_2, temp1, temp2;

    m_1 = powm(ct, sk.d, sk.p);
    m_2 = powm(ct, sk.d, sk.q);

    temp1 = m_1 * sk.q * mod_inv(sk.q, sk.p);
    temp2 = m_2 * sk.p * mod_inv(sk.p, sk.q);

    return mod(temp1 + temp2, sk.N);

}
mpz_class RSA_sign(RSA_SK sk, mpz_class msg){
    return powm(msg, sk.d, sk.N);
}
bool RSA_verify(RSA_PK pk, mpz_class sig, mpz_class msg){
    mpz_class dt = powm(sig, pk.e, pk.N);
    if (cmp(dt, msg) == 0)
        return true;
    else
        return false;
}