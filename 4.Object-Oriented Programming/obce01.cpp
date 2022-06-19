#include <iostream>
using namespace std;

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
         m_mileage += remainingRange();
         return distance - remainingRange();
      }
   private:
      double m_maxBatteryCapacity, // kWh
             m_efficiency, // Wh/km
             m_maxSpeed, // km/h
             m_batteryCapacity, //kWh
             m_mileage; // km
};

int main()
{
   OBCE obce { 58.0,166.0,160.0 };
   cout << obce.charge(29.0) << endl;
   cout << obce.batteryCapacity() << endl;
   cout << obce.charge(50.0) << endl;
   cout << obce.batteryCapacity() << endl;
   cout << obce.remainingRange() << endl;
}
