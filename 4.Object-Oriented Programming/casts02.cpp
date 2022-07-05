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
   vector<shared_ptr<NonPolymorphicBase>> object_manager;
   object_manager.push_back(make_shared<NonPolymorphicBase>());
   object_manager.push_back(make_shared<NonPolymorphicChild>());
   object_manager.push_back(make_shared<NonPolymorphicSibling>());
   for (const auto o : object_manager)
   {
      o->name();
   }
   static_cast<NonPolymorphicChild&>(*object_manager.at(1)).name(); // fine!
   static_cast<NonPolymorphicSibling&>(*object_manager.at(1)).name(); // oops!

//   dynamic_cast<NonPolymorphicChild&>(*object_manager.at(1)).name(); // fine!
//   dynamic_cast<NonPolymorphicSibling&>(*object_manager.at(1)).name(); // oops!

}
