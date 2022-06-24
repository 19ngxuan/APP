#include <iostream>
#include <string>
using namespace std;

class DemoMFT
{
   public:
      DemoMFT(const string& s):m_name(s)
      {
      }
      template <typename T>
      void mft(T t)
      {
         cout << "m_name: " << m_name
              << ", t: " << t << endl;
      }
   private:
      string m_name;
};

template <>
void DemoMFT::mft<int>(int i)
{
   cout << "Explicit full specialization\n"
        << "m_name: " << m_name
        << ", t: " << i << endl;
}

int main()
{
   DemoMFT o { "Alex" };
   o.mft(12.4);
   o.mft(true);
   o.mft(" is a first name");
   o.mft(42);
}
