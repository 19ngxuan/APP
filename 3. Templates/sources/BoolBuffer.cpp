// BoolBuffer.cpp by Ulrich Eisenecker, May 12, 2022

#include <iostream>
using namespace std;

template <typename T,int CAPACITY>
class RingBuffer
{
   public:
      void put(const T& element)
      {
         if (m_noOfElements < CAPACITY)
         { 
            ++m_noOfElements;
         }
         m_elements[m_writeIndex++] = element;
         m_writeIndex %= CAPACITY;
      }
      T get()
      {
         // The function must not be called if m_noOfElements == 0!
         int readIndex { (CAPACITY - m_noOfElements 
                          + m_writeIndex) % CAPACITY };
         --m_noOfElements;
         return m_elements[readIndex];
      }
   private:
      int m_writeIndex { 0 },
          m_noOfElements { 0 };
      T m_elements[CAPACITY];
};

template <int CAPACITY>
class RingBuffer<bool,CAPACITY>
{
   public:
      void put(bool element)
      {
         m_elements <<= 1;
         if (element)
         {
            m_elements |= 1;
         }
         if (m_readIndex < CAPACITY)
         {
           ++m_readIndex;
         }
      }
      bool get()
      {
         bool result { false };
         if (m_readIndex > 0)
         {
            result = m_elements & (1 << --m_readIndex);
         }
         return result;
      }
   private:
      static_assert(CAPACITY <= sizeof(unsigned long long) * 8);
      int m_readIndex { 0 };
      unsigned long long m_elements { 0 };
};

int main()
{
   RingBuffer<bool,6> bb;
   cout << bb.get() << endl;
   bb.put(true);
   bb.put(false);
   bb.put(true);
   bb.put(false);
   bb.put(false);
   bb.put(true);
   cout << bb.get() << endl;
   cout << bb.get() << endl;
   cout << bb.get() << endl;
   cout << bb.get() << endl;
   cout << bb.get() << endl;
   cout << bb.get() << endl;
}
