#ifndef EC_HELPER_HPP
#define EC_HELPER_HPP

#include <string>
#include <string_view>
#include <ostream>
#include <iostream> /*!< Because of std::cerr. */

namespace ec_helper
{
   [[nodiscard]] std::string toupper(std::string s);
   bool error(const std::string_view& msg,std::ostream& output = std::cerr);
}
#endif // EC_HELPER_HPP
