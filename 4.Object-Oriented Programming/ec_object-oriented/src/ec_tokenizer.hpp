#ifndef EC_TOKENIZER_HPP
#define EC_TOKENIZER_HPP

#include <string>
#include <istream>

namespace ec_tokenizer
{
   using Position = std::pair<std::size_t,std::size_t>;

   enum class TokenType
   {
      EMPTY,
      STRING,
      QUOTED_STRING,
      INCOMPLETE_QUOTED_STRING
   };

   class Token final
   {
      public:
         Token(const TokenType& tt = TokenType::EMPTY,
               const std::string& v = "",
               const Position& b = { 0,0 },
               const Position& e = { 0,0 }):m_tokenType { tt },
                                            m_value { v },
                                            m_begin { b },
                                            m_end { e }
         {
         }
         [[nodiscard]] const TokenType& type() const;
         [[nodiscard]] const std::string value() const;/*!< MUST return by value! */
         [[nodiscard]] const Position& begin() const;
         [[nodiscard]] const Position& end() const;
      private:
         TokenType m_tokenType;
         std::string m_value;
         Position m_begin,
                  m_end;
   };

   const char Quote { '\"' };
   const char NewLine { '\n' };
   const char Space { ' ' };

   class Tokenizer final
   {
      public:
         Tokenizer(std::istream& is):m_is { is },
                                     m_position { 1,0 }
         {
         }
         Token next();
      private:
         std::istream& m_is;
         Position m_position;
   };
}
#endif // EC_TOKENIZER_HPP
