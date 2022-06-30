#ifndef EC_RULES_HPP
#define EC_RULES_HPP

#include "ec_tokenizer.hpp"
#include <string>
#include <set>
#include <vector>
#include <utility>

namespace ec_rules
{
   enum class RuleType
   {
      ANSWER,
      GOAL,
      IF,
      INVALID,
      LEGALANSWERS,
      QUESTION
   };

   class Rule
   {
      public:
         [[nodiscard]] virtual RuleType type() const = 0;
         void begin(const ec_tokenizer::Position& pos);
         void end(const ec_tokenizer::Position& pos);
         [[nodiscard]] const ec_tokenizer::Position& begin() const;
         [[nodiscard]] const ec_tokenizer::Position& end() const;
         virtual ~Rule() = default;
      private:
         ec_tokenizer::Position m_begin { 0,0 },
                                m_end { 0,0 };
   };

   class AnswerRule: public Rule
   {
      public:
         [[nodiscard]] bool text(const std::string& txt);
         [[nodiscard]] const std::string& text() const;
         [[nodiscard]] bool variable(const std::string& var);
         [[nodiscard]] const std::string& variable() const;
         [[nodiscard]] RuleType type() const override;
      private:
         std::string m_text,
                     m_variable;
   };

   class GoalRule: public Rule
   {
      public:
         [[nodiscard]] RuleType type() const override;
         [[nodiscard]] bool variable(const std::string& var);
         [[nodiscard]] const std::string& variable() const;
      private:
         std::string m_variable;
   };

   class IfRule: public Rule
   {
      public:
         RuleType type() const override;
         [[nodiscard]] bool condition(const std::string& variable,
                                      const std::string& value);
         [[nodiscard]] const std::vector<std::pair<std::string,std::string>>& 
            conditions() const;
         [[nodiscard]] bool conditionUses(const std::string& var) const;
         [[nodiscard]]  bool conclusion(const std::string& variable,
                                       const std::string& value);
         [[nodiscard]] const std::pair<std::string,std::string>& 
            conclusion() const;
         [[nodiscard]] std::size_t size() const;
         [[nodiscard]] bool isAbout(const std::string& variable) const;
         [[nodiscard]] bool isValid() const;
         [[nodiscard]] bool operator==(const IfRule& ifRule) const;
      private:
         std::vector<std::pair<std::string,std::string>> m_conditions;
         std::pair<std::string,std::string> m_conclusion;
   };

   class LegalAnswersRule: public Rule
   {
      public:
         [[nodiscard]] bool insert(const std::string& v);
         [[nodiscard]] bool contains(const std::string& v) const;
         [[nodiscard]] std::size_t size() const;
         const std::set<std::string>& legalAnswers() const;
         [[nodiscard]] RuleType type() const override;
      private:
         std::set<std::string> m_values;
   };

   class QuestionRule: public AnswerRule
   {
      public:
         [[nodiscard]] RuleType type() const override;
         [[nodiscard]] bool operator<(const QuestionRule& qRule) const;
         /*!< Necessary for storing in a set<> */
   };
}
#endif // EC_RULES_HPP
