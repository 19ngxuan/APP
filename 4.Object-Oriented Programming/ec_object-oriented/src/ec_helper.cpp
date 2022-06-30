#include "ec_helper.hpp"

namespace ec_helper
{
   using namespace std;

   [[nodiscard]] string toupper(string s)
   {
      for (char& c : s)
         c = std::toupper(c); /*!< std:: required! */
      return s;
   }

   bool error(const std::string_view& msg,std::ostream& os)
   {
      os << msg 
         << endl;
      return false;
   }
}
