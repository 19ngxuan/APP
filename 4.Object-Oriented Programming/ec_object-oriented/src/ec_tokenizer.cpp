#include "ec_tokenizer.hpp"
#include "ec_helper.hpp"
#include <stdexcept> /*!< Because of std::logic_error. */
#define FMT_HEADER_ONLY
#include <fmt/format.h> /*!< Because of fmt::format(). */

namespace ec_tokenizer
{
   using namespace std;
   using namespace ec_helper;
   using fmt::format;

   constexpr string_view
      ERROR_EMPTY
         { "{} MUST not be called if m_tokenType == TokenType::EMPTY." };

   const TokenType& Token::type() const
   {
      return m_tokenType;
   }
   const string Token::value() const
   {
      if (type() == TokenType::EMPTY)
      {
         throw logic_error(format(ERROR_EMPTY,"Token::value()"));
      }
      if (type() == TokenType::STRING)
      {
         return toupper(m_value); /*!< Therefore, return by value! */
      }
      return m_value;
   }
   const Position& Token::begin() const
   {
      if (type() == TokenType::EMPTY)
      {
         throw logic_error(format(ERROR_EMPTY,"Token::begin()"));
      }
      return m_begin;
   }
   const Position& Token::end() const
   {
      if (type() == TokenType::EMPTY)
      {
         throw logic_error(format(ERROR_EMPTY,"Token::end()"));
      }
      return m_end;
   }

   Token Tokenizer::next()
   {
      bool quotedString { false },
           unbalancedQuote { false };
      string token { };
      char c { };
      Position begin { m_position };
      if (!m_is)
      {
         return Token();
      }
      while (m_is)
      {
         if (!quotedString && m_is.peek() == Quote)
         {
            if (token.length() > 0)
            {
               return Token(TokenType::STRING,token,
                            begin,m_position);
            }
            m_is.get(); // Remove initial quote.
            m_position.second += 1;
            begin = m_position;
            quotedString = true;
            unbalancedQuote = true;
         }
         c = m_is.get();
         if (!m_is)
         {
            if (unbalancedQuote)
            {
               return Token(TokenType::INCOMPLETE_QUOTED_STRING,
                            token,begin,m_position);
            }
            else
            {
               return Token(); // return token with TokenType::EMPTY
            }
         }
         if (c == NewLine)
         {
            m_position.first += 1;
            m_position.second = 0;
         }
         else
         {
            m_position.second += 1;
         }
         if (quotedString && c != Quote)
         {
            token += c;
         }
         if (quotedString && c == Quote)
         {
            return Token(TokenType::QUOTED_STRING,token,
                         begin,m_position);
         }
         if (quotedString && m_is.peek() == Quote) // Take care for ""
         {
            m_is.get(); // Remove terminating quote
            m_position.second += 1;
            return Token(TokenType::QUOTED_STRING,token,
                         begin,m_position);
            
         }
         if (!quotedString && c > Space)
         {
            if (token.length() == 0)
            {
               begin = m_position;
            }
            token += c;
         }
         if (!quotedString && token.length() > 0 &&
             m_is.peek() <= Space)
         {
            return Token(TokenType::STRING,token,
                         begin,m_position);
         }
      }
      return  Token(TokenType::INCOMPLETE_QUOTED_STRING,token,
                    begin,m_position);
   }
}
