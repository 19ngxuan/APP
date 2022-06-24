// SwapMoreSpecialized.cpp by Ulrich Eisenecker, May 3, 2022

#include <iostream>
#include <string>
using namespace std;

template <typename T>
void swapVariables(T& a,T& b)
{
   T temp { a };
   a = b;
   b = temp;
}

template <typename T>
void prepareInput()
{}

template <>
void prepareInput<string>()
{
   if(cin.peek() == '\n')
   {
      cin.get();
   }
}

template <typename T> 
T inputValue()
{
   T t;
   cin >> t;
   return t;
}

template <>
string inputValue<string>()
{
   string t;
   getline(cin,t);
   return t;
}

template <typename T>
void demonstrateSwap(char t)
{
   T v1,v2;
   prepareInput<T>();
   cout << t << "1 = ";
   v1 = inputValue<T>();;
   cout << t << "2 = ";
   v2 = inputValue<T>();;
   swapVariables(v1,v2);
   cout << t << "1 = " << v1
        << "\n" << t << "2 = " << v2
        << endl;
}

int main()
{
   demonstrateSwap<int>('i');
   demonstrateSwap<double>('d');
   demonstrateSwap<string>('s');
}
