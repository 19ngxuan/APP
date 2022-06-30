#include <iostream>
#include <string_view>
#include <string>
#include <sstream>
#include <fmt/format.h>
#include <fmt/ostream.h>
using namespace std;
using fmt::format;
using fmt::print;

// Fully templatized version!

// Added VW_ID3_OBC as final class

template <typename ValType = double>
class OBC
{
   public:
      OBC() = default;
      OBC& operator=(const OBC&) = delete;
      // ~OBC() = delete; // don't do that!
      //OBC(const OBC&) = delete;
      using ValueType = ValType;
      OBC(ValueType maxSpeed)
              : m_maxSpeed { maxSpeed },
                m_mileage { 0.0 }
      {
      }
      [[nodiscard]] const ValueType& maxSpeed() const
      {
         return m_maxSpeed;
      }
      [[nodiscard]] const ValueType& mileage() const
      {
         return m_mileage;
      }
      virtual ValueType remainingRange() const = 0;
      virtual ValueType drive(ValueType distance) = 0; // returns distance not covered
      virtual ~OBC() // A base class MUST have a virtual destructor
      {
         cout << "OBC::~OBC" << endl;
      }
   protected:
      void  addMileage(ValueType distance)
      {
         m_mileage += distance;
      }
   private:
      ValueType m_maxSpeed, // km/h
                m_mileage; // km
};

template <typename ValType = double>
class OBCE: public OBC<ValType> 
{
   public:
   using ValueType = typename OBC<ValType>::ValueType;
      OBCE(ValueType maxBatteryCapacity,
           ValueType efficiency,
           ValueType maxSpeed)
              :OBC<ValType> { maxSpeed },
               m_maxBatteryCapacity { maxBatteryCapacity },
               m_efficiency { efficiency },
               m_batteryCapacity { 0.0 }
      {
      }
      [[nodiscard]] const ValueType& maxBatteryCapacity() const
      {
         return m_maxBatteryCapacity;
      }
      [[nodiscard]] const ValueType& efficiency() const
      {
         return m_efficiency;
      }
      [[nodiscard]] const ValueType& batteryCapacity() const
      {
         return m_batteryCapacity;
      }
      ValueType charge(ValueType amount) // return unused power
      {
         m_batteryCapacity += amount;
         if (m_batteryCapacity > m_maxBatteryCapacity)
         {
            ValueType difference { m_batteryCapacity - m_maxBatteryCapacity };
            m_batteryCapacity = m_maxBatteryCapacity;
            return difference;
         }
         return 0.0;
      }
      ValueType remainingRange() const override
      {
         return m_batteryCapacity * 1000.0 / m_efficiency;
      }
      ValueType drive(ValueType distance) override // returns distance not covered
      {
         if (remainingRange() >= distance)
         {
            m_batteryCapacity -= distance * m_efficiency / 1000.0;
            this->addMileage(distance);
            return 0.0;
         }
         ValueType coveredDistance { remainingRange() };
         m_batteryCapacity = 0.0;
         this->addMileage(coveredDistance);
         return distance - coveredDistance;
      }
      ~OBCE()
      {
         cout << "OBCE::~OBCE" << endl;
      }
   private:
      ValueType m_maxBatteryCapacity, // kWh
                m_efficiency, // Wh/km
                m_batteryCapacity; // kWh
};

template <class T>
T extract(const string& s,int no)
{
   istringstream is { s };
   T temp;
   for (int n { 0 }; n <= no; ++n)
   { 
      is >> temp;
   }
   return temp;
}

template <typename ValType = double>
class OBCC: public OBC<ValType>
{
   public:
      using ValueType = typename OBC<ValType>::ValueType;
      OBCC(ValueType maxTankCapacity,
           ValueType fuelConsumption,
           ValueType maxSpeed)
              :OBC<ValType> { maxSpeed },
               m_maxTankCapacity { maxTankCapacity },
               m_fuelConsumption { fuelConsumption },
               m_tankLevel { 0.0 }
      {
      }
      OBCC()
      {
         cout << "OBCC::OBCC()" << endl;
      }
/*      explicit OBCC(const std::string& init)
         :OBC { extract<ValueType>(init,2) },
          m_maxTankCapacity { extract<ValueType>(init,0) },
          m_fuelConsumption { extract<ValueType>(init,1) },
          m_tankLevel { 0.0 }
      {
      }*/
      explicit OBCC(const std::string& init) // delegating constructor
         :OBCC { extract<ValueType>(init,2),
                 extract<ValueType>(init,0),
                 extract<ValueType>(init,1)
               }      
      {
      }    
      [[nodiscard]] const ValueType& maxTankCapacity() const
      {
         return m_maxTankCapacity;
      }
      [[nodiscard]] const ValueType& fuelConsumption() const
      {
         return m_fuelConsumption;
      }
      [[nodiscard]] const ValueType& tankLevel() const
      {
         return m_tankLevel;
      }
      ValueType refuel(ValueType amount) // returns unused power
      {
         m_tankLevel += amount;
         if (m_tankLevel > m_maxTankCapacity)
         {
            ValueType difference  
                { amount - (m_tankLevel - m_maxTankCapacity) };
            m_tankLevel = m_maxTankCapacity;
            return amount - difference;
         }
         return 0.0;
      }
      ValueType remainingRange() const override
      {
         return m_tankLevel * 100 / m_fuelConsumption;
      }
      ValueType drive(ValueType distance) override // returns distance not covered
      {
         if (remainingRange() >= distance)
         {
            m_tankLevel -= distance * m_fuelConsumption / 100;
            this->addMileage(distance);
            return 0.0;
         }
         this->addMileage(remainingRange());
         return distance - remainingRange();
      }
      ~OBCC()
      {
         cout << "OBCC::~OBCC" << endl;
      }
   private:
      ValueType m_maxTankCapacity, // kWh
                m_fuelConsumption, // Wh/km
                m_tankLevel; //kWh
};

constexpr fmt::string_view
   ch { "Charged {} of {} kWh.\n" },
   bc { "Battery capacity is {} kWh.\n" },
   rr { "Remaining range is {} km.\n" },
   dr { "Covered {} of {} km.\n" },
   mi { "The current mileage is {} km.\n" },
   ms { "The maximum speed is {} km/h.\n" },
   fc { "The fuel consumption is {} l/100 km.\n" },
   mt { "The tank volume is {} l.\n" };

template <class T>
void testOBC(OBC<T>& obc)
{
   print(rr,obc.remainingRange());
   print(dr,obc.drive(200.0),200);
   print(mi,obc.mileage());   
}

template <class T>
void testOBC(OBC<T>* obc) 
{
   print(rr,obc->remainingRange());// (*obc).remainingRange();
   print(dr,obc->drive(200.0),200);
   print(mi,obc->mileage());   
}

template <class T>
void testOBCC(OBCC<T> anOBCC)
{
   print(ms,anOBCC.maxSpeed());
   print(fc,anOBCC.fuelConsumption());
   print(mt,anOBCC.maxTankCapacity());
}

template <class T>
void testOBCC(const std::string& s) // calling function with string again
                                    // by applying overloading polymorphism
{
   OBCC<T> obcc(s);
   testOBCC(obcc);
}

class VW_ID3_OBC final: public OBCE<double>
{
   public:
      VW_ID3_OBC():OBCE<double> { 82.0 , 139.0, 160.0 }
      {
         cout << "VW_ID3_OBC::VW_ID3_OBC()" << endl;
      }
};

int main()
{
   OBCE<> obce { 58.0,166.0,160.0 };
   print(ch,obce.charge(29.0),29.0);
   print(bc,obce.batteryCapacity());
   print(ch,obce.charge(50.0),50.0);
   print(bc,obce.batteryCapacity());
   print(rr,obce.remainingRange());
   print(dr,obce.drive(175.0),175.0);
   print(mi,obce.mileage());
   print(bc,obce.batteryCapacity());
   print(rr,obce.remainingRange());
   print(dr,obce.drive(200.0),200);
   print(mi,obce.mileage());
   print(bc,obce.batteryCapacity());
   print(rr,obce.remainingRange());

   OBCC<> obcc { 70.0,8.0,160.0 };
   cout << obcc.refuel(29.0) << endl;
   cout << obcc.tankLevel() << endl;
   cout << obcc.refuel(50.0) << endl;
   cout << obcc.tankLevel() << endl;
   cout << obcc.remainingRange() << endl;

   // OBC<> obc(99.0); // illegal, because OBC ist abstract
   testOBC(obce);
   testOBC(obcc);

   testOBC(&obce);
   testOBC(&obcc);

   // Terrible mess if OBC destructor is not virtual!

   OBC<>* someOBC { new OBCE<> { 58.0,166.0,160.0 } };
   delete someOBC;

   OBCC<> notReallyGood { }; // explicitly calling standard constructor
   //notReallyGood = obcc;
   //notReallyGood.operator=(obcc);
   //OBCC anotherOne { obcc };
   OBCC<> newOBCC("40.0 5.5 160.0"); // tank volume, fuel consumpt., max speed
   testOBCC(newOBCC);
   testOBCC<double>("41.0 5.6 161.0"s);

    VW_ID3_OBC vwid3;
    testOBC(vwid3);
}
