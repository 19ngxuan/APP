#include "ec_parser.hpp"
#include "ec_helper.hpp"
#include "ec_knowledgebase.hpp"
#include <fmt/format.h>
#include <stdexcept>
#include <fstream>
#include <algorithm>

namespace ec_parser
{
   using namespace std;
   using namespace ec_helper;
   using namespace ec_tokenizer;
   using namespace ec_rules;
   using namespace ec_knowledgebase;
   using fmt::format;
   

   constexpr string_view 
      AND { "AND" },
      ANSWER { "ANSWER" },
      ARE { "ARE" },
      ASTERISK { "*" },
      GOAL { "GOAL" },
      IF { "IF" },
      IS { "IS" },
      LEGALANSWERS { "LEGALANSWERS" },
      THEN { "THEN" },
      QUESTION { "QUESTION" },

      KEYWORDS_RULE { "ANSWER, GOAL, IF, LEGALANSWERS or QUESTION" },
      KEYWORDS_IS_ARE { "IS or ARE" },
      KEYWORDS_AND_THEN { "AND or THEN" },
      STRING { "STRING.WITHOUT.QUOTES" },
      QUOTED_STRING { "\"string in quotes\"" },
      INCOMPLETE_QUOTED_STRING { "\"string without terminating quote" },
      NOTHING { "nothing" },
      ERROR_STRING { "Expected {}, got "
                     "{} ({},{} - "
                     "{},{})." 
                   },
      ERROR_EMPTY { "Expected {}, got nothing." 
                  },
      ERROR_DUPLICATE_VALUE { "Duplicate value in Legal Answers rule: "
                              "{} ({},{} - {},{})." 
                            },
      ERROR_DUPLICATE_RULE { "Duplicate {} rule: "
                             "({},{} - {},{}) and "
                             "({},{} - {},{})."  
                           },
      ERROR_INSERT_VALUE { "Failed to insert the following value into "
                           "Legal Answers rule: "
                           "{} ({},{} - {},{})." 
                         },
      ERROR_TOO_FEW_VALUES { "At least two different values are required in "
                             "Legal Answers rule, only {} found."
                           },
      ERROR_UNKNOWN { "Unknown error" };
   const string                        
      ERROR_NO_ANSWER_RULE { "No Answer rule to parse!" },
      ERROR_NO_LEGAL_ANSWERS_RULE { "No Legal Answers rule to parse!" },
      ERROR_NO_GOAL_RULE { "No Goal rule to parse!" },
      ERROR_NO_IF_RULE { "No If rule to parse!" },
      ERROR_NO_QUESTION_RULE { "No Question rule to parse!" };
      
   RuleType Parser::next()
   {
      auto token { m_tokenizer.next() };
      if (token.type() == TokenType::STRING)
      {
         m_ruleBegin = token.begin();
         if (token.value() == ANSWER)
         {
            return (m_rule = RuleType::ANSWER);
         }
         if (token.value() == GOAL)
         {
            return (m_rule = RuleType::GOAL);
         }
         if (token.value() == IF)
         {
            return (m_rule = RuleType::IF);
         }
         if (token.value() == LEGALANSWERS)
         {
            return (m_rule = RuleType::LEGALANSWERS);
         }
         if (token.value() == QUESTION)
         {
            return (m_rule = RuleType::QUESTION);
         }
         m_ruleBegin = { 0,0 };
         error(format(ERROR_STRING,KEYWORDS_RULE,token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
      }
      if (token.type() == TokenType::QUOTED_STRING)
      {
         error(format(ERROR_STRING,KEYWORDS_RULE,token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
      }
      if (token.type() == TokenType::INCOMPLETE_QUOTED_STRING)
      {
         error(format(ERROR_STRING,KEYWORDS_RULE,
                      token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
      }
      if (token.type() == TokenType::EMPTY)
      {
         /*! 
          * In this case, no error message should be issued,
          * because every knowledge base eventually runs out
          * of rules and it is not known in advance how many
          * rules a knowledge base contains.          
          */
         //error(format(ERROR_EMPTY,KEYWORDS_RULE),m_err);
      }
      else
      {
         throw logic_error(string(ERROR_UNKNOWN)); /*!< This should 
                                                        not happen! */
      }
      return (m_rule = RuleType::INVALID);
   }

   const Position& Parser::ruleBegin() const
   {
      return m_ruleBegin;
   }

   bool Parser::parseIsAre()
   {
      auto token { m_tokenizer.next() };
      if (token.type() == TokenType::STRING)
      {
         m_ruleBegin = token.begin();
         if (token.value() == IS || token.value() == ARE)
         {
            return true;
         }
         error(format(ERROR_STRING,KEYWORDS_IS_ARE,token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
         return false;
      }
      if (token.type() == TokenType::QUOTED_STRING)
      {
         error(format(ERROR_STRING,KEYWORDS_IS_ARE,token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
         return false;
      }
      if (token.type() == TokenType::INCOMPLETE_QUOTED_STRING)
      {
         error(format(ERROR_STRING,KEYWORDS_IS_ARE,
                      token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
         return false;
      }
      if (token.type() == TokenType::EMPTY)
      {
         error(format(ERROR_EMPTY,KEYWORDS_IS_ARE),m_err);
         return false;
      }
      throw logic_error(string(ERROR_UNKNOWN)); /*!< This should 
                                                     not happen! */

      return false;
   }

   bool Parser::parseVariableValue(string& v,Position& start,Position& stop)
   {
      auto token { m_tokenizer.next() };
      if (token.type() == TokenType::EMPTY)
      {
         error(format(ERROR_EMPTY,STRING),m_err);
         return false;
      }
      if (token.type() == TokenType::QUOTED_STRING)
      {
         error(format(ERROR_STRING,STRING,token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
         return false;
      }
      if (token.type() == TokenType::INCOMPLETE_QUOTED_STRING)
      {
         error(format(ERROR_STRING,STRING,
                      token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
          return false;
      }
      v = token.value();
      start = token.begin();
      stop = token.end();
      return true;
   }

   optional<AnswerRule> Parser::parseAnswerRule()
   {
      if (m_rule != RuleType::ANSWER)
      {
         throw logic_error(ERROR_NO_ANSWER_RULE);
      }
      AnswerRule aRule;
      aRule.begin(m_ruleBegin);
      if (!parseIsAre())
      {
         return nullopt;
      }
      string value { };
      auto token { m_tokenizer.next() };
      if (token.type() == TokenType::EMPTY)
      {
         error(format(ERROR_EMPTY,QUOTED_STRING),m_err);
         return nullopt;
      }
      value = token.value();
      if (token.type() == TokenType::STRING)
      {
         error(format(ERROR_STRING,QUOTED_STRING,value,
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
         return nullopt;
      }
      if (token.type() == TokenType::INCOMPLETE_QUOTED_STRING)
      {
         error(format(ERROR_STRING,QUOTED_STRING,
                      value,
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
          return nullopt;
      }
      if (!aRule.text(token.value()))
      {
         throw logic_error(string(ERROR_UNKNOWN));  /*!< This should 
                                                         not happen! */
         return nullopt;
      }
      token = m_tokenizer.next();
      if (token.type() == TokenType::EMPTY)
      {
         error(format(ERROR_EMPTY,STRING),m_err);
         return nullopt;
      }
      value = token.value();
      if (token.type() == TokenType::QUOTED_STRING)
      {
         error(format(ERROR_STRING,STRING,value,
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
         return nullopt;
      }
      if (token.type() == TokenType::INCOMPLETE_QUOTED_STRING)
      {
         error(format(ERROR_STRING,STRING,
                      value,
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
          return nullopt;
      }
      if (!aRule.variable(token.value()))
      {
         throw logic_error (string(ERROR_UNKNOWN));  /*!< This should 
                                                          not happen! */
         return nullopt;
      }
      aRule.end(token.end());
      return aRule;
   }

   optional<ec_rules::GoalRule> Parser::parseGoalRule()   
   {
      if (m_rule != RuleType::GOAL)
      {
         throw logic_error(ERROR_NO_GOAL_RULE);
      }
      GoalRule gRule;
      gRule.begin(m_ruleBegin);
      if (!parseIsAre())
      {
         return nullopt;
      }
      auto token { m_tokenizer.next() };
      if (token.type() == TokenType::EMPTY)
      {
         error(format(ERROR_EMPTY,STRING),m_err);
         return nullopt;
      }
      if (token.type() == TokenType::QUOTED_STRING)
      {
         error(format(ERROR_STRING,STRING,token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
         return nullopt;
      }
      if (token.type() == TokenType::INCOMPLETE_QUOTED_STRING)
      {
         error(format(ERROR_STRING,QUOTED_STRING,
                      token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
          return nullopt;
      }
      if (!gRule.variable(token.value()))
      {
         throw logic_error(string(ERROR_UNKNOWN));  /*!< This should 
                                                         not happen! */
         return nullopt;
      }
      gRule.end(token.end());
      return gRule;
   }

   optional<IfRule> Parser::parseIfRule()
   {
      if (m_rule != RuleType::IF)
      {
         throw logic_error(ERROR_NO_IF_RULE);
      }
      IfRule ifRule;
      ifRule.begin(m_ruleBegin);
      Position start,
               stop;
      string variable { },
             value { },
             keyword { };
      do
      {
         if (!parseVariableValue(variable,start,stop) ||
             !parseIsAre() ||
             !parseVariableValue(value,start,stop))
         {
            return nullopt;
         };
         if (!ifRule.condition(variable,value) || 
            !parseVariableValue(keyword,start,stop))
         {
            return nullopt;
         };
         if (keyword != AND && keyword != THEN)
         {
            error(format(ERROR_STRING,KEYWORDS_AND_THEN,keyword,
                         start.first,start.second,stop.first,stop.second),
                         m_err
                 );
            return nullopt;
         }
      } while (keyword == AND);
      if (!parseVariableValue(variable,start,stop) ||
          !parseIsAre() ||
          !parseVariableValue(value,start,stop))
      {
         return nullopt;
      };
      if (!ifRule.conclusion(variable,value))
      {
         return nullopt;
      };
      ifRule.end(stop);
      return ifRule;
   }

   optional<LegalAnswersRule> Parser::parseLegalAnswersRule()
   {
      if (m_rule != RuleType::LEGALANSWERS)
      {
         throw logic_error(ERROR_NO_LEGAL_ANSWERS_RULE);
      }
      LegalAnswersRule laRule;
      laRule.begin(m_ruleBegin);
      if (!parseIsAre())
      {
         return nullopt;
      }
      string value { };
      do
      {
         auto token { m_tokenizer.next() };
         if (token.type() == TokenType::EMPTY)
         {
            error(format(ERROR_EMPTY,STRING),m_err);
            return nullopt;
         }
         value = token.value();
         if (token.type() == TokenType::QUOTED_STRING)
         {
            error(format(ERROR_STRING,STRING,value,
                         token.begin().first,token.begin().second,
                         token.end().first,token.end().second
                        ),m_err
                 );
            return nullopt;
         }
         if (token.type() == TokenType::INCOMPLETE_QUOTED_STRING)
         {
            error(format(ERROR_STRING,STRING,
                         value,
                         token.begin().first,token.begin().second,
                         token.end().first,token.end().second
                        ),m_err
                 );
             return nullopt;
         }
         if (value != ASTERISK)
         {
            if (laRule.contains(value))
            {
               error(format(ERROR_DUPLICATE_VALUE,value,
                            token.begin().first,token.begin().second,
                            token.end().first,token.end().second
                           ),m_err
                    );
               return nullopt;
            }
            auto inserted { laRule.insert(value) };
            if (!inserted)
            {
               error(format(ERROR_INSERT_VALUE,value,
                            token.begin().first,token.begin().second,
                            token.end().first,token.end().second
                           ),m_err
                    );
                return nullopt;
            }
            laRule.end(token.end());
         }
      } while (value != ASTERISK);      
      if (laRule.size() < 2)
      {
         error(format(ERROR_TOO_FEW_VALUES,laRule.size()));
         return std::nullopt;
      }      
      return laRule;
   }
   optional<QuestionRule> Parser::parseQuestionRule()
   {
      if (m_rule != RuleType::QUESTION)
      {
         throw logic_error(ERROR_NO_QUESTION_RULE);
      }
      QuestionRule qRule;
      qRule.begin(m_ruleBegin);
      auto token { m_tokenizer.next() };
      if (token.type() == TokenType::EMPTY)
      {
         error(format(ERROR_EMPTY,STRING),m_err);
         return nullopt;
      }
      if (token.type() == TokenType::QUOTED_STRING)
      {
         error(format(ERROR_STRING,STRING,token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
         return nullopt;
      }
      if (token.type() == TokenType::INCOMPLETE_QUOTED_STRING)
      {
         error(format(ERROR_STRING,STRING,
                      token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
          return nullopt;
      }
      if (!qRule.variable(token.value()))
      {
         throw logic_error(string(ERROR_UNKNOWN));  /*!< This should 
                                                         not happen! */

         return nullopt;
      };
      if (!parseIsAre())
      {
         return nullopt;
      }
      token = m_tokenizer.next();
      if (token.type() == TokenType::EMPTY)
      {
         error(format(ERROR_EMPTY,QUOTED_STRING),m_err);
         return nullopt;
      }
      if (token.type() == TokenType::STRING)
      {
         error(format(ERROR_STRING,QUOTED_STRING,token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
         return nullopt;
      }
      if (token.type() == TokenType::INCOMPLETE_QUOTED_STRING)
      {
         error(format(ERROR_STRING,QUOTED_STRING,
                      token.value(),
                      token.begin().first,token.begin().second,
                      token.end().first,token.end().second
                     ),m_err
              );
          return nullopt;
      }
      if (!qRule.text(token.value()))
      {
         throw logic_error(string(ERROR_UNKNOWN)); /*!< This should 
                                                        not happen! */
         return nullopt;
      }
      qRule.end(token.end());
      return qRule;
   }

    optional<KnowledgeBase> Parser::parseKnowledgeBase()
    {
       KnowledgeBase kb;
       RuleType ruleType { };
       do
       {
          ruleType = next();
          switch (ruleType)
          {
             case RuleType::ANSWER : 
                {
                   auto rule { parseAnswerRule() };
                   if (!rule)
                   {
                      return nullopt;
                   }
                   auto result { kb.insert(rule.value()) };
                   if (result)
                   {
                      error(format(ERROR_DUPLICATE_RULE,ANSWER,   
                                   result.value().begin().first,
                                   result.value().begin().second,
                                   result.value().end().first,
                                   result.value().end().second,
                                   rule.value().begin().first,
                                   rule.value().begin().second,
                                   rule.value().end().first,
                                   rule.value().end().second
                                  ),m_err
                          );
                      return nullopt;
                   }
                } 
                break;
             case RuleType::GOAL :
                {
                   auto rule { parseGoalRule() };
                   if (!rule)
                   {
                      return nullopt;
                   }
                   auto result { kb.insert(rule.value()) };
                   if (result)
                   {
                      error(format(ERROR_DUPLICATE_RULE,GOAL,   
                                   result.value().begin().first,
                                   result.value().begin().second,
                                   result.value().end().first,
                                   result.value().end().second,
                                   rule.value().begin().first,
                                   rule.value().begin().second,
                                   rule.value().end().first,
                                   rule.value().end().second
                                  ),m_err
                          );
                      return nullopt;
                   }
                } 
                break;
             case RuleType::IF :
                {
                   auto rule { parseIfRule() };
                   if (!rule)
                   {
                      return nullopt;
                   }
                   auto result { kb.insert(rule.value()) };
                   if (result)
                   {
                      error(format(ERROR_DUPLICATE_RULE,IF,   
                                   result.value().begin().first,
                                   result.value().begin().second,
                                   result.value().end().first,
                                   result.value().end().second,
                                   rule.value().begin().first,
                                   rule.value().begin().second,
                                   rule.value().end().first,
                                   rule.value().end().second
                                  ),m_err
                          );
                      return nullopt;
                   }
                } 
                break;
             case RuleType::LEGALANSWERS :
                {
                   auto rule { parseLegalAnswersRule() };
                   if (!rule)
                   {
                      return nullopt;
                   }
                   auto result { kb.insert(rule.value()) };
                   if (result)
                   {
                      error(format(ERROR_DUPLICATE_RULE,LEGALANSWERS,   
                                   result.value().begin().first,
                                   result.value().begin().second,
                                   result.value().end().first,
                                   result.value().end().second,
                                   rule.value().begin().first,
                                   rule.value().begin().second,
                                   rule.value().end().first,
                                   rule.value().end().second
                                  ),m_err
                          );
                      return nullopt;
                   }
                } 
                break;
             case RuleType::QUESTION :
                {
                   auto rule { parseQuestionRule() };
                   if (!rule)
                   {
                      return nullopt;
                   }
                   auto result { kb.insert(rule.value()) };
                   if (result)
                   {
                      error(format(ERROR_DUPLICATE_RULE,QUESTION,   
                                   result.value().begin().first,
                                   result.value().begin().second,
                                   result.value().end().first,
                                   result.value().end().second,
                                   rule.value().begin().first,
                                   rule.value().begin().second,
                                   rule.value().end().first,
                                   rule.value().end().second
                                  ),m_err
                          );
                      return nullopt;
                   }
                }
                break;
             case RuleType::INVALID :
                break;
             default: 
                throw logic_error(string(ERROR_UNKNOWN)); /*!< This should 
                                                               not happen! */
                break;
          }
       } while (ruleType != RuleType::INVALID );
       // SOME MORE ERRORS TO DETECT HERE!
       if (!kb.answerRulePresent())
       {
          error(ERROR_NO_ANSWER_RULE,m_err);
          return nullopt;
       }
       if (!kb.goalRulePresent())
       {
          error(ERROR_NO_GOAL_RULE,m_err);
          return nullopt;
       }
       if (kb.numberOfIfRules() == 0)
       {
          error(ERROR_NO_IF_RULE,m_err);
          return nullopt;
       }
       if (kb.numberOfLegalAnswers() == 0)
       {
          error(ERROR_NO_LEGAL_ANSWERS_RULE,m_err);
          return nullopt;
       }
       if (kb.numberOfQuestionRules() == 0)
       {
          error(ERROR_NO_QUESTION_RULE,m_err);
          return nullopt;
       }
       return kb;
   }
}
