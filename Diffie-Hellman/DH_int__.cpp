#include <iostream>
#include <vector>

using namespace std;

// 벡터에 x를 2진수로 저장하는 함수
void to_bits(vector<int>& bits, int x);
// base의 exp승에 모드연산을 수행하는 함수
int powm(int base, int exp, int mod);
// base가 mod의 원시근인지 확인하는 함수
bool is_primitive(int base, int mod);
// mod의 모든 원시근을 벡터에 저장하는 함수
void primitive_root(vector<int>& p_roots, int mod);

// 벡터 프린트함수(테스트 편하게 하기 위해 만듬)
void print_vector(vector<int>& vec){
    for (int&v : vec)
	    cout << v << " ";
    cout << endl;
}

int main(){
    // 공개키 p -> 모드 p상의 숫자를 이용할 것임
    int p = 41;
    // 공개키 p의 원시근들을 저장할 벡터
    vector<int> p_r;
    // 공개키 p의 원시근들을 저장
    primitive_root(p_r, p);

    // 5번째 원시근을 g값으로 사용
    int g = p_r.at(5);

    // Alice, Bob의 개인키
    int a_sk = 11; int b_sk = 9;

    // 각자 g에 개인키 승 모드연산을 해서 임시로 저장
    int a_temp = powm(g, a_sk, p);
    int b_temp = powm(g, b_sk, p);

    // 임시로 저장한 값을 서로 바꿔서, 한번더 자신의 개인키 승 모드연산을 함 => 같은 개인키를 공유하게 됨
    int a_pk = powm(b_temp, a_sk, p);
    int b_pk = powm(a_temp, b_sk, p);

    // 정말 같은 키를 공유하는지 확인
    if (a_pk == b_pk)
        cout << "a_pk == b_pk => success!" << endl;
    
    return 0;
}

void to_bits(vector<int>& bits, int x){
    /* 
        x를 받아 bits에 2진수로 저장한다.
        x를 2로 나누어 나오는 나머지를 차례대로 저장하고, x의 몫을 x에 저장해 다음 반복으로 넘긴다.
        제일 처음 나머지가 가장 오른쪽에 위치해야 하므로, 항상 벡터의 맨 앞에 새로운 값을 넣어준다.
        즉 11의 경우, bits에 [1, 0, 1, 1]이 저장되는 것과 같다.
    */
    while (x > 0){
        bits.insert(bits.begin(), x % 2);
        x /= 2;
    }
}
int powm(int base, int exp, int mod){
    /* 
        base^exp % mod 연산을 진행하는 함수
        exp가 클 경우에 직접 지수승 한 뒤에 연산을 하는것은 매우 오래걸리며 비효율적이다.
        따라서, 지수를 2진수로 표현하여 mod연산을 중간중간 하며 효율적으로 연산할 수 있다.
        2진수로 표현했을 때, 1일경우에는 base를 곱하고 모드연산을하고 자리수를 하나 올릴때에는 제곱을 한뒤 모드연산을 한다.
        0일 경우에는 자리수를 올리는 것만 하고 모드연산을 한다.

        만약 11 = 0x1011의 경우, (((1*base % mod)^2 * base % mod)^2 % mod)^2 * base % mod)
        -> 곱 모드 제곱, 곱 모드 제곱, 모드 제곱, 곱 모드 제곱
    */
    vector<int> bits; 
    to_bits(bits, exp);

    int result = base;
    for(int i=1; i < bits.size(); i++){
        result = (result * result) % mod;
        if (bits.at(i) == 1)
            result = (result * base) % mod;
    }

    return result;
}
bool is_primitive(int base, int mod){
    /*
        base % mod == base^n % mod 가 될 때까지 반복한다.
        모듈러 연산을 1승부터 +1씩 하면, mod-1개의 값들이 반복적으로 나타나며 한주기를 돌때까지 같은 값은 나오지 않는다.
        즉 base % mod == base^n % mod일 때,  n == mod라면 1부터 mod-1의 수가 모두 있다는 뜻이 된다.
        (모드상에서 나올 수 있는 수는 1~mod-1이고, base^1 % mod와 base^mod % mod가 같으므로 주기가 mod-1이다.)
        (원시근은 모드 p상에서 지수승 연산의 결과가 1~p-1의 값들이 주기적으로 나타나는 것이므로,,, 원시근 확인 가능)
    */
    int tmp = base; // 'base' is same as 'base % mod'
    int i = 1;
    do{
        tmp = (tmp * base) % mod;
        i++;
    }while(tmp != base);

    return i == mod;
}
void primitive_root(vector<int>& p_roots, int mod){
    /*
        2부터 mod까지 모든 수들에 대해 is_primitive함수로 원시근인지 판별함.
        원시근은 p_roots 벡터에 저장
    */
    for (int i = 2; i < mod; i++){
        if(is_primitive(i, mod))
            p_roots.push_back(i);
    }
}
