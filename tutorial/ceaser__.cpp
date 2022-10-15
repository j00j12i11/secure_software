#include <iostream>
#include <string>

using namespace std;

// 메시지를 암호문으로 바꿔주는 함수
void caesar_encode(string& msg, int key);
// 암호문을 원본 메시지로 바꿔주는 함수
void caesar_decode(string& msg, int key);

int main(){
    // 원본 메시지
    string msg = "caesar's cipher test";
    // 키 값
    int key = 5;

    // 원본 메시지 출력
    cout << "message: " << msg << "\n" << endl;

    // 원본 메시지를 암호문으로 변경
    caesar_encode(msg, key);

    // 암호문 출력
    cout << "encoded text: " << msg << endl;

    // 암호문을 원본 메시지로 복호화
    caesar_decode(msg, key);

    // 복호화한 메시지 출력
    cout << "decoded text: " << msg << endl;
    
    return 0;
}

void caesar_encode(string& msg, int key){
    /* 
        원본 메시지의 각 단어에 키 값을 더한다.
        - 문자는 유니코드(숫자)의 형태로 저장되고, 유니코드는 알파벳 순서대로 매핑되어 있다.
        - a = 61, b = 62, c = 63 ... -> a(61) + 5 = f(66)
    */
    // 배열 속에 있는 모든 원소들에 대해 반복하도록 하는 반복문(배열 = 문자열, 원소 = 문자)
    for (char& a:msg) a += key;
}

void caesar_decode(string& msg, int key){
     /* 
        원본 메시지의 각 단어에 키 값을 뺀다.
        - 암호화때 키 값만큼 더했으므로, 동일한 값을 빼 원래의 메시지를 얻을 수 있다.
    */
    // 배열 속에 있는 모든 원소들에 대해 반복하도록 하는 반복문(배열 = 문자열, 원소 = 문자)
    for (char& a:msg) a -= key; 
}