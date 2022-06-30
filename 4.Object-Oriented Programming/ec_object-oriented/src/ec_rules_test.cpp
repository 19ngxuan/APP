#include "ec_rules.hpp"

TEST_CASE("Rule","[ec_rules]")
{
   using namespace ec_rules;
   // Abstract class cannot be instantiated.
   // Hence, GoalRule is used for testing Rule-part
   GoalRule gRule;
   gRule.begin({0,1});
   gRule.end({2,3});
   Rule& rule { gRule };
   REQUIRE(rule.begin().first == 0);
   REQUIRE(rule.begin().second == 1);
   REQUIRE(rule.end().first == 2);
   REQUIRE(rule.end().second == 3);
   REQUIRE(rule.type() == RuleType::GOAL);
   rule.begin({4,5});
   rule.end({6,7});
   REQUIRE(rule.begin().first == 4);
   REQUIRE(rule.begin().second == 5);
   REQUIRE(rule.end().first == 6);
   REQUIRE(rule.end().second == 7);
}

TEST_CASE("AnswerRule","[ec_rules]")
{
   using namespace ec_rules;
   AnswerRule aRule;
   REQUIRE(aRule.type() == RuleType::ANSWER);
   REQUIRE(aRule.variable("VAR") == true);;
   REQUIRE(aRule.variable() == "VAR");
   REQUIRE(aRule.text("\"Some text\"") == true);
   REQUIRE(aRule.text() == "\"Some text\"");
}

TEST_CASE("GoalRule","[ec_rules]")
{
   using namespace ec_rules;
   GoalRule gRule;
   REQUIRE(gRule.variable("NAME") == true);
   REQUIRE(gRule.variable() == "NAME");
   REQUIRE(gRule.type() == RuleType::GOAL);
}

TEST_CASE("IfRule","[ec_rules]")
{
   using namespace ec_rules;
   IfRule ifRule;
   REQUIRE(ifRule.type() == RuleType::IF);
   REQUIRE(ifRule.conditions().size() == 0);
   REQUIRE(ifRule.size() == 0);
   REQUIRE(ifRule.conclusion().first == "");
   REQUIRE(ifRule.conclusion().second == "");
   REQUIRE(ifRule.isAbout("XYZ") == false);
   REQUIRE(ifRule.isValid() == false);
   REQUIRE(ifRule.condition("VARIABLE","VALUE") == true);
   REQUIRE(ifRule.size() == 1);
   REQUIRE(ifRule.conditions().size() == 1);
   REQUIRE(ifRule.condition("VARIABLE","VALUE") == false);
   REQUIRE(ifRule.condition("ANOTHER_VAR","ANOTHER_VAL") == true);
   REQUIRE(ifRule.size() == 2);
   REQUIRE(ifRule.conditionUses("VARIABLE") == true);
   REQUIRE(ifRule.conditionUses("ANOTHER_VAR") == true);
   REQUIRE(ifRule.conditionUses("VALUE") == false);
   REQUIRE(ifRule.conditionUses("XYZ") == false);
   REQUIRE(ifRule.isValid() == false);
   REQUIRE(ifRule.conditions().size() == 2);
   REQUIRE(ifRule.conclusion("GOAL","RESULT") == true);
   REQUIRE(ifRule.conclusion("GOAL","RESULT") == false);
   REQUIRE(ifRule.conclusion().first == "GOAL");
   REQUIRE(ifRule.conclusion().second == "RESULT");
   REQUIRE(ifRule.isAbout("GOAL") == true);
   REQUIRE(ifRule.isValid() == true);
   REQUIRE((ifRule == ifRule) == true);
   IfRule ifRule2 { ifRule };
   REQUIRE((ifRule == ifRule2) == true);
   REQUIRE(ifRule2.condition("MORE.SPECIAL","YES") == true);
   REQUIRE((ifRule == ifRule2) == false);
}

TEST_CASE("LegalAnswersRule","[ec_rules]")
{
   using namespace ec_rules;
   LegalAnswersRule laRule;
   REQUIRE(laRule.size() == 0);
   REQUIRE(laRule.legalAnswers().size() == 0);
   REQUIRE(laRule.type() == RuleType::LEGALANSWERS);
   REQUIRE(laRule.contains("XYZ") == false);
   REQUIRE(laRule.insert("ABC") == true);
   REQUIRE(laRule.size() == 1);
   REQUIRE(laRule.legalAnswers().size() == 1);
   REQUIRE(laRule.contains("ABC") == true);
   REQUIRE(laRule.insert("ABC") == false);
}

TEST_CASE("QuestionRule","[ec_rules]")
{
   using namespace ec_rules;
   QuestionRule qRule;
   REQUIRE(qRule.variable("VAR") == true);
   REQUIRE(qRule.variable() == "VAR");
   REQUIRE(qRule.text("\"Some text\"") == true);
   REQUIRE(qRule.text() == "\"Some text\"");
   QuestionRule qRule2;
   REQUIRE(qRule2.variable("W") == true);
   REQUIRE(qRule2.variable() == "W");
   REQUIRE(qRule2.text("\"Some text\"") == true);
   REQUIRE(qRule2.text() == "\"Some text\"");
   REQUIRE((qRule < qRule2) == true);
   REQUIRE((qRule2 < qRule) == false);
}
