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
    while (x > 0){
        bits.insert(bits.begin(), x % 2);
        x /= 2;
    }
}
int powm(int base, int exp, int mod){
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
    int tmp = base; // 'base' is same as 'base % mod'
    int i = 1;
    do{
        tmp = (tmp * base) % mod;
        i++;
    }while(tmp != base);

    return i == mod;
}
void primitive_root(vector<int>& p_roots, int mod){
    for (int i = 2; i < mod; i++){
        if(is_primitive(i, mod))
            p_roots.push_back(i);
    }
}
