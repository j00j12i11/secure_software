#include <iostream>
#include <gmpxx.h>
#include <vector>

using namespace std;

// 포인트 구조체
struct Point {
    int x;
    int y;
};

// 타원곡선 구조체: y^2 = x^3 + a*x + b (mod p)
struct EC {
    int a; // x의 계수
    int b; // 일차항
    int p; // 모드
};

// 모드상의 역원 구하는 함수
int mod_inv(int x, int mod);
// 타원곡선상의 두 점을 더하는 함수
Point EC_add(EC E, Point P, Point Q);
// 타원곡선상의 한 점을 더블링 하는 함수
Point EC_double(EC E, Point P);
// 타원곡선상의 스칼라곱 함수
Point EC_mult(EC E, Point P, int r);

// 이전에 구현한 함수(상수를 이진수로)
void to_bits(vector<int>& bits, int x);

int main(){
    EC E {2, 3, 5};
    Point G {1, 1};
    Point R;

    Point A = {1, 4};
    Point B = {3 ,1};
    R = EC_add(E, A, B);
    cout << "1P + 4P => " << R.x << ", " << R.y << endl;


    R = EC_double(E, G);
    cout << "2G => " << R.x << ", " << R.y << endl;

    R = EC_mult(E, G, 3);
    cout << "3*G => " << R.x << ", " << R.y << endl;

    R = EC_mult(E, G, 9);
    cout << "8*G => " << R.x << ", " << R.y << endl;

    return 0;
}

// 모드상의 역원 구하기
int mod_inv(int x, int mod){
    // 모드가 0 이하인 경우 에러
    if (mod <= 0) return -1;
    // x가 0인 경우, 0의 역원은 무한대...
    if (x == 0) return 0;

    // 위의 조건에 걸리지 않는 경우
    else{
        // x가 음수라면 모드를 더해 양수로 만든다.
        while (x < 0){
            x += mod;
        }

        // 1부터 mod-1까지 반복하여 역원에 해당하는지 확인
        for (int r=1; r<mod; r++){
            // x에 곱했을때 모드 연산의 결과가 1이되는 값이 역원이므로
            if ((r*x)%mod == 1)
                return r;
        }

        // 역원을 찾지 못한 경우
        return -1;
    }
}

// 타원곡선상의 두 점 더하기
Point EC_add(EC E, Point P, Point Q){
    // point of infinity는 덧셈의 항등원이므로,,, 
    // 항등원: 항상 'A + [] = [] + A = A'가 되도록 하는 수
    if (P.x == 0 && P.y == 0) return Q;
    if (Q.x == 0 && Q.y == 0) return P;

    // 결과를 저장할 포인트 구조체 생성
    Point R;

    //람다 구하기
    int lambda = ((Q.y - P.y) * mod_inv(Q.x - P.x, E.p));
    while (lambda < 0) lambda += E.p; // ->
    lambda %= E.p;
     
    // x
    R.x = lambda * lambda - P.x - Q.x;
    while (R.x < 0) R.x += E.p; // ->
    R.x %= E.p;

    // y
    R.y = lambda * (P.x - R.x) - P.y;
    while (R.y < 0) R.y += E.p; // ->
    R.y %= E.p;

    // -> 항상 모드연산하기 전에 음수를 양수로 만들기 위해 모드값을 더해주는것 잊지 않기

    return R;
}

// 타원곡선상의 한 점 더블링하기
Point EC_double(EC E, Point P){ 
    // 결과를 저장할 포인터 구조체
    Point R;

    // 람다
    int lambda = ((3*(P.x*P.x)+ E.a) * mod_inv(2*P.y, E.p));
    while (lambda < 0) lambda += E.p; // ->
    lambda %= E.p;

    // x
    R.x = ((lambda * lambda) - 2*P.x);
    while (R.x < 0) R.x += E.p; // ->
    R.x %= E.p;

    // y
    R.y = (lambda * (P.x - R.x) - P.y) % E.p;
    while (R.y < 0) R.y += E.p; // ->
    R.y %= E.p;

    // -> 항상 모드연산하기 전에 음수를 양수로 만들기 위해 모드값을 더해주는것 잊지 않기

    return R;
}

// 모드상의 스칼라곱
Point EC_mult(EC E, Point P, int r){
    // 결과를 저장할 포인터 구조체
    Point R;

    // 무한대점일경우 스칼라곱 불가능 -> (0,0) 반환
    if (P.x == 0 && P.y == 0) {
        R.x = 0; R.y = 0;
        return R;
    }
    // r이 0보다 작거나 같을 경우 에러-> 빈 구조체 반환
    if (r <= 0){
        cout << "error" << endl;
        return R;
    }
    
    // 타원곡선 상의 스칼라곱도, 모드상의 지수승 계산과 같이 이진수를 사용한다.

    // 이진수를 저장할 벡터
    vector<int> bits;
    // 곱할 수를 이진수로 벡터에 저장
    to_bits(bits, r);
    
    // 맨 처음은 무조건 1이므로 R = P
    R = P;

    for (int i = 1; i < bits.size(); i++){
        // 자리수 올리기
        R = EC_double(E, R);
        // 비트가 1일경우에는 P를 한번 더해줌
        if (bits.at(i) == 1)
            R = EC_add(E, R, P);
        
    }

    return R;
}

void to_bits(vector<int>& bits, int x){
    while (x > 0){
        bits.insert(bits.begin(), x % 2);
        x /= 2;
    }
}
