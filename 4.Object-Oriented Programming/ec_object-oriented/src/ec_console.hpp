#ifndef EC_CONSOLE_HPP
#define EC_CONSOLE_HPP

#include <string>
#include <set>
#include <iostream>
#include "ec_interface.hpp"

namespace ec_interface
{
   class Console: public ec_interface::Interface
   {
      public:
         Console(std::istream& is = std::cin,std::ostream& os = std::cout);
         [[nodiscard]] virtual std::string 
            askQuestion(const std::string& text,
                        const std::set<std::string>& legalAnswers) const override;
         virtual void success(const std::string& text,
                              const std::string& value) const override;
         virtual void failed(const std::string& var,
                             const std::string& text) const override;
         virtual void lookingForVariable(const std::string& var) const override;
         virtual void variableAssigned(const std::string& var,
                                       const std::string& value) const override;
      private:
         std::istream& m_is;
         std::ostream& m_os;
   };
}
#endif // EC_CONSOLE_HPP
