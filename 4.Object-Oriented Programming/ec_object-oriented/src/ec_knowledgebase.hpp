#ifndef EC_KNOWLEDGEBASE_H
#define EC_KNOWLEDGEBASE_H

#include "ec_rules.hpp"
#include <set>
#include <vector>
#include <utility>
#include <optional>

namespace ec_knowledgebase
{
   class KnowledgeBase
   {
      public:
         std::optional<ec_rules::AnswerRule>
            insert(const ec_rules::AnswerRule& aRule);
         bool answerRulePresent() const;
         const std::string& answerText() const;
         const std::string& answerVariable() const;
         std::optional<ec_rules::GoalRule> 
            insert(const ec_rules::GoalRule& gRule);
         bool goalRulePresent() const;
         const std::string& goalVariable() const;
         std::optional<ec_rules::IfRule> 
            insert(const ec_rules::IfRule& ifRule);
         std::size_t numberOfIfRules() const;
         std::vector<ec_rules::IfRule> 
            ifRules(const std::string& variable) const;
         std::size_t numberOfRuleLines() const;
         std::optional<ec_rules::LegalAnswersRule> 
            insert(const ec_rules::LegalAnswersRule& laRule);
         std::size_t numberOfLegalAnswers() const;
         bool isLegalAnswer(const std::string& answer) const;
         const std::set<std::string>& legalAnswers() const;
         std::optional<ec_rules::QuestionRule>
            insert(const ec_rules::QuestionRule& qRule);
         std::size_t numberOfQuestionRules() const;
         std::optional<std::string> 
            questionText(const std::string& variable) const;
         const std::set<ec_rules::QuestionRule>&
            questionRules() const;
         [[nodiscard]] bool isReady() const;
      private:
         ec_rules::AnswerRule m_answerRule;
         ec_rules::GoalRule m_goalRule;
         std::vector<ec_rules::IfRule> m_ifRules;
         ec_rules::LegalAnswersRule m_legalAnswersRule;
         std::set<ec_rules::QuestionRule> m_questionRules;
   };
}
#endif // EC_KNOWLEDGEBASE_H
