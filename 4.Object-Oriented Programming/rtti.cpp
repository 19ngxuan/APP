#include <iostream>
#include <typeinfo>
using namespace std;

int main()
{
   int i; 
   signed int j;
   if (typeid(i) == typeid(j))
   {
      cout << "i and j are of the same type" << endl;
   }

   if (typeid(int) == typeid(unsigned int))
   {
      cout << "int and unsigned int are of the same type" << endl;
   }

}
