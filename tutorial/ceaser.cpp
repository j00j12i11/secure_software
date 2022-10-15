#include <iostream>
#include <string>

using namespace std;

void caesar_encode(string& msg, int key);
void caesar_decode(string& msg, int key);

int main(){
    string msg = "caesar's cipher test";
    int key = 5;

    cout << "message: " << msg << "\n" << endl;
    caesar_encode(msg, key);
    cout << "encoded text: " << msg << endl;
    caesar_decode(msg, key);
    cout << "decoded text: " << msg << endl;
    
    return 0;
}

void caesar_encode(string& msg, int key){
    for (auto& a:msg) a += key;
}

void caesar_decode(string& msg, int key){
    for (auto& a:msg) a -= key;
}