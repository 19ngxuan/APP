#include <iostream>
#include <bitset>
using namespace std;

const int& f(int& v)
{
   return v;
}


int main()
{
  //const int n { 2 };
  //f(n);
  int m { 2 };
  m = m + 2;
  cout << f(m) << endl;
  const_cast<int&>(f(m)) = 3;
  cout << m << endl;

  auto b { true };
  cout << reinterpret_cast<bitset<sizeof(b) * 8>&>(b) << endl;
}
