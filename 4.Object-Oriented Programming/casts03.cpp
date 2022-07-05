#include <iostream>
#include <vector>
#include <memory>
#include <typeinfo>
using namespace std;

class PolymorphicBase
{
   public:
      virtual void name() const
      {
         cout << "PolymorphicBase::name()" << endl;
      }
      virtual ~PolymorphicBase() = default;
};

class PolymorphicChild: public PolymorphicBase
{
   public:
      void name() const
      {
         cout << "PolymorphicChild::name()" << endl;
      }
};

class PolymorphicSibling: public PolymorphicBase
{
   public:
      void name() const
      {
         cout << "PolymorphicSibling::name()" << endl;
      }
};

int main()
{
   vector<shared_ptr<PolymorphicBase>> object_manager;
   object_manager.push_back(make_shared<PolymorphicBase>());
   object_manager.push_back(make_shared<PolymorphicChild>());
   object_manager.push_back(make_shared<PolymorphicSibling>());
   for (const auto o : object_manager)
   {
      o->name();
   }
   static_cast<PolymorphicChild&>(*object_manager.at(1)).name(); // fine!
   static_cast<PolymorphicSibling&>(*object_manager.at(1)).name(); // oops!

   dynamic_cast<PolymorphicChild&>(*object_manager.at(1)).name(); // fine!
   dynamic_cast<PolymorphicSibling&>(*object_manager.at(2)).name(); // fine!
   dynamic_cast<PolymorphicSibling&>(*object_manager.at(1)).name(); // fine!
}
