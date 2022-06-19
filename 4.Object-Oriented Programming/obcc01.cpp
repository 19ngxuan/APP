#include <iostream>
using namespace std;

class OBCC
{
   public:
      OBCC(double maxTankCapacity,
           double fuelConsumption,
           double maxSpeed)
              :m_maxTankCapacity(maxTankCapacity),
               m_fuelConsumption(fuelConsumption),
               m_maxSpeed(maxSpeed),
               m_tankLevel(0.0),
               m_mileage(0.0)
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
      [[nodiscard]] const double& maxSpeed() const
      {
         return m_maxSpeed;
      }
      [[nodiscard]] const double& tankLevel() const
      {
         return m_tankLevel;
      }
      [[nodiscard]] const double& mileage() const
      {
         return m_mileage;
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
      double maximumRange() const
      {
         return m_tankLevel * 100 / m_fuelConsumption;
      }
      double drive(double distance) // returns distance not covered
      {
         if (maximumRange() >= distance)
         {
            m_tankLevel -= distance * m_fuelConsumption / 100;
            m_mileage += distance;
            return 0.0;
         }
         m_mileage += maximumRange();
         return distance - maximumRange();
      }
   private:
      double m_maxTankCapacity, // kWh
             m_fuelConsumption, // Wh/km
             m_maxSpeed, // km/h
             m_tankLevel, //kWh
             m_mileage; // km
};

int main()
{
   OBCC obcc { 70.0,8.0,160.0 };
   cout << obcc.refuel(29.0) << endl;
   cout << obcc.tankLevel() << endl;
   cout << obcc.refuel(50.0) << endl;
   cout << obcc.tankLevel() << endl;
   cout << obcc.maximumRange() << endl;
}
