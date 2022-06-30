#include "ec_console.hpp"
#include <string>
#include <sstream>
#include <set>

TEST_CASE("ec_interface","[Console]")
{
   using namespace std;
   using namespace ec_interface;

   istringstream input;
   ostringstream output;
   Console con(input,output);
   SECTION("askQuestion")
   {
      input.str("yes\nno\n");
      set<string> la;
      la.insert("YES");
      la.insert("NO");
      REQUIRE(con.askQuestion("A",la) == "YES");
      REQUIRE(con.askQuestion("A",la) == "NO");
      REQUIRE(output.str() == "A\nA\n");
   }
   SECTION("success")
   {
      con.success("A","B");
      REQUIRE(output.str() == "AB\n\n");
   }
   SECTION("failed")
   {
      con.failed("A","B");
      REQUIRE(output.str() == 
         "Error in Knowledge Base.\n"
         "A searched for but not found.\nBUNKNOWN\n\n");
   }
   SECTION("lookingForVariable")
   {
      con.lookingForVariable("A");
      REQUIRE(output.str() == "Currently looking for: A.\n");
   }
   SECTION("variableAssigned")
   {
      con.variableAssigned("A","B");
      REQUIRE(output.str() == "It has been learned that A\nis B.\n");
   }
}
