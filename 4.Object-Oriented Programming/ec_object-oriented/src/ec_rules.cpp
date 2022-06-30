#include "ec_rules.hpp"
#include "ec_tokenizer.hpp"
#include <algorithm>

namespace ec_rules
{

   using namespace ec_tokenizer;
   using namespace std;
   
   void Rule::begin(const ec_tokenizer::Position& pos)
   {
      m_begin = pos;
   }
   void Rule::end(const ec_tokenizer::Position& pos)
   {
      m_end = pos;
   }
   const ec_tokenizer::Position& Rule::begin() const
   {
      return m_begin;
   }
   const ec_tokenizer::Position& Rule::end() const
   {
      return m_end;
   }

   bool AnswerRule::text(const string& txt)
   {
      if (m_text != "")
      {
         return false;
      }
      m_text = txt;
      return true;
   }
   const string& AnswerRule::text() const
   {
      return m_text;
   }
   bool AnswerRule::variable(const string& var)
   {
      if (m_variable != "")
      {
         return false;
      }
      m_variable = var;
      return true;
   }
   const string& AnswerRule::variable() const
   {
      return m_variable;
   }
   RuleType AnswerRule::type() const
   {
      return RuleType::ANSWER;
   }

   bool GoalRule::variable(const string& var) 
   {
      if (m_variable != "")
      {
         return false;
      }
      m_variable = var;
      return true;
   }
   const std::string& GoalRule::variable() const
   {
      return m_variable;
   }
   RuleType GoalRule::type() const
   {
      return RuleType::GOAL;
   }

   RuleType IfRule::type() const
   {
      return RuleType::IF;
   }
   bool IfRule::condition(const string& variable,const string& value)
   {
      for (const auto& element : m_conditions)
      {
         if (element.first == variable)
         {
            return false;
         }
      }
      m_conditions.push_back(make_pair(variable,value));
      return true;
   }
   bool IfRule::conditionUses(const string& var) const
   {
      for (const auto& condition : m_conditions)
      {
         if (condition.first == var)
         {
            return true;
         }
      }
      return false;
   }
   const vector<pair<string,string>>& IfRule::conditions() const
   {
      return m_conditions;
   }
   bool IfRule::conclusion(const string& variable,const string& value)
   {
      if (m_conclusion.first != "")
      {
         return false;
      }
      m_conclusion.first = variable;
      m_conclusion.second = value;
      return true;
   }
   const pair<string,string>& IfRule::conclusion() const
   {
      return m_conclusion;
   }
   size_t IfRule::size() const
   {
      return m_conditions.size();
   }
   bool IfRule::isAbout(const string& variable) const
   {
      if (m_conclusion.first == variable)
      {
         return true;
      }
      return false;
   }
   bool IfRule::isValid() const
   {
      if (m_conditions.size() > 0 && m_conclusion.first != "")
      {
         return true;
      }
      return false;
   }
   bool IfRule::operator==(const IfRule& ifRule) const
   {
      if (conclusion().first != ifRule.conclusion().first ||
          conclusion().second != ifRule.conclusion().second ||
          conditions().size() != ifRule.conditions().size()
         )
      { 
         return false;
      };
      for (const auto& condition : conditions())
      {
         auto result { find(ifRule.conditions().begin(),
                            ifRule.conditions().end(),
                            condition)
                     };
         if (result == ifRule.conditions().end())
         {
            return false;
         }
      }
      return true;
   }

   bool LegalAnswersRule::insert(const std::string& v)
   {
      if (m_values.contains(v))
      {
         return false;
      }
      return m_values.insert(v).second;
   }
   bool LegalAnswersRule::contains(const std::string& v) const
   {
      return m_values.contains(v);
   }
   std::size_t LegalAnswersRule::size() const
   {
      return m_values.size();
   }
   const set<string>& LegalAnswersRule::legalAnswers() const
   {
      return m_values;
   }
   RuleType LegalAnswersRule::type() const 
   {
      return RuleType::LEGALANSWERS;
   }

   RuleType QuestionRule::type() const 
   {
      return RuleType::QUESTION;
   }
   bool QuestionRule::operator<(const QuestionRule& qRule) const
   {
      return variable() < qRule.variable(); /*!< Restrict comparsion
                                                 to variable(). */
   }
}
