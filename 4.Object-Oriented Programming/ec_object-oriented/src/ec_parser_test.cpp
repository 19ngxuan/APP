#include "ec_parser.hpp"
#include "ec_knowledgebase.hpp"
#include <sstream>
#include <optional>
#include <iostream>

TEST_CASE("Parser","[ec_parser]")
{
   using namespace std;
   using namespace ec_parser;
   using namespace ec_tokenizer;
   using namespace ec_rules;

   istringstream is;
   Tokenizer tokenizer(is);
   ostringstream err;
   Parser parser(tokenizer,err);
   SECTION("RuleType::INVALID")
   {
      REQUIRE(parser.next() == RuleType::INVALID);
   }
   /* 
    * parseAnswerRule 
    */
   SECTION("parseAnswerRule") // Happy path
   {
      is.str("ANSWER is \"This is the answer\" answer");
      REQUIRE(parser.next() == RuleType::ANSWER);
      auto result { parser.parseAnswerRule() };
      REQUIRE(result.has_value() == true);/*!< Instead of 
                                               REQUIRE(result == ...  */
      REQUIRE(result.value().text() == "This is the answer");
      REQUIRE(result.value().variable() == "ANSWER");
   }
   // Errors
   SECTION("NO_ANSWER_RULE")
   {
      is.str("QUESTION");
      REQUIRE(parser.next() == RuleType::QUESTION);
      SECTION("logic_error")
      {
         REQUIRE_THROWS_AS(parser.parseAnswerRule(),logic_error);
      }
      SECTION("what()")
      {
         REQUIRE_THROWS_WITH(parser.parseAnswerRule(),
                             "No Answer rule to parse!");
      }
   }
   SECTION("NO_IS_ARE")
   {
      is.str("answer invalid");
      REQUIRE(parser.next() == RuleType::ANSWER);
      auto result { parser.parseAnswerRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() == 
              "Expected IS or ARE, got INVALID (1,8 - 1,14).\n"
             );
   }
   SECTION("NO_TEXT")
   {
      is.str("ANswer is");
      REQUIRE(parser.next() == RuleType::ANSWER);
      auto result { parser.parseAnswerRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected \"string in quotes\", got nothing.\n"
             );
   }
   SECTION("VARIABLE_INSTEAD_OF_TEXT")
   {
      is.str("ANswer is UnquoTed_StrinG");
      REQUIRE(parser.next() == RuleType::ANSWER);
      auto result { parser.parseAnswerRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected \"string in quotes\", "
              "got UNQUOTED_STRING (1,11 - 1,25).\n"
             );
   }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTEAD_OF_TEXT")
   {
      is.str("ANswer is \"incomplete quoted string");
      REQUIRE(parser.next() == RuleType::ANSWER);
      auto result { parser.parseAnswerRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected \"string in quotes\", "
              "got incomplete quoted string (1,11 - 1,35).\n"
             );
   }
   SECTION("NO_VARIABLE")
   {
      is.str("answer IS \"This is the answer: \"");
      REQUIRE(parser.next() == RuleType::ANSWER);
      auto result { parser.parseAnswerRule() };
      REQUIRE(result.has_value() == false);/*!< Instead of 
                                                REQUIRE(result == ...*/ 
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got nothing.\n"
             );
   }
   SECTION("TEXT_INSTEAD_OF_VARIABLE")
   {
      is.str("answer IS \"This is the answer: \" \"quoted string\"");
      REQUIRE(parser.next() == RuleType::ANSWER);
      auto result { parser.parseAnswerRule() };
      REQUIRE(result.has_value() == false);/*!< Instead of 
                                                REQUIRE(result == ...*/ 
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, "
              "got quoted string (1,34 - 1,48).\n"
             );
   }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTEAD_OF_VARIABLE")
   {
      is.str("answer IS \"This is the answer: \" \"incomplete");
      REQUIRE(parser.next() == RuleType::ANSWER);
      auto result { parser.parseAnswerRule() };
      REQUIRE(result.has_value() == false);/*!< Instead of 
                                                REQUIRE(result == ...*/ 
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, "
              "got incomplete (1,34 - 1,44).\n"
             );
   }
  /* 
    * parseGoalRule 
    */
   SECTION("parseGoalRule") // Happy path
   {
      is.str("goal is some_goal");
      REQUIRE(parser.next() == RuleType::GOAL);
      auto result { parser.parseGoalRule() };
      REQUIRE(result.has_value() == true);/*!< Instead of 
                                               REQUIRE(result == ...*/
      REQUIRE(result.value().variable() == "SOME_GOAL");
   }
   // Errors
   SECTION("NO_GOAL_RULE")
   {
      is.str("IF");
      REQUIRE(parser.next() == RuleType::IF);
      SECTION("logic_error")
      {
         REQUIRE_THROWS_AS(parser.parseGoalRule(),logic_error);
      }
      SECTION("what()")
      {
         REQUIRE_THROWS_WITH(parser.parseGoalRule(),
                             "No Goal rule to parse!");
      }
   }
   SECTION("NO_IS_ARE")
   {
      is.str("GOAL invalid");
      REQUIRE(parser.next() == RuleType::GOAL);
      auto result { parser.parseGoalRule() };
      REQUIRE(result.has_value() == false);/*!< Instead of 
                                                REQUIRE(result == ...*/ 
      REQUIRE(err.str() ==
              "Expected IS or ARE, got INVALID (1,6 - 1,12).\n"
             );
   }
   SECTION("NO_VARIABLE")
   {
      is.str("GOAL is");
      REQUIRE(parser.next() == RuleType::GOAL);
      auto result { parser.parseGoalRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got nothing.\n"
             );
   }
   SECTION("TEXT_INSTEAD_OF_VARIABLE")
   {
      is.str("GOAL is \"quoted string\"");
      REQUIRE(parser.next() == RuleType::GOAL);
      auto result { parser.parseGoalRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, "
              "got quoted string (1,9 - 1,23).\n"
             );
   }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTEAD_OF_VARIABLE")
   {
      is.str("GOAL is \"incomplete quoted string");
      REQUIRE(parser.next() == RuleType::GOAL);
      auto result { parser.parseGoalRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected \"string in quotes\", "
              "got incomplete quoted string (1,9 - 1,33).\n"
             );
   }
   /* 
    * parseIfRule 
    */
   SECTION("parseIfRule") // Happy path
   {
      is.str("If hasTail is yes and hasFur is no\nThen type is Tail");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == true);/*!< Instead of 
                                               REQUIRE(result == ...  */
      REQUIRE(result.value().size() == 2);
      REQUIRE(result.value().isValid() == true);
      REQUIRE(result.value().isAbout("TYPE") == true);
      auto c { result.value().conditions() };
      REQUIRE(c.size() == 2);
      REQUIRE(c.at(0).first == "HASTAIL"); 
      REQUIRE(c.at(0).second == "YES"); 
      REQUIRE(c.at(1).first == "HASFUR"); 
      REQUIRE(c.at(1).second == "NO"); 
      REQUIRE(result.value().conclusion().first == "TYPE");
      REQUIRE(result.value().conclusion().second == "TAIL");
   }
   SECTION("NO_IF_RULE")
   {
      is.str("GOAL");
      REQUIRE(parser.next() == RuleType::GOAL);
      SECTION("logic_error")
      {
         REQUIRE_THROWS_AS(parser.parseIfRule(),logic_error);
      }
      SECTION("what()")
      {
         REQUIRE_THROWS_WITH(parser.parseIfRule(),
                             "No If rule to parse!");
      }
   }
   SECTION("TEXT_INSTEAD_OF_VARIABLE")
   {
      is.str("IF \"no_variable\" is value THEN x is y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, "
              "got no_variable (1,4 - 1,16).\n"
             );
   }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTEAD_OF_VARIABLE")
   {
      is.str("IF \"no_variable is value THEN x is y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, "
              "got no_variable is value THEN x is y (1,4 - 1,36).\n"
             );
   }
   SECTION("NO_IS_ARE")
   {
      is.str("IF variable value THEN x is y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected IS or ARE, got VALUE (1,13 - 1,17).\n"
             );
   }
   SECTION("TEXT_INSTEAD_OF_VALUE")
   {
      is.str("IF variable is \"no_value\" THEN x is y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got no_value (1,16 - 1,25).\n"
             );
   }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTEAD_OF_VALUE")
   {
      is.str("IF variable is \"no_value THEN x is y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, "
              "got no_value THEN x is y (1,16 - 1,36).\n"
             );
   }
   SECTION("NO_AND_OR_THEN")
   {
      is.str("IF variable is value \"AND\" x is y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got AND (1,22 - 1,26).\n"
             );
   }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTEAD_OF_AND_OR_THEN")
   {
      is.str("IF variable is value \"THEN x is y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, "
              "got THEN x is y (1,22 - 1,33).\n"
             );
   }
   SECTION("NO_CONCLUSION")
   {
      is.str("IF variable is value THEN");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got nothing.\n"
             );
   }
   SECTION("NO_VARIABLE")
   {
      is.str("IF variable is value THEN \"X\" IS Y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got X (1,27 - 1,29).\n"
             );
   }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTEAD_OF_VARIABLE")
   {
      is.str("IF variable is value THEN \"X IS Y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got X IS Y (1,27 - 1,33).\n"
             );
   }
   SECTION("NO_IS_ARE")
   {
      is.str("IF variable is value THEN X \"IS\" Y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected IS or ARE, got IS (1,29 - 1,32).\n"
             );
   }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTEAD_OF_IS_ARE")
   {
      is.str("IF variable is value THEN X \"ARE Y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected IS or ARE, got ARE Y (1,29 - 1,34).\n"
             );
   }
   SECTION("NO_VALUE")
   {
      is.str("IF variable is value THEN X ARE");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got nothing.\n"
             );
   }
   SECTION("TEXT_INSTEAD_OF_VALUE")
   {
      is.str("IF variable is value THEN X ARE \"Y\"");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got Y (1,33 - 1,35).\n"
             );
   }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTEAD_OF_VALUE")
   {
      is.str("IF variable is value THEN X ARE \"Y");
      REQUIRE(parser.next() == RuleType::IF);
      auto result { parser.parseIfRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got Y (1,33 - 1,34).\n"
             );
   }
   /* 
    * parseLegalAnswersRule 
    */
   SECTION("parseLegalAnswersRule") // Happy path
   {
      is.str("LEGALANSWERS IS a b c *");
      REQUIRE(parser.next() == RuleType::LEGALANSWERS);
      auto result { parser.parseLegalAnswersRule() };
      REQUIRE(result.has_value() == true);/*!< Instead of 
                                               REQUIRE(result == ...*/      
      REQUIRE(result.value().size() == 3);
      REQUIRE(result.value().contains("A") == true);
      REQUIRE(result.value().contains("B") == true);
      REQUIRE(result.value().contains("C") == true);
      REQUIRE(result.value().contains("D") == false);
   }
   // Errors
   SECTION("NO_LEGAL_ANSWERS_RULE")
   {
      is.str("IF");
      REQUIRE(parser.next() == RuleType::IF);
      SECTION("logic_error")
      {
         REQUIRE_THROWS_AS(parser.parseLegalAnswersRule(),logic_error);
      }
      SECTION("what()")
      {
         REQUIRE_THROWS_WITH(parser.parseLegalAnswersRule(),
                             "No Legal Answers rule to parse!");
      }
   }
   SECTION("NO_IS_ARE")
   {
      is.str("LegalAnswers invalid");
      REQUIRE(parser.next() == RuleType::LEGALANSWERS);
      auto result { parser.parseLegalAnswersRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected IS or ARE, got INVALID (1,14 - 1,20).\n"
             );
   }
   SECTION("NO_VALUE")
   {
      is.str("LEGALANSWERS ARE");
      REQUIRE(parser.next() == RuleType::LEGALANSWERS);
      auto result { parser.parseLegalAnswersRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got nothing.\n"
             );
   }
   SECTION("TEXT_INSTEAD_OF_VALUE")
   {
      is.str("LegalAnswers are \"quoted string\"");
      REQUIRE(parser.next() == RuleType::LEGALANSWERS);
      auto result { parser.parseLegalAnswersRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, "
              "got quoted string (1,18 - 1,32).\n"
             );
   }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTAD_OF_VALUE")
   {
      is.str("legalanswers IS \"incomplete quoted string");
      REQUIRE(parser.next() == RuleType::LEGALANSWERS);
      auto result { parser.parseLegalAnswersRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, "
              "got incomplete quoted string (1,17 - 1,41).\n"
             );
   }
   /* 
    * parseQuestionRule 
    */
   SECTION("parseQuestionRule") // Happy path
   {
      is.str("queSTION variable is \"Is it true? \"");
      REQUIRE(parser.next() == RuleType::QUESTION);
      auto result { parser.parseQuestionRule() };
      REQUIRE(result.has_value() == true);/*!< Instead of 
                                               REQUIRE(result == ...  */
      REQUIRE(result.value().variable() == "VARIABLE");
      REQUIRE(result.value().text() == "Is it true? ");
   }
   // Errors
   SECTION("NO_QUESTION_RULE")
   {
      is.str("ANSWER");
      REQUIRE(parser.next() == RuleType::ANSWER);
      SECTION("logic_error")
      {
         REQUIRE_THROWS_AS(parser.parseQuestionRule(),logic_error);
      }
      SECTION("what()")
      {
         REQUIRE_THROWS_WITH(parser.parseQuestionRule(),
                             "No Question rule to parse!");
      }
   }
   SECTION("NO_VARIABLE")
   {
      is.str("Question ");
      REQUIRE(parser.next() == RuleType::QUESTION);
      auto result { parser.parseQuestionRule() };
      REQUIRE(result.has_value() == false);/*!< Instead of 
                                                REQUIRE(result == ...*/ 
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, got nothing.\n"
             );
   }
   SECTION("TEXT_INSTEAD_OF_VARIABLE")
   {
      is.str("question \"no_variable\"");
      REQUIRE(parser.next() == RuleType::QUESTION);
      auto result { parser.parseQuestionRule() };
      REQUIRE(result.has_value() == false);/*!< Instead of 
                                                REQUIRE(result == ...*/ 
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, "
              "got no_variable (1,10 - 1,22).\n"
             );
  }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTEAD_OF_VARIABLE")
   {
      is.str("question \"no_variable");
      REQUIRE(parser.next() == RuleType::QUESTION);
      auto result { parser.parseQuestionRule() };
      REQUIRE(result.has_value() == false);/*!< Instead of 
                                                REQUIRE(result == ...*/ 
      REQUIRE(err.str() ==
              "Expected STRING.WITHOUT.QUOTES, "
              "got no_variable (1,10 - 1,21).\n"
             );
   }
   SECTION("NO_IS_ARE")
   {
      is.str("question vArIaBlE");
      REQUIRE(parser.next() == RuleType::QUESTION);
      auto result { parser.parseQuestionRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected IS or ARE, got nothing.\n"
             );
   }
   SECTION("NO_TEXT")
   {
      is.str("question variable is");
      REQUIRE(parser.next() == RuleType::QUESTION);
      auto result { parser.parseQuestionRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected \"string in quotes\", got nothing.\n"
             );
   }
   SECTION("VARIABLE_INSTEAD_OF_TEXT")
   {
      is.str("question variable is no_text");
      REQUIRE(parser.next() == RuleType::QUESTION);
      auto result { parser.parseQuestionRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected \"string in quotes\", "
              "got NO_TEXT (1,22 - 1,28).\n"
             );
   }
   SECTION("INCOMPLETE_QUOTED_STRING_INSTEAD_OF_TEXT")
   {
      is.str("question variable is \"no_text");
      REQUIRE(parser.next() == RuleType::QUESTION);
      auto result { parser.parseQuestionRule() };
      REQUIRE(result.has_value() == false);
      REQUIRE(err.str() ==
              "Expected \"string in quotes\", "
              "got no_text (1,22 - 1,29).\n"
             );
   }
   SECTION("parseKnowledgeBase")
   {
      string 
         la { "LEGALANSWERS ARE false true *\n\n" },
         gr { "GOAL IS logical.operator\n\n" },
         ar { "ANSWER IS \"The logical operator is \" logical.operator\n\n" },
         q1 { "QUESTION true.op.true\nIS \""
              "What results in true linked to true? \"\n\n" 
            },
         q2 { "QUESTION true.op.false\nIS \""
              "What results in true linked to false? \"\n\n" 
            },
         q3 { "QUESTION false.op.true\nIS \""
              "What results in false linked to true? \"\n\n" 
            },
         q4 { "QUESTION false.op.false\nIS \""
              "What results in false linked to false? \"\n\n" 
            },
         i1 { "IF true.op.true IS true\n"
              "AND true.op.false IS false\n"
              "AND false.op.true IS false\n"
              "AND false.op.false IS false\n"
              "THEN logical.operator IS logical_and\n\n"
            },
         i2 { "IF true.op.true IS true\n"
              "AND true.op.false IS true\n"
              "AND false.op.true IS true\n"
              "AND false.op.false IS false\n"
              "THEN logical.operator IS logical_or\n\n"
            },
         i3 { "IF true.op.true IS false\n"
              "AND true.op.false IS true\n"
              "AND false.op.true IS true\n"
              "AND false.op.false IS false\n"
              "THEN logical.operator IS logical_xor\n\n"
            },
         ok { la + gr + ar + q1 + q2 + q3 + q4 + i1 + i2 + i3 };
       SECTION("Happy path")
       {
          is.str(ok);
          auto result { parser.parseKnowledgeBase() };
          REQUIRE(result.has_value() == true);
          REQUIRE(result.value().isReady() == true);
       }
       SECTION("ERROR_NO_ANSWER_RULE")
       {
          is.str(gr + ar + q1 + q2 + q3 + q4 + i1 + i2 + i3);
          auto result { parser.parseKnowledgeBase() };
          REQUIRE(result.has_value() == false);
          REQUIRE(err.str() ==
                  "No Legal Answers rule to parse!\n"
                 );
       }
       SECTION("ERROR_NO_GOAL_RULE")
       {
          is.str(la + ar + q1 + q2 + q3 + q4 + i1 + i2 + i3);
          auto result { parser.parseKnowledgeBase() };
          REQUIRE(result.has_value() == false);
          REQUIRE(err.str() ==
                  "No Goal rule to parse!\n"
                 );
       }
       SECTION("ERROR_NO_LEGAL_ANSWER_RULE")
       {
          is.str(ar + gr + q1 + q2 + q3 + q4 + i1 + i2 + i3);
          auto result { parser.parseKnowledgeBase() };
          REQUIRE(result.has_value() == false);
          REQUIRE(err.str() ==
                  "No Legal Answers rule to parse!\n"
                 );
       }
       SECTION("ERROR_NO_QUESTION_RULE")
       {
          is.str(la + gr + ar + i1 + i2 + i3);
          auto result { parser.parseKnowledgeBase() };
          REQUIRE(result.has_value() == false);
          REQUIRE(err.str() ==
                  "No Question rule to parse!\n"
                 );
       }
       SECTION("ERROR_NO_IF_RULE")
       {
          is.str(la + gr + ar + q1 + q2 + q3 + q4);
          auto result { parser.parseKnowledgeBase() };
          REQUIRE(result.has_value() == false);
          REQUIRE(err.str() ==
                  "No If rule to parse!\n"
                 );
       }
       SECTION("DUPLICATE_ANSWER_RULE")
       {
          is.str(ok + ar);
          auto result { parser.parseKnowledgeBase() };
          REQUIRE(result.has_value() == false);
          REQUIRE(err.str() ==
                  "Duplicate ANSWER rule: "
                  "(5,1 - 5,53) and (37,1 - 37,53).\n"
                 );
       }
       SECTION("DUPLICATE_GOAL_RULE")
       {
          is.str(ok + gr);
          auto result { parser.parseKnowledgeBase() };
          REQUIRE(result.has_value() == false);
          REQUIRE(err.str() ==
                  "Duplicate GOAL rule: "
                  "(3,1 - 3,24) and (37,1 - 37,24).\n"
                 );
       }
       SECTION("DUPLICATE_LEGAL_ANSWERS_RULE")
       {
          is.str(ok + la);
          auto result { parser.parseKnowledgeBase() };
          REQUIRE(result.has_value() == false);
          REQUIRE(err.str() ==
                  "Duplicate LEGALANSWERS rule: "
                  "(1,1 - 1,27) and (37,1 - 37,27).\n"
                 );
       }
       SECTION("DUPLICATE_IF_RULE")
       {
          is.str(ok + i3);
          auto result { parser.parseKnowledgeBase() };
          REQUIRE(result.has_value() == false);
          REQUIRE(err.str() ==
                  "Duplicate IF rule: "
                  "(31,1 - 35,36) and (37,1 - 41,36).\n"
                 );
       }
       SECTION("DUPLICATE_QUESTION_RULE")
       {
          is.str(q4 + ok);
          auto result { parser.parseKnowledgeBase() };
          REQUIRE(result.has_value() == false);
          REQUIRE(err.str() ==
                  "Duplicate QUESTION rule: "
                  "(1,1 - 2,44) and (19,1 - 20,44).\n"
                 );
       }
   }
}
