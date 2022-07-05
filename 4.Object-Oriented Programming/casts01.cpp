#include <iostream>
#include <vector>
#include <memory>
using namespace std;

class NonPolymorphicBase
{
   public:
      void name() const
      {
         cout << "NonPolymorphicBase::name()" << endl;
      }
};

class NonPolymorphicChild: public NonPolymorphicBase
{
   public:
      void name() const
      {
         cout << "NonPolymorphicChild::name()" << endl;
      }
};

class NonPolymorphicSibling: public NonPolymorphicBase
{
   public:
      void name() const
      {
         cout << "NonPolymorphicSibling::name()" << endl;
      }
};

int main()
{
   vector<unique_ptr<NonPolymorphicBase>> object_manager;
   object_manager.push_back(make_unique<NonPolymorphicBase>());
   object_manager.push_back(make_unique<NonPolymorphicChild>());
   object_manager.push_back(make_unique<NonPolymorphicSibling>());
   for (const auto& o : object_manager)
   {
      o->name();
   }
}
