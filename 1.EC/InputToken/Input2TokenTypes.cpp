// Input2TokenTypes.cpp Ulrich Eisenecker, August 2,2021

#include <iostream>
#include <string>
using namespace std;

[[nodiscard]] string inputToken(istream& is) 
{
   string token;
   is >> ws; // remove leading whitespace
   if (is.peek() == '\"')
   {
      is.get(); // discard initial quote
      while (is && is.peek() != '\"')
      {
         token += is.get();
      }
      if (is && is.peek() ==  '\"')
      {
         is.get(); // discard closing quote
      }
   }
   else
   {
      while (is && is.peek() > 32 && is.peek() != '\"')
      {
         token += toupper(is.get());
      }
   };
   return token;
}

int main()
{
   string token { };
   while (cin)
   {
      token = inputToken(cin);
      if (cin)
      {
         cout << '[' << token << ']' << endl;
      }
   }
}
