#include "ec_knowledgebase.hpp"
#include <exception>

TEST_CASE("KnowledgeBase","[ec_knowledgebase]")
{
   using namespace ec_knowledgebase;
   using namespace ec_rules;
   using namespace std;

   KnowledgeBase kb;
   REQUIRE(kb.isReady() == false);
   // Everything related to AnswerRule
   REQUIRE(kb.answerRulePresent() == false);
   REQUIRE_THROWS_AS(kb.answerText(),logic_error);
   REQUIRE_THROWS_WITH(kb.answerText(),"Answer rule not present!");
   REQUIRE_THROWS_AS(kb.answerVariable(),logic_error);
   REQUIRE_THROWS_WITH(kb.answerVariable(),"Answer rule not present!");
   AnswerRule aRule;
   REQUIRE(aRule.variable("ANSWER") == true);
   REQUIRE(aRule.text("This is the answer") == true);
   REQUIRE(kb.insert(aRule).has_value() == false);
   REQUIRE(kb.answerRulePresent() == true);
   REQUIRE(kb.insert(aRule).has_value() == true);
   REQUIRE(kb.answerRulePresent() == true);
   REQUIRE(kb.answerText() == "This is the answer");
   REQUIRE(kb.answerVariable() == "ANSWER");
   REQUIRE(kb.isReady() == false);
   // Everything related to GoalRule
   REQUIRE(kb.goalRulePresent() == false);
   GoalRule gRule;
   REQUIRE_THROWS_AS(kb.goalVariable(),logic_error);
   REQUIRE_THROWS_WITH(kb.goalVariable(),"Goal rule not present!");
   REQUIRE(gRule.variable("THE_GOAL") == true);
   REQUIRE(kb.insert(gRule).has_value() == false);
   REQUIRE(kb.goalRulePresent() == true);
   REQUIRE(kb.goalVariable() == "THE_GOAL");
   REQUIRE(kb.insert(gRule).has_value() == true);
   REQUIRE(kb.isReady() == false);
   // Everything related to IfRule
   REQUIRE(kb.numberOfIfRules() == 0);
   REQUIRE(kb.numberOfRuleLines() == 0);
   REQUIRE(kb.ifRules("X").size() == 0);
   IfRule ifRule1;
   REQUIRE(ifRule1.condition("IS.ADULT","YES") == true);
   REQUIRE(ifRule1.condition("CAN.PAY","YES") == true);
   REQUIRE(ifRule1.conclusion("SELL.CIGARETTES","YES") == true);
   REQUIRE(kb.insert(ifRule1).has_value() == false);
   REQUIRE(kb.numberOfRuleLines() == 3);
   REQUIRE((kb.insert(ifRule1) == ifRule1) == true);
   REQUIRE(kb.numberOfIfRules() == 1);
   REQUIRE(kb.ifRules("SELL.CIGARETTES").size() == 1);
   IfRule ifRule2 { ifRule1 };
   REQUIRE(ifRule2.condition("LOOKS.HEALTHY","YES"));
   REQUIRE(kb.insert(ifRule2).has_value() == false);
   REQUIRE(kb.numberOfIfRules() == 2);
   REQUIRE(kb.ifRules("SELL.CIGARETTES").size() == 2);
   auto result { kb.ifRules("SELL.CIGARETTES") };
   REQUIRE((result.at(0) == result.at(1)) == false);
   REQUIRE(kb.isReady() == false);
   REQUIRE(kb.numberOfRuleLines() == 7);
   // Everything related to LegalAnswersRule
   REQUIRE(kb.numberOfLegalAnswers() == 0);
   LegalAnswersRule laRule;
   REQUIRE(laRule.insert("YES") == true);
   REQUIRE(laRule.insert("NO") == true);
   REQUIRE(kb.insert(laRule).has_value() == false);
   REQUIRE(kb.numberOfLegalAnswers() == 2);
   REQUIRE(kb.legalAnswers().contains("NO"));
   REQUIRE(kb.legalAnswers().contains("YES"));
   REQUIRE(kb.isLegalAnswer("YES") == true);
   REQUIRE(kb.isLegalAnswer("NO") == true);
   REQUIRE(kb.isLegalAnswer("XYZ") == false);
   REQUIRE(kb.insert(laRule).has_value() == true);
   REQUIRE(kb.numberOfLegalAnswers() == 2);
   REQUIRE(kb.legalAnswers().contains("NO"));
   REQUIRE(kb.legalAnswers().contains("YES"));
      REQUIRE(kb.isLegalAnswer("YES") == true);
   REQUIRE(kb.isLegalAnswer("NO") == true);
   REQUIRE(kb.isReady() == false);
   // Everything related to QuestionRule
   REQUIRE(kb.numberOfQuestionRules() == 0);
   QuestionRule qRule;
   REQUIRE(kb.questionText("X").has_value() == false);
   REQUIRE(kb.questionText("").has_value() == false);
   REQUIRE(qRule.variable("X") == true);
   REQUIRE(qRule.text("5") == true);
   REQUIRE(kb.insert(qRule).has_value() == false);
   REQUIRE(kb.numberOfQuestionRules() == 1);
   REQUIRE(kb.insert(qRule).has_value() == true);
   REQUIRE(kb.numberOfQuestionRules() == 1);
   QuestionRule qRule2;
   REQUIRE(qRule2.variable("X") == true);
   REQUIRE(qRule2.text("6") == true);
   REQUIRE(kb.insert(qRule2).has_value() == true);
   REQUIRE(kb.questionText("X").has_value() == true);
   REQUIRE(kb.questionText("X") == "5");
   REQUIRE(kb.questionText("Y").has_value() == false);
   REQUIRE(kb.isReady() == true);
}
