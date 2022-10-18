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

// System parameter
EC secp256k1 = {0, 7, mpz_class{"0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"}};
Point G = {mpz_class{"0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"},
           mpz_class{"0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"}};

Point G2 = {mpz_class{"0xC6047F9441ED7D6D3045406E95C07CD85C778E4B8CEF3CA7ABAC09B95C709EE5"},
            mpz_class{"0x1AE168FEA63DC339A3C58419466CEAEEF7F632653266D0E1236431A950CFE52A"}};

Point G3 = {mpz_class{"0xF9308A019258C31049344F85F89D5229B531C845836F99B08601F113BCE036F9"},
            mpz_class{"0x388F7B0F632DE8140FE337E62A37F3566500A99934C2231B6CB9FD7584B8E672"}};

Point G7 = {mpz_class{"0x5CBDF0646E5DB4EAA398F365F2EA7A0E3D419B7E0330E39CE92BDDEDCAC4F9BC"},
            mpz_class{"0x6AEBCA40BA255960A3178D6D861A54DBA813D0B813FDE7B5A5082628087264DA"}};

// 다음의 함수들을 구현하여 main이 성공적으로 실행되도록 하자.
Point EC_add(EC E, Point P, Point Q);
Point EC_double(EC E, Point P);
Point EC_mult(EC E, Point P, int r);
Point EC_mult(EC E, Point P, mpz_class r);

// 래퍼함수
mpz_class mod_inv(mpz_class x, mpz_class mod);
mpz_class mod(mpz_class x, mpz_class mod);
mpz_class urandomm(gmp_randstate_t state, mpz_class p);
int div_by_2(mpz_class& q, mpz_class n);
int cmp(mpz_class a, mpz_class b);

bool is_infinity(Point a);
vector<int> to_bits(int n);
vector<int> to_bits(mpz_class n);
// ECDH로 키를 분배해 각각 저장하는 함수.
void ECDH_key_exchange(Point& key_A, Point& key_B);

bool is_equal_point(Point P, Point Q){
    if (cmp(P.x, Q.x) == 0 && cmp(P.y, Q.y) == 0) return true;
    else return false;
}

int main() {
    
    //EC_add
    if (is_equal_point(EC_add(secp256k1, G, G2), G3)) 
        cout << "G + 2G == 3G" << endl;
    else 
        cout << "fail" << endl;

    //EC_double
    if (is_equal_point(EC_double(secp256k1, G), G2))
        cout << "G double == 2G" << endl;
    else 
        cout << "fail" << endl;

    //EC_mult
    if (is_equal_point(EC_mult(secp256k1, G, mpz_class{7}), G7))
        cout << "G * 7 == 7G" << endl;
    else 
        cout << "fail" << endl;

    //ECDH key exchange
    Point Alice, Bob;
    ECDH_key_exchange(Alice, Bob);
    if (is_equal_point(Alice, Bob))
        cout << "success" << endl;
    else 
        cout << "fail" << endl;

    // complie -> gcc ECDH_test.cpp -lstdc++ -lgmpxx -lgmp -o ECDH_test
    return 0;
}


mpz_class mod_inv(mpz_class x, mpz_class mod){
    mpz_class r;
    if(mpz_invert(r.get_mpz_t(), x.get_mpz_t(), mod.get_mpz_t()) == 0)
        cout << "error" << endl;
    return r;
}
mpz_class mod(mpz_class x, mpz_class mod){
    mpz_class r;
    mpz_mod(r.get_mpz_t(), x.get_mpz_t(), mod.get_mpz_t());
    return r;
}
mpz_class urandomm(gmp_randstate_t state, mpz_class p){
    mpz_class r;
    mpz_urandomm(r.get_mpz_t(), state, p.get_mpz_t());
    return r;
}
int div_by_2(mpz_class& q, mpz_class n){
    mpz_class r; mpz_class d{2};
    mpz_tdiv_qr(q.get_mpz_t(), r.get_mpz_t(), n.get_mpz_t(), d.get_mpz_t());
    return r.get_ui();
}
int cmp(mpz_class a, mpz_class b){
    return mpz_cmp(a.get_mpz_t(), b.get_mpz_t());
}

bool is_infinity(Point a){
    if (cmp(a.x, mpz_class{0}) == 0 && cmp(a.y, mpz_class{0}) == 0)
        return true;
    else 
        return false;
}
Point EC_add(EC E, Point P, Point Q){
    if(is_infinity(P)) return Q;
    if(is_infinity(Q)) return P;

    Point R;

    mpz_class lam = mod(mod(Q.y-P.y, E.p) * mod_inv(Q.x - P.x, E.p), E.p);

    R.x = mod(lam * lam - P.x - Q.x, E.p);
    R.y = mod(lam * (P.x - R.x) - P.y, E.p);

    return R;
}
Point EC_double(EC E, Point P){
    Point R;

    mpz_class lam = mod(mod(3*P.x*P.x + E.a, E.p) * mod_inv(2*P.y, E.p), E.p);

    R.x = mod(lam * lam - 2*P.x, E.p);
    R.y = mod(lam*(P.x - R.x) - P.y, E.p);

    return R;
}

vector<int> to_bits(int n){
    vector<int> bits;
    /*
    for ( ; n > 0; n/2)
        bits.insert(bits.begin(), n % 2);
    */

    while(n > 0){
        bits.insert(bits.begin(), n % 2);
        n /= 2;
    }
    return bits;
}
vector<int> to_bits(mpz_class n){
    vector<int> bits;
    int b;
    
    while(cmp(n, mpz_class{0})){
        b = div_by_2(n, n);
        bits.insert(bits.begin(), b);    
    }

    return bits;
}
Point EC_mult(EC E, Point P, mpz_class r){
    vector<int> bits = to_bits(r);

    Point R = P;
    for (int i=1; i<bits.size(); i++){
        R = EC_double(E, R);
        if (bits.at(i) == 1){
            R = EC_add(E, R, P);
        }
    }

    return R;
}

void ECDH_key_exchange(Point& key_A, Point& key_B){
    mpz_class a, b;
    Point Q_a, Q_b;
    gmp_randstate_t state;
    gmp_randinit_mt(state);

    a = urandomm(state, secp256k1.p);
    b = urandomm(state, secp256k1.p);

    Q_a = EC_mult(secp256k1, G, a);    
    Q_b = EC_mult(secp256k1, G, b);    
    
    key_A = EC_mult(secp256k1, Q_b, a);
    key_B = EC_mult(secp256k1, Q_a, b);
}