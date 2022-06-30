#include "ec_helper.hpp"

TEST_CASE("toupper","[ec_helper]")
{
   using namespace std;
   using namespace ec_helper;
   // Testing empty string.
   REQUIRE(toupper("") == ""s);
   // Testing string with one none alphabet char.
   REQUIRE(toupper("1") == "1"s);
   // Testing string with one lowercase char.
   REQUIRE(toupper("z") == "Z"s);
   // Testing string with one uppercase char.
   REQUIRE(toupper("M") == "M"s);
   // Testing string with 10 lowercase chars.
   REQUIRE(toupper("abcdefghij") == "ABCDEFGHIJ"s);
   // Testing string with 10 uppercase chars.
   REQUIRE(toupper("ABCDEFGHIJ") == "ABCDEFGHIJ"s);
   // Testing string with 10 mixed chars."
   REQUIRE(toupper("ab.-EF12)!") == "AB.-EF12)!"s);
   // Testing string with german umlauts and sharp s
   // They MUST not be transformed.
   REQUIRE(toupper("äöüÄÖÜß") == "äöüÄÖÜß"s);
}

TEST_CASE("error","[ec_helper]")
{
   using namespace std;
   using namespace ec_helper;
   constexpr string_view sv { "TESTSTRING 123" };
   ostringstream os;
   string result { sv };
   SECTION("return")
   {
      REQUIRE(error(sv,os) == false);
   }
   SECTION("string_view")
   { 
      error(sv,os);
      result += '\n';
      REQUIRE(os.str() == result);
   }
   SECTION("string")
   {
      error(result,os);
      result += '\n';
      REQUIRE(os.str() == result);
   }
}
