// StackWithTTP.cpp by Ulrich Eisenecker, May 20, 2022
// Warning: Incomplete program with fictitious example!

#include <vector>
#include <deque>
#include <list>

template<
   class T,
   template<
      class U = T,
      class Allocator = std::allocator<U>
   > class Container = std::deque
> class stack
{
   public:
      using UnderlyingContainerType = Container<>;
};

int main()
{
   stack<int>::UnderlyingContainerType s1;
   stack<int,std::deque>::UnderlyingContainerType s2;
   stack <int,std::vector>::UnderlyingContainerType s3;
   stack<int,std::list>::UnderlyingContainerType s4;
}
