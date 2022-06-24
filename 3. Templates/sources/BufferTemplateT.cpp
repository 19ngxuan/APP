//BufferTemplateT.cpp by Ulrich Eisenecker, May 10, 2022

#include <iostream>
using namespace std;

template <typename T>
class RingBuffer
{
   public:
      void put(const T& element)
      {
         if (m_noOfElements < 5)
         { 
            ++m_noOfElements;
         }
         m_elements[m_writeIndex++] = element;
         m_writeIndex %= 5;
      }
      T get()
      {
         // The function must not be called if m_noOfElements == 0!
         int readIndex { (5 - m_noOfElements + m_writeIndex) % 5 };
         --m_noOfElements;
         return m_elements[readIndex];
      }
   private:
      int m_writeIndex { 0 },
          m_noOfElements { 0 };
      T m_elements[5];
};

int main()
{
   RingBuffer<char> rb;
   rb.put('A');
   rb.put('B');
   rb.put('C');
   rb.put('D');
   rb.put('E');
   rb.put('F');
   cout << rb.get() << endl;
   cout << rb.get() << endl;
   cout << rb.get() << endl;
   cout << rb.get() << endl;
   cout << rb.get() << endl;
   cout << rb.get() << endl;
   cout << rb.get() << endl;
   cout << rb.get() << endl;
   cout << rb.get() << endl;
   cout << rb.get() << endl;
   cout << rb.get() << endl;
}
