#include<iostream>
#include<cstdio>
#include<cstdlib>
using namespace std;
long long n1, n2, n3, R;

long long mabs (long long a){
    if (a < 0) return -a;
    return a;
}

long long minn (long long a, long long b){
    if (a > b) return b;
    return a;
}

long long case2 (long long k){
    return k*(2*R-k+1)/2;
}

long long case3 (long long k, long long n){
    long long suma = 0;

    for (long long i=1; i<=k; i++)
        suma += minn (n, R-i);

    return suma;
}

int main (){
    long long range;
    scanf("%lld %lld %lld %lld",&n1, &n2, &n3, &range);

    long long ans = 0;
    for (R = range; R>=1; R--){
    ans += (R+1)*(R+2)/2;
    long long k1 = minn (n1, R), k2 = minn(n2, R), k3 = minn(n3,R);

    ans += case2(k1);
    ans += case2(k2);
    ans += case2(k3);

    ans += case3(k1, n2);
    ans += case3(k2, n3);
    ans += case3(k3, n1);

    long long Min_N = minn (n2, n3), Max_N = n2+n3-Min_N;

    for (long long i=0; i<=minn(n1-1,R); i++){
        long long S = n1+n2+n3-R-i;
        if (S < 0) break;

        long long alpha = S-(Max_N-1);
        if (alpha >= Min_N) continue;
        if (alpha < 0) alpha = 0;

        ans += minn (S+1, Min_N-alpha);
    }
    if (ans < 0) cout << "ops\n";
    }

    printf ("%lld\n",ans);
    return 0;
}
