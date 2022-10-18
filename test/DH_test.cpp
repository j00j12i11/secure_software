#include <iostream>
#include <gmpxx.h>

using namespace std;

// system parameter
mpz_class p{"0xFFFFFFFFFFFFFFFFADF85458A2BB4A9AAFDC5620273D3CF1D8B9C583CE2D3695A9E13641146433FBCC939DCE249B3EF97D2FE363630C75D8F681B202AEC4617AD3DF1ED5D5FD65612433F51F5F066ED0856365553DED1AF3B557135E7F57C935984F0C70E0E68B77E2A689DAF3EFE8721DF158A136ADE73530ACCA4F483A797ABC0AB182B324FB61D108A94BB2C8E3FBB96ADAB760D7F4681D4F42A3DE394DF4AE56EDE76372BB190B07A7C8EE0A6D709E02FCE1CDF7E2ECC03404CD28342F619172FE9CE98583FF8E4F1232EEF28183C3FE3B1B4C6FAD733BB5FCBC2EC22005C58EF1837D1683B2C6F34A26C1B2EFFA886B4238611FCFDCDE355B3B6519035BBC34F4DEF99C023861B46FC9D6E6C9077AD91D2691F7F7EE598CB0FAC186D91CAEFE130985139270B4130C93BC437944F4FD4452E2D74DD364F2E21E71F54BFF5CAE82AB9C9DF69EE86D2BC522363A0DABC521979B0DEADA1DBF9A42D5C4484E0ABCD06BFA53DDEF3C1B20EE3FD59D7C25E41D2B66C62E37FFFFFFFFFFFFFFFF"};
mpz_class g = 2;

// difiie hellman 키교환을 수행하는 다음 함수를 완성하기(함수 수행결과 key_A 와 key_B에 키 값이 저장)
void DH_key_exchange(mpz_class& key_A, mpz_class& key_B);

int cmp(mpz_class A, mpz_class B);
mpz_class powm(mpz_class base, mpz_class exp, mpz_class mod);
mpz_class urandom(gmp_randstate_t state, mpz_class n);

int main(){
    mpz_class Alice, Bob;
    DH_key_exchange(Alice, Bob);

    if (cmp(Alice, Bob) == 0)
        cout << "success!" << endl;
    else 
        cout << "fail.." << endl;

    // complie -> gcc DH_test.cpp -lstdc++ -lgmpxx -lgmp -o DH_test
    return 0;
}

int cmp(mpz_class A, mpz_class B){
    return mpz_cmp(A.get_mpz_t(), B.get_mpz_t());
}
mpz_class powm(mpz_class base, mpz_class exp, mpz_class mod){
    mpz_class r;
    mpz_powm(r.get_mpz_t(), base.get_mpz_t(), exp.get_mpz_t(), mod.get_mpz_t());
    return r;
}
mpz_class urandom(gmp_randstate_t state, mpz_class n){
    mpz_class r;
    mpz_urandomm(r.get_mpz_t(), state, n.get_mpz_t());
    return r;
}
void DH_key_exchange(mpz_class& key_A, mpz_class& key_B){
    mpz_class a, b, q_a, q_b;

    gmp_randstate_t state;
    gmp_randinit_mt(state);
    a = urandom(state, p);
    b = urandom(state, p);

    q_a = powm(g, a, p);
    q_b = powm(g, b, p);

    key_A = powm(q_b, a, p);
    key_B = powm(q_a, b, p);
}