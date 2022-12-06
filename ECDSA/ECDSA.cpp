#include <iostream>
#include <vector>
#include <gmpxx.h>

using namespace std;

struct Point {
    mpz_class x;
    mpz_class y;
};
struct EC {
    mpz_class a;
    mpz_class b; 
    mpz_class p;
};
struct ECDSA_PK{
    EC E;
    Point G;
    mpz_class n;
    Point Q;
};
struct ECDSA_SK{
    EC E;
    Point G;
    mpz_class n;
    Point Q;
    mpz_class d;
};
struct ECDSA_SIG{
    mpz_class S1;
    mpz_class S2;
};

mpz_class urandomm(gmp_randstate_t state, mpz_class mod);
mpz_class mod (mpz_class n, mpz_class p);
mpz_class mod_inv(mpz_class x, mpz_class mod);
int cmp(mpz_class A, mpz_class B);

vector<bool> bits_inv(mpz_class r);
Point EC_add(EC E, Point P, Point Q);
Point EC_double(EC E, Point P);
Point EC_mult(EC E, Point P, mpz_class n);

void ECDSA_keygen(ECDSA_PK *pk, ECDSA_SK *sk);
ECDSA_SIG ECDSA_sign(ECDSA_SK sk, mpz_class msg);
bool ECDSA_verify(ECDSA_PK pk, ECDSA_SIG sig, mpz_class msg);

int main(){
    EC E{2, 2, 17};
    Point G{5, 1};
    
    Point R = EC_mult(E, G, 21);

    cout << "result R       -> " << R.x << "\t" << R.y << endl;
    
    return 0;
    // gcc ECDSA.cpp –lstdc++ –lgmpxx –lgmp –o ECDSA
}

mpz_class urandomm(gmp_randstate_t state, mpz_class mod){
    mpz_class r;
    mpz_urandomm(r.get_mpz_t(), state, mod.get_mpz_t());
    return r;
}
mpz_class mod (mpz_class n, mpz_class p){
    mpz_class r;
    mpz_mod(r.get_mpz_t(), n.get_mpz_t(), p.get_mpz_t());
    return r;
}
mpz_class mod_inv(mpz_class x, mpz_class mod){
    mpz_class r;
    mpz_invert(r.get_mpz_t(), x.get_mpz_t(), mod.get_mpz_t());
    return r;
}
int cmp(mpz_class A, mpz_class B){
    return mpz_cmp(A.get_mpz_t(), B.get_mpz_t());
}

vector<bool> bits_inv(mpz_class r){
    vector<bool> bits;
    for(; r > 0; r /= 2){
        bits.push_back(r % 2 == 1);
    }
    return bits;
}
Point EC_add(EC E, Point P, Point Q){
    Point R;
    if (P.x == 0 && P.y == 0) return Q;
    if (Q.x == 0 && Q.y == 0) return P;
    if (P.x == Q.x && P.y == Q.y) return EC_double(E, P);

    mpz_class lambda = mod_inv(Q.x-P.x, E.p);
    if (lambda == 0) {
        R.x = 0; R.y = 0;
        return R;
    }
    lambda = mod((Q.y-P.y) * lambda, E.p);
    
    R.x = mod(lambda * lambda - P.x - Q.x, E.p);
    R.y = mod(lambda * (P.x - R.x) - P.y, E.p);

    return R;
}
Point EC_double(EC E, Point P){
    Point R;
    
    mpz_class lambda = mod_inv(2*P.y, E.p);
    if (lambda == 0) {
        R.x = R.y = 0; return R;
    }
    lambda = mod((3*P.x*P.x+E.a)*lambda, E.p);

    R.x = mod(lambda*lambda - 2*P.x, E.p);
    R.y = mod(lambda * (P.x-R.x) - P.y, E.p);

    return R;
}
Point EC_mult(EC E, Point P, mpz_class r){
    Point R;
    vector<bool> bits = bits_inv(r);
    int n = bits.size();

    R = P;
    for(; n>1; n--){
        R = EC_double(E, R);
        if (bits.at(n-2) == 1)
            R = EC_add(E, R, P);
    }

    return R;
}