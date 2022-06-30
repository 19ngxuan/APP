#include "ec_tokenizer.hpp"
#include <sstream>
#include <stdexcept> /*!< Because of std::logic_error. */
#include <tuple> /*!< Because of std::tie(). */

TEST_CASE("Token","[ec_tokenizer]")
{
   using namespace ec_tokenizer;
   const Token t(TokenType::STRING,"Test",{ 1,2 },{ 3,4 });
   REQUIRE(t.type()  == TokenType::STRING);
   REQUIRE(t.value() == "TEST");
   REQUIRE(t.begin().first  == 1);
   REQUIRE(t.begin().second == 2);
   REQUIRE(t.end().first    == 3);
   REQUIRE(t.end().second   == 4);
}

TEST_CASE("Tokenizer","[ec_tokenizer]")
{
   using namespace std;
   using namespace ec_tokenizer;
   istringstream is;
   Tokenizer tokenizer(is);
   SECTION("POSITION")
   {
      is.str("1-1_1-7\n 2-2_2-8\n\" 1-3 3-11\"\n\"4-1 \n5-4\"");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::STRING);
      REQUIRE(token.value() == "1-1_1-7");
      REQUIRE(token.begin().first == 1);
      REQUIRE(token.begin().second == 1);
      REQUIRE(token.end().first == 1);
      REQUIRE(token.end().second == 7);
      token = tokenizer.next();
      REQUIRE(token.type() == TokenType::STRING);
      REQUIRE(token.value() == "2-2_2-8");
      REQUIRE(token.begin().first == 2);
      REQUIRE(token.begin().second == 2);
      REQUIRE(token.end().first == 2);
      REQUIRE(token.end().second == 8);
      token = tokenizer.next();
      REQUIRE(token.type() == TokenType::QUOTED_STRING);
      REQUIRE(token.value() == " 1-3 3-11");
      REQUIRE(token.begin().first == 3);
      REQUIRE(token.begin().second == 1);
      REQUIRE(token.end().first == 3);
      REQUIRE(token.end().second == 11);
      token = tokenizer.next();
      REQUIRE(token.type() == TokenType::QUOTED_STRING);
      REQUIRE(token.value() == "4-1 \n5-4");
      REQUIRE(token.begin().first == 4);
      REQUIRE(token.begin().second == 1);
      REQUIRE(token.end().first == 5);
      REQUIRE(token.end().second == 4);
   }
   SECTION("EMPTY") // Empty input stream.
   {
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::EMPTY);
      REQUIRE_THROWS_AS(token.value(),logic_error);
      REQUIRE_THROWS_WITH(token.value(),
         "Token::value() MUST not be called if m_tokenType == TokenType::EMPTY.");
      REQUIRE_THROWS_AS(token.begin(),logic_error);
      REQUIRE_THROWS_WITH(token.begin(),
         "Token::begin() MUST not be called if m_tokenType == TokenType::EMPTY.");
      REQUIRE_THROWS_AS(token.end(),logic_error);
      REQUIRE_THROWS_WITH(token.end(),
         "Token::end() MUST not be called if m_tokenType == TokenType::EMPTY.");
   }
   SECTION("whitespaceEMPTY") // Input stream with whitespace only.
   {
      is.str("   ");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::EMPTY);
   }
   SECTION("STRING")
   {
      is.str("string");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::STRING);
      REQUIRE(token.value() == "STRING");
      SECTION("begin")
      {
         auto [ x, y ] = token.begin(); /*!< Structured binding declaration */
         REQUIRE(x == 1);
         REQUIRE(y == 1);
      }
      SECTION("end")
      {
         auto [ x,y ] = token.end();
         REQUIRE(x == 1);
         REQUIRE(y == 6);
      }
   }
   SECTION("whitespaceSTRING")
   {
      is.str("   string");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::STRING);
      REQUIRE(token.value() == "STRING");
   }
   SECTION("STRINGwhitespace")
   {
      is.str("string   ");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::STRING);
      REQUIRE(token.value() == "STRING");
   }
   SECTION("whitespaceSTRINGwhitespace")
   {
      is.str("string   ");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::STRING);
      REQUIRE(token.value() == "STRING");
   }
   SECTION("QUOTED_STRING")
   {
      is.str("\" string in double quotes. \"");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::QUOTED_STRING);
      REQUIRE(token.value() == " string in double quotes. ");
   }
   SECTION("whitespaceQUOTED_STRING")
   {
      is.str("\t \t\" string in double quotes. \"");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::QUOTED_STRING);
      REQUIRE(token.value() == " string in double quotes. ");
   }
   SECTION("QUOTED_STRINGwhitespace")
   {
      is.str("\" string in double quotes. \"\t \t\"");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::QUOTED_STRING);
      REQUIRE(token.value() == " string in double quotes. ");
   }
   SECTION("whitespaceQUOTED_STRINGwhitespace")
   {
      is.str("\" string in double quotes. \"\t \t\"");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::QUOTED_STRING);
      REQUIRE(token.value() == " string in double quotes. ");
   }
   SECTION("QUOTED_STRINGmultiline")
   {
      is.str("   \"\n2-4\n\n   3-7\"");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::QUOTED_STRING);
      REQUIRE(token.value() == "\n2-4\n\n   3-7");
      SECTION("begin")
      {
         auto [ x,y ] = token.begin();
         REQUIRE(x == 1);
         REQUIRE(y == 4);
      }
      SECTION("end")
      {
         auto [ x,y ] = token.end();
         REQUIRE(x == 4);
         REQUIRE(y == 7);
      }
   }
   SECTION("INCOMPLETE_QUOTED_STRING")
   {
      is.str("\"");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::INCOMPLETE_QUOTED_STRING);
      REQUIRE(token.value() == "");
      SECTION("begin")
      {
         auto [ x,y ] = token.begin();
         REQUIRE(x == 1);
         REQUIRE(y == 1);
      }
      SECTION("end")
      {
         auto [ x,y ] = token.end();
         REQUIRE(x == 1);
         REQUIRE(y == 1);
      }
   }
   SECTION("whitespaceINCOMPLETE_QUOTED_STRING")
   {
      is.str("\t \n\" \n");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::INCOMPLETE_QUOTED_STRING);
      REQUIRE(token.value() == " \n");
      SECTION("begin")
      {
         auto [ x,y ] = token.begin();
         REQUIRE(x == 2);
         REQUIRE(y == 1);
      }
      SECTION("end")
      {
         auto [ x,y ] = token.end();
         REQUIRE(x == 3);
         REQUIRE(y == 0);
      }
   }   
   SECTION("complete")
   {
      is.str("stringa \t \"StringB \" \n\"Incomplete");
      auto token { tokenizer.next() };
      REQUIRE(token.type() == TokenType::STRING);
      REQUIRE(token.value() == "STRINGA");
      auto [x,y ] = token.begin();
      REQUIRE(x == 1);
      REQUIRE(y == 1);
      tie(x,y) = token.end(); /*!< Re-using x and y thanks to std::tie(). */
      REQUIRE(x == 1);
      REQUIRE(y == 7);
      token = tokenizer.next();
      REQUIRE(token.type() == TokenType::QUOTED_STRING);
      REQUIRE(token.value() == "StringB ");
      tie(x,y) = token.begin();
      REQUIRE(x == 1);
      REQUIRE(y == 11);
      tie(x,y) = token.end();
      REQUIRE(x == 1);
      REQUIRE(y == 20);
      token = tokenizer.next();
      REQUIRE(token.type() == TokenType::INCOMPLETE_QUOTED_STRING);
      REQUIRE(token.value() == "Incomplete");
      tie(x,y) = token.begin();
      REQUIRE(x == 2);
      REQUIRE(y == 1);
      tie(x,y) = token.end();
      REQUIRE(x == 2);
      REQUIRE(y == 11);
      token = tokenizer.next();
      REQUIRE(token.type() == TokenType::EMPTY);
   }   
}
