#include <iostream>
#include <string>
using namespace std;

class DemoMCT
{
   public:
      template <typename T,int n>
      class SimpleArray
      {
         public:
            T m_elements[n];
            const T* begin() const
            {
               return m_elements;
            }
            const T* end() const
            {
               return m_elements + n;
            }
      };
};

int main()
{
   DemoMCT::SimpleArray<int,3> array_int_3 { 2, 3, 5 };
   DemoMCT::SimpleArray<char,5> array_char_5 { 'h','a','l','l','o' };
   for (auto i : array_int_3)
   {
      cout << i << endl;
   }
   for (auto c : array_char_5)
   {
      cout << c;
   }
   cout << endl;
}
