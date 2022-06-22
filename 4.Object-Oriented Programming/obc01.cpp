#include <iostream>
#include <string_view>
#define FMT_HEADER_ONLY
#include <fmt/format.h>
using namespace std;
using fmt::format;
using fmt::print;

class OBCE
{
   public:
      OBCE(double maxBatteryCapacity,
           double efficiency,
           double maxSpeed)
              :m_maxBatteryCapacity(maxBatteryCapacity),
               m_efficiency(efficiency),
               m_maxSpeed(maxSpeed),
               m_batteryCapacity(0.0),
               m_mileage(0.0)
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
      [[nodiscard]] const double& maxSpeed() const
      {
         return m_maxSpeed;
      }
      [[nodiscard]] const double& batteryCapacity() const
      {
         return m_batteryCapacity;
      }
      [[nodiscard]] const double& mileage() const
      {
         return m_mileage;
      }
      double charge(double amount) // returns unused power
      {
         m_batteryCapacity += amount;
         if (m_batteryCapacity > m_maxBatteryCapacity)
         {
            double difference  
                { amount - (m_batteryCapacity - m_maxBatteryCapacity) };
            m_batteryCapacity = m_maxBatteryCapacity;
            return amount - difference;
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
             m_maxSpeed, // km/h
             m_batteryCapacity, //kWh
             m_mileage; // km
};

string
   ch { "Charged {} of {} kWh.\n" },
   bc { "Battery capacity is {} kWh.\n" },
   rr { "Remaining range is {} km.\n" },
   dr { "Covered {} of {} km.\n" },
   mi { "The current mileage is {} km.\n" };



int main()
{
   OBCE obce { 58.0,166.0,160.0 };
   cout << format("Charged {} of {} kWh.\n",obce.charge(29.0),29.0); //this works
   cout << format(bc,obce.batteryCapacity());                        //this does not work
   cout << format(ch,obce.charge(50.0),50.0);
   cout << format(bc,obce.batteryCapacity());
   cout << format(rr,obce.remainingRange());
   cout << format(dr,obce.drive(175.0),175.0);
   cout << format(mi,obce.mileage());
   cout << format(bc,obce.batteryCapacity());
   cout << format(rr,obce.remainingRange());
   cout << format(dr,obce.drive(200.0),200);
   cout << format(mi, obce.mileage());
   cout << format(bc,obce.batteryCapacity());
   cout << format("Remaining range is {} km.\n",obce.remainingRange());
   

   return 0;
   
}