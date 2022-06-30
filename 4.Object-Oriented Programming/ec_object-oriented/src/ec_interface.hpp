#ifndef EC_INTERFACE_HPP
#define EC_INTERFACE_HPP
#include <string>
#include <set>


namespace ec_interface
{
   class Interface
   {
      public:
         [[nodiscard]] virtual std::string 
            askQuestion(const std::string& text,
                        const std::set<std::string>& legalAnswers) const = 0;
         virtual void success(const std::string& text,
                              const std::string& value) const = 0;
         virtual void failed(const std::string& var,
                              const std::string& text) const = 0;
         virtual void lookingForVariable(const std::string& var) const = 0;
         virtual void variableAssigned(const std::string& var,
                                       const std::string& value) const = 0;
         virtual ~Interface() = default;
   };     
};

#endif // EC_INTERFACE_HPP
