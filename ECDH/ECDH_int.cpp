#include <iostream>
#include <gmpxx.h>
#include <vector>

using namespace std;

struct Point {
    int x;
    int y;
};

struct EC {
    int a;
    int b;
    int p;
};

int mod_inv(int x, int mod);
Point EC_add(EC E, Point P, Point Q);
Point EC_double(EC E, Point P);
Point EC_mult(EC E, Point P, int r);

void to_bits(vector<int>& bits, int x);
int powm(int base, int exp, int mod);

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

int mod_inv(int x, int mod){
    if (mod <= 0) return -1;
    if (x == 0) return 0;
    else{
        while (x < 0){
            x += mod;
        }

        for (int r=0; r<mod; r++){
            if ((r*x)%mod == 1)
                return r;
        }

        return -1;
    }
}
Point EC_add(EC E, Point P, Point Q){
    if (P.x == 0 && P.y == 0) return Q;
    if (Q.x == 0 && Q.y == 0) return P;

    Point R;

    int lambda = ((Q.y - P.y) * mod_inv(Q.x - P.x, E.p));
    while (lambda < 0) lambda += E.p;
    lambda %= E.p;

    R.x = lambda * lambda - P.x - Q.x;
    while (R.x < 0) R.x += E.p;
    R.x %= E.p;
    
    R.y = lambda * (P.x - R.x) - P.y;
    while (R.y < 0) R.y += E.p;
    R.y %= E.p;

    return R;
}
Point EC_double(EC E, Point P){ 
    Point R;

    int lambda = ((3*(P.x*P.x)+ E.a) * mod_inv(2*P.y, E.p));
    while (lambda < 0) lambda += E.p;
    lambda %= E.p;

    R.x = ((lambda * lambda) - 2*P.x);
    while (R.x < 0) R.x += E.p;
    R.x %= E.p;

    R.y = (lambda * (P.x - R.x) - P.y);
    while (R.y < 0) R.y += E.p;
    R.y %= E.p;

    return R;
}
Point EC_mult(EC E, Point P, int r){
    Point R;

    if (P.x == 0 && P.y == 0) {
        R.x = 0; R.y = 0;
        return R;
    }
    if (r <= 0){
        cout << "error" << endl;
        return R;
    }
    
    vector<int> bits;
    to_bits(bits, r);
    
    R = P;

    for (int i = 1; i < bits.size(); i++){
        R = EC_double(E, R);
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