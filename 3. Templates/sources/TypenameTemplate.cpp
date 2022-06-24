// TypenameTemplate.cpp by Ulrich Eisenecker, May 20, 2022

#include <iostream>
using namespace std;

class SomeClass
{
   public:
      static inline const int A { 42 };
      using B = int;
      template <int n>
      class C
      {
         public:
            static inline const int D { n };
            using E = double;
      };
};

template <class T>
class ClassTemplate
{
   public:
      ClassTemplate()
      {
         cout << T::A << endl;
         typename T::B someValue { 99 };
         cout << someValue << endl;
         typename T:: template C<1> someClass;
         cout << T::template C<0>::D << endl;
         typename T:: template C<-1>::E anotherValue { 22.0 / 7.0 };
         cout << anotherValue << endl;
      }
};

int main()
{
   ClassTemplate<SomeClass> ct;
}
