#include <iostream>
#include <vector>
#include <gmpxx.h>

using namespace std;

// 포인트를 표현하는 구조체
struct Point {
    mpz_class x;
    mpz_class y;
};
// 타원곡선을 표현하는 구조체 (타원곡선: y^2 = x^3 + a*x + b (mod p))
struct EC {
    mpz_class a;
    mpz_class b;
    mpz_class p;
};

// ----------- wrapper functions ---------------------
mpz_class mod_inv(mpz_class x, mpz_class p); // 모드상의 역원
int cmp(mpz_class x, mpz_class y); // 두 값의 크기차 비교
mpz_class mod(mpz_class n, mpz_class p); // 모드연산
int mpz_div_by_2(mpz_class& q, mpz_class n); // 2로나눌때의 나머지와 몫
mpz_class urandomm(mpz_class p); // P보다 작은 값을 랜덤하게 선택

vector<int> to_bits(mpz_class x); // 주어진 값을 비트로 변환
bool is_inf_point(Point P); // point of infinity인지 확인
bool is_equal_point(Point P, Point Q); // 두 포인트가 같은지 확인
Point EC_add(EC E, Point P, Point Q); // 타원곡선 상의 두 점 더하기
Point EC_double(EC E, Point P); // 타원곡선상의 한 점 더블링
Point EC_mult(EC E, Point P, mpz_class r); // 타원곡선상의 스칼라 곱


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

int main() {
    
    cout << "[test] - EC_add" << endl;
    if (is_equal_point(EC_add(secp256k1, G, G2), G3))
        cout << "G + 2G == 3G" << endl;
    else 
        cout << "fail" << endl;

    cout << "\n[test] - EC_double" << endl;
    if (is_equal_point(EC_double(secp256k1, G), G2))
        cout << "G double == 2G" << endl;
    else 
        cout << "fail" << endl;

    cout << "\n[test] - EC_mult" << endl;
    if (is_equal_point(EC_mult(secp256k1, G, mpz_class{7}), G7))
        cout << "G * 7 == 7G" << endl;
    else 
        cout << "fail" << endl;

    cout << "\n------- Diffie Hellman key exchange -------\n" << endl;
    mpz_class a, b;
    Point temp_a, temp_b, a_key, b_key;

    a = urandomm(secp256k1.p);
    b = urandomm(secp256k1.p);

    temp_a = EC_mult(secp256k1, G, a);
    temp_b = EC_mult(secp256k1, G, b);

    a_key = EC_mult(secp256k1, temp_b, a);
    b_key = EC_mult(secp256k1, temp_a, b);

    if (is_equal_point(a_key, b_key))
        cout << "--> success" << endl;
    else 
        cout << "--> fail" << endl;

    // complie -> gcc ECDH_gmp.cpp -lstdc++ -lgmpxx -lgmp -o ECDH_gmp
    return 0;
}

// 단순 래퍼함수들... 별도의 설명 추가하지 않음
mpz_class mod_inv(mpz_class x, mpz_class p){
    mpz_class result;
    if (mpz_invert(result.get_mpz_t(), x.get_mpz_t(), p.get_mpz_t()) == 0)
        cout << "error. result will be empty" << endl;
    return result;
}
int cmp(mpz_class x, mpz_class y){
    return mpz_cmp(x.get_mpz_t(), y.get_mpz_t());
}
mpz_class mod(mpz_class n, mpz_class p){
    mpz_class result;
    mpz_mod(result.get_mpz_t(), n.get_mpz_t(), p.get_mpz_t());
    return result;
}
int mpz_div_by_2(mpz_class& q, mpz_class n){
    mpz_class r; mpz_class d{2};
    mpz_tdiv_qr(q.get_mpz_t(), r.get_mpz_t(), n.get_mpz_t(), d.get_mpz_t());
    return r.get_ui();
}
mpz_class urandomm(mpz_class p){
    gmp_randstate_t state;
    gmp_randinit_mt(state);

    mpz_class result;
    mpz_urandomm(result.get_mpz_t(), state, p.get_mpz_t());
    return result;
}

// mpz_class변수를 2진수로 벡터에 저장하는 함수
vector<int> to_bits(mpz_class x){
    vector<int> bits; int b;
    // x가 0과 같거나 작을때까지 반복. (x에는 몫이 반복적으로 저장됨)
    while (cmp(x, mpz_class{0})){
        // 함수 실행 결과, x에는 몫이 다시 저장되고 b는 나머지가 저장된다.
        b = mpz_div_by_2(x, x);
        // 항상 벡터의 맨 앞에 값을 추가 -> 벡터 0번 인덱스에 2진수의 맨 앞 숫자가 오도록
        bits.insert(bits.begin(), b);
    }
    return bits;
}
// 점의 x와 y가 모두 0이면 point of infinity임
bool is_inf_point(Point P){
    if (cmp(P.x, mpz_class{0}) == 0 && cmp(P.y, mpz_class{0}) == 0)
        return true;
    else 
        return false;
}
// 두 점의 x값과 y값이 같은지 확인한다.
bool is_equal_point(Point P, Point Q){
    if (cmp(P.x, Q.x) == 0 && cmp(P.y, Q.y) == 0)
        return true;
    else 
        return false;    
}
// 타원곡선상의 두 점 덧셈
Point EC_add(EC E, Point P, Point Q){
    // point of infinity는 항등원이므로,,
    if (is_inf_point(P)) return Q;
    if (is_inf_point(Q)) return P;

    // 결과를 저장할 포인트
    Point R;

    mpz_class lambda = mod(Q.y - P.y, E.p) * mod_inv(Q.x - P.x, E.p);
    lambda = mod(lambda, E.p);

    R.x = mod(lambda * lambda - P.x - Q.x, E.p);
    R.y = mod(lambda * (P.x - R.x) - P.y, E.p);

    return R;
}
Point EC_double(EC E, Point P){
    // 결과를 저장할 포인트
    Point R;

    mpz_class lambda = mod(mod((3*(P.x*P.x) + E.a), E.p) * mod_inv(2*P.y, E.p), E.p);

    R.x = mod((lambda * lambda) - 2*P.x, E.p);
    R.y = mod(lambda * (P.x - R.x) - P.y, E.p);

    return R;
}
Point EC_mult(EC E, Point P, mpz_class r){
    // 결과를 저장할 포인트
    Point R;
    // 숫자를 2진수로 바꿔서 벡터에 저장한다.
    vector<int> bits = to_bits(r);

    // 2진수의 맨 앞자리는 무조건 1이므로, R = P 부터 한다.
    R = P;

    for(int i=1; i<bits.size(); i++){
        // 자리수가 올라갈 때마다 더블링
        R = EC_double(E, R);
        if (bits.at(i) == 1)
            // 비트가 1일 경우 더하기
            R = EC_add(E, R, P);
    }

    return R;
}