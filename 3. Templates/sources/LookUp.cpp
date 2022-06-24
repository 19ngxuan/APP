// LookUp.cpp by Ulrich Eisenecker, May 20, 2022

#include <iostream>
#include <string>
using namespace std;

string message()
{
   return "Uh oh!"s;
}

class Surprise
{
   public:
      string message()
      {
         return "Ah!"s;
      }
};

template <class T>
class SomeTemplate: public T
{
   public:
      SomeTemplate()
      {
         cout << message() << endl;
         cout << T::message() << endl;
         cout << this->message() << endl;
      }
};

int main()
{
   SomeTemplate<Surprise> sts;
}
