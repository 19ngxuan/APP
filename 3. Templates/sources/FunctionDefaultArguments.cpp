#include <iostream>
using namespace std;

template <int n = 5,class T = bool> 
void f(T t = T())
{
   cout << n << t << endl;
};

int main()
{
   f<1>(6);
   f(false);
   f(19.9);
   f<6>(20.4);
}
