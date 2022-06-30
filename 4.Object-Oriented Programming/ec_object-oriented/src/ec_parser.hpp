#ifndef EC_PARSER_HPP
#define EC_PARSER_HPP
#include <set>
#include <optional>
#include <ostream>
#include <iostream>
#include "ec_tokenizer.hpp"
#include "ec_knowledgebase.hpp"

namespace ec_parser
{
   class Parser final
   {
      public:
         Parser(ec_tokenizer::Tokenizer& t,
                std::ostream& err = std::cerr):m_tokenizer(t),
                                               m_err(err)
         {
         }
         [[nodiscard]] ec_rules::RuleType next();
         [[nodiscard]] const ec_tokenizer::Position& ruleBegin() const;
         [[nodiscard]] std::optional<ec_rules::AnswerRule> parseAnswerRule();
         [[nodiscard]] std::optional<ec_rules::GoalRule> parseGoalRule();
         [[nodiscard]] std::optional<ec_rules::IfRule> parseIfRule();
         [[nodiscard]] std::optional<ec_rules::LegalAnswersRule> 
            parseLegalAnswersRule();
         [[nodiscard]] std::optional<ec_rules::QuestionRule> 
            parseQuestionRule();
         [[nodiscard]] std::optional<ec_knowledgebase::KnowledgeBase>
            parseKnowledgeBase();
      private:
         [[nodiscard]] bool parseIsAre();
         [[nodiscard]]  bool parseVariableValue(std::string& v,
                                                ec_tokenizer::Position& start,
                                                ec_tokenizer::Position& stop);
         ec_tokenizer::Tokenizer& m_tokenizer;
         std::ostream& m_err;
         ec_tokenizer::Position m_ruleBegin { 0,0 };
         ec_rules::RuleType m_rule { ec_rules::RuleType::INVALID };
   };
}
#endif // EC_PARSER_HPP
