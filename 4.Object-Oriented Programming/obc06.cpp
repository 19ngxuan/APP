#include <iostream>
#include <string_view>
#include <fmt/format.h>
using namespace std;
using fmt::format;

class OBC
{
   public:
      OBC(double maxSpeed)
              : m_maxSpeed(maxSpeed),
                m_mileage(0.0)
      {
      }
      [[nodiscard]] const double& maxSpeed() const
      {
         return m_maxSpeed;
      }
      [[nodiscard]] const double& mileage() const
      {
         return m_mileage;
      }
      virtual double remainingRange() const = 0;
      virtual double drive(double distance) = 0;// returns distance not covered
   protected:
      double m_maxSpeed, // km/h
             m_mileage; // km
};

class OBCE: public OBC
{
   public:
      OBCE(double maxBatteryCapacity,
           double efficiency,
           double maxSpeed)
              :OBC(maxSpeed),
               m_maxBatteryCapacity(maxBatteryCapacity),
               m_efficiency(efficiency),
               m_batteryCapacity(0.0)
      {
      }
      [[nodiscard]] const double& maxBatteryCapacity() const
      {
         return m_maxBatteryCapacity;
      }
      [[nodiscard]] const double& efficiency() const
      {
         return m_efficiency;
      }
      [[nodiscard]] const double& batteryCapacity() const
      {
         return m_batteryCapacity;
      }
      double charge(double amount) // return unused power
      {
         m_batteryCapacity += amount;
         if (m_batteryCapacity > m_maxBatteryCapacity)
         {
            double difference { m_batteryCapacity - m_maxBatteryCapacity };
            m_batteryCapacity = m_maxBatteryCapacity;
            return difference;
         }
         return 0.0;
      }
      double remainingRange() const
      {
         return m_batteryCapacity * 1000.0 / m_efficiency;
      }
      double drive(double distance) // returns distance not covered
      {
         if (remainingRange() >= distance)
         {
            m_batteryCapacity -= distance * m_efficiency / 1000.0;
            m_mileage += distance;
            return 0.0;
         }
         double coveredDistance { remainingRange() };
         m_batteryCapacity = 0.0;
         m_mileage += coveredDistance;
         return distance - coveredDistance;
      }
   private:
      double m_maxBatteryCapacity, // kWh
             m_efficiency, // Wh/km
             m_batteryCapacity; // kWh
};

class OBCC: public OBC
{
   public:
      OBCC(double maxTankCapacity,
           double fuelConsumption,
           double maxSpeed)
              :OBC(maxSpeed),
               m_maxTankCapacity(maxTankCapacity),
               m_fuelConsumption(fuelConsumption),
               m_tankLevel(0.0)
      {
      }
      [[nodiscard]] const double& maxTankCapacity() const
      {
         return m_maxTankCapacity;
      }
      [[nodiscard]] const double& fuelConsumption() const
      {
         return m_fuelConsumption;
      }
      [[nodiscard]] const double& tankLevel() const
      {
         return m_tankLevel;
      }
      double refuel(double amount) // returns unused power
      {
         m_tankLevel += amount;
         if (m_tankLevel > m_maxTankCapacity)
         {
            double difference  
                { amount - (m_tankLevel - m_maxTankCapacity) };
            m_tankLevel = m_maxTankCapacity;
            return amount - difference;
         }
         return 0.0;
      }
      double remainingRange() const
      {
         return m_tankLevel * 100 / m_fuelConsumption;
      }
      double drive(double distance) // returns distance not covered
      {
         if (remainingRange() >= distance)
         {
            m_tankLevel -= distance * m_fuelConsumption / 100;
            m_mileage += distance;
            return 0.0;
         }
         m_mileage += remainingRange();
         return distance - remainingRange();
      }
   private:
      double m_maxTankCapacity, // kWh
             m_fuelConsumption, // Wh/km
             m_tankLevel; //kWh
};

constexpr string_view
   ch { "Charged {} of {} kWh.\n" },
   bc { "Battery capacity is {} kWh.\n" },
   rr { "Remaining range is {} km.\n" },
   dr { "Covered {} of {} km.\n" },
   mi { "The current mileage is {} km.\n" };

void testOBC(OBC& obc)
{
   cout << format(rr,obc.remainingRange());
   cout << format(dr,obc.drive(200.0),200);
   cout << format(mi,obc.mileage());   
}

void testOBC(OBC* obc)
{
   cout << format(rr,obc->remainingRange());// (*obc).remainingRange();
   cout << format(dr,obc->drive(200.0),200);
   cout << format(mi,obc->mileage());   
}

int main()
{
   OBCE obce { 58.0,166.0,160.0 };
   cout << format(ch,obce.charge(29.0),29.0);
   cout << format(bc,obce.batteryCapacity());
   cout << format(ch,obce.charge(50.0),50.0);
   cout << format(bc,obce.batteryCapacity());
   cout << format(rr,obce.remainingRange());
   cout << format(dr,obce.drive(175.0),175.0);
   cout << format(mi,obce.mileage());
   cout << format(bc,obce.batteryCapacity());
   cout << format(rr,obce.remainingRange());
   cout << format(dr,obce.drive(200.0),200);
   cout << format(mi,obce.mileage());
   cout << format(bc,obce.batteryCapacity());
   cout << format(rr,obce.remainingRange());

   OBCC obcc { 70.0,8.0,160.0 };
   cout << obcc.refuel(29.0) << endl;
   cout << obcc.tankLevel() << endl;
   cout << obcc.refuel(50.0) << endl;
   cout << obcc.tankLevel() << endl;
   cout << obcc.remainingRange() << endl;

   // OBC obc(99.0); // illegal, because OBC ist abstract
   testOBC(obce);
   testOBC(obcc);

   testOBC(&obce);
   testOBC(&obcc);
}
