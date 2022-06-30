#include "ec_knowledgebase.hpp"
#include "ec_helper.hpp"
#include <string>
#include <exception>

namespace ec_knowledgebase
{
   using namespace std;
   using namespace ec_helper;
   using namespace ec_rules;

   const string
      ERROR_NO_ANSWER_RULE { "Answer rule not present!" },
      ERROR_NO_GOAL_RULE { "Goal rule not present!" };

   optional<AnswerRule> KnowledgeBase::insert(const AnswerRule& aRule)
   {
      if (answerRulePresent())
      {
         return m_answerRule;
      }
      m_answerRule = aRule;
      return nullopt;
   }
   bool KnowledgeBase::answerRulePresent() const
   {
      return m_answerRule.variable() != "";
   }
   const string& KnowledgeBase::answerText() const
   {
      if (!answerRulePresent())
      {
         throw logic_error(ERROR_NO_ANSWER_RULE);
      }
      return  m_answerRule.text();
   }
   const string& KnowledgeBase::answerVariable() const
   {
      if (!answerRulePresent())
      {
         throw logic_error(ERROR_NO_ANSWER_RULE);
      }
      return m_answerRule.variable();
   }
   optional<GoalRule> KnowledgeBase::insert(const GoalRule& gRule)
   {
      if (goalRulePresent())
      {
         return m_goalRule;
      }
      m_goalRule = gRule;
      return nullopt;
   }
   bool KnowledgeBase::goalRulePresent() const
   {
      return m_goalRule.variable() != "";
   }
  const string& KnowledgeBase::goalVariable() const
   {
      if (!goalRulePresent())
      {
         throw logic_error(ERROR_NO_GOAL_RULE);
      }
      return m_goalRule.variable();
   }
   optional<IfRule> KnowledgeBase::insert(const IfRule& ifRule)
   {
     for (const auto& rule : m_ifRules)
     {
        if (rule == ifRule)
        {
           return rule;
        }
     }
     m_ifRules.push_back(ifRule);
     return nullopt;
   }
   size_t KnowledgeBase::numberOfIfRules() const
   {
      return m_ifRules.size();
   }
   vector<IfRule> KnowledgeBase::ifRules(const string& variable) const
   {
      vector<IfRule> result;
      for (const auto& rule : m_ifRules)
      {
         if (rule.isAbout(variable))
         {
            result.push_back(rule);
         }
      }
      return result;
   }
   size_t  KnowledgeBase::numberOfRuleLines() const
   {
      size_t lines { 0 };
      for (const auto& ifRule : m_ifRules)
      {
         lines += ifRule.conditions().size() + 1;
      }
      return lines;
   }
   optional<LegalAnswersRule> 
      KnowledgeBase::insert(const LegalAnswersRule& laRule)
   {
      if (numberOfLegalAnswers() > 0)
      {
         return m_legalAnswersRule;
      }
      m_legalAnswersRule = laRule;
      return nullopt;
   }
   size_t KnowledgeBase::numberOfLegalAnswers() const
   {
      return m_legalAnswersRule.size();
   }
   bool KnowledgeBase::isLegalAnswer(const string& answer) const
   {
      return m_legalAnswersRule.contains(answer);
   }
   const set<string>& KnowledgeBase::legalAnswers() const
   {
      return m_legalAnswersRule.legalAnswers();
   }
   optional<QuestionRule> KnowledgeBase::insert(const QuestionRule& qRule)
   {
     if (m_questionRules.contains(qRule))
     {
        return *m_questionRules.find(qRule);
     }
     m_questionRules.insert(qRule);
     return nullopt;
   }
   size_t KnowledgeBase::numberOfQuestionRules() const
   {
      return m_questionRules.size();
   }
   optional<string>  
      KnowledgeBase::questionText(const string& variable) const
   {
      QuestionRule qRule;
      auto ignore { qRule.variable(variable) };
      auto result { m_questionRules.find(qRule) };
      if (result == m_questionRules.end())
      {
         return nullopt;
      }
      return result->text();
   }
   const set<QuestionRule>& KnowledgeBase::questionRules() const
   {
      return m_questionRules;
   }
   bool KnowledgeBase::isReady() const
   {
      if (answerRulePresent() &&
          goalRulePresent() &&
          numberOfIfRules() > 0 &&
          numberOfLegalAnswers() > 0 &&
          numberOfQuestionRules() > 0
         )
      {
         return true;
      }
      return false;
   }
}
