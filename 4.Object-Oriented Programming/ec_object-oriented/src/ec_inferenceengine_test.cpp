#include "ec_inferenceengine.hpp"
#include <exception>
#include <sstream>
#include "ec_tokenizer.hpp"
#include "ec_parser.hpp"
#include "ec_interface.hpp"
#include "ec_console.hpp"

TEST_CASE("Inference","[ec_inference]")
{
   using namespace std;
   using namespace ec_inferenceengine;
   using namespace ec_knowledgebase;
   using namespace ec_tokenizer;
   using namespace ec_parser;
   using namespace ec_interface;

   istringstream input;
   ostringstream output;
   Console con(input,output);

   SECTION("ERROR_KNOWLEDGEBASE_NOT_READY")
   {
      KnowledgeBase kb;
      REQUIRE(kb.isReady() == false);
      REQUIRE_THROWS_AS(InferenceEngine(kb,con,false),logic_error);
      REQUIRE_THROWS_WITH(InferenceEngine(kb,con,false),
                          "Knowledgebase not ready!");
      /*!< Normally, one would expect to write "Inference inf(kb ...) ..."
       * in each of the two lines above. However, this will result in an error.
       * According to https://github.com/catchorg/Catch2/issues/798 
       * (accessed on March 16, 2022), the solution is to call the destructor
       * without declaring a variable. 
       */
   }
   /* 
    * Next, the string exemplary_kb is initialized with a raw string literal.
    */
   string exemplary_kb { R"(
      LEGALANSWERS ARE false true unknown *

      GOAL IS logical.operator

      ANSWER IS "The logical operator is " logical.operator

      QUESTION true.op.true
      IS "What results in true linked to true? "

      QUESTION true.op.false
      IS "What results in true linked to false? "

      QUESTION false.op.true
      IS "What results in false linked to true? "

      QUESTION false.op.false
      IS "What results in false linked to false? "

      IF true.op.true IS true
      AND true.op.false IS false
      AND false.op.true IS false
      AND false.op.false IS false
      THEN logical.operator IS logical_and

      IF true.op.true IS true
      AND true.op.false IS true
      AND false.op.true IS true
      AND false.op.false IS false
      THEN logical.operator IS logical_or

      IF true.op.true IS false
      AND true.op.false IS true
      AND false.op.true IS true
      AND false.op.false IS false
      THEN logical.operator IS logical_xor

      IF true.op.true IS false
      AND true.op.false IS true
      AND false.op.true IS true
      AND false.op.false IS true
      THEN logical.operator IS logical_nand

      IF true.op.true IS unknown
      AND true.op.false IS true
      AND false.op.true IS true
      AND false.op.false IS false
      THEN logical.operator IS logical_xor_or_logical_nand

      IF true.op.true IS false
      AND true.op.false IS true
      AND false.op.true IS true
      AND false.op.false IS unknown
      THEN logical.operator IS logical_or_or_logical_xor
   )" };
   
   istringstream is;
   is.str(exemplary_kb);
   Tokenizer tokenizer(is);
   ostringstream err;
   Parser parser(tokenizer,err);
   auto result { parser.parseKnowledgeBase() };
   REQUIRE(result.has_value() == true);
   KnowledgeBase kb { result.value() };
   REQUIRE(kb.isReady() == true);
   InferenceEngine inf(kb,con);
   SECTION("failed")
   {
      input.str("true\ntrue\ntrue\ntrue\n");
      inf.run();
      REQUIRE(output.str() == 
         "What results in true linked to true? \n"
         "What results in true linked to false? \n" 
         "What results in false linked to true? \n" 
         "What results in false linked to false? \n" 
         "Error in Knowledge Base.\n"
         "LOGICAL.OPERATOR searched for but not found.\n"
         "The logical operator is UNKNOWN\n\n");
      cout << output.str() << endl;
   }
   SECTION("success")
   {
     input.str("unknown\ntrue\ntrue\nfalse\n");
     inf.run();
     REQUIRE(output.str() == 
        "What results in true linked to true? \n"
        "What results in true linked to false? \n"
        "What results in false linked to true? \n"
        "What results in false linked to false? \n"
        "The logical operator is LOGICAL_XOR_OR_LOGICAL_NAND\n\n");
   }
   /*!
    * Testing private methods.
    */
/*   SECTION("private methods & attributes")
   {
      REQUIRE(inf.m_tracing == false);
      REQUIRE(inf.m_ifRules.size() == 0);
      REQUIRE(inf.m_stack.top() == inf.m_kb.goalVariable());
      REQUIRE(inf.isDefined(inf.m_kb.goalVariable()) == true);
      REQUIRE((inf.m_variables.find(inf.m_kb.goalVariable())->first ==
         inf.m_kb.goalVariable()) == true);
      REQUIRE(inf.m_variables.find(inf.m_kb.goalVariable())->second == "");
      REQUIRE(inf.isAssigned(inf.m_kb.goalVariable()) == false);
      REQUIRE_THROWS_AS(inf.value(inf.m_kb.goalVariable()),logic_error);
      REQUIRE_THROWS_WITH(inf.value(inf.m_kb.goalVariable()),
         "Variable not assigned: LOGICAL.OPERATOR");
      REQUIRE_THROWS_AS(inf.define(inf.m_kb.goalVariable()),logic_error);
      REQUIRE_THROWS_WITH(inf.define(inf.m_kb.goalVariable()),
         "Variable already defined: LOGICAL.OPERATOR");
      REQUIRE(inf.success() == false);
      REQUIRE(inf.m_stack.size() == 1);
      REQUIRE(inf.isDefined("TRUE.OP.TRUE") == false);
      REQUIRE_THROWS_AS(inf.isAssigned("TRUE.OP.TRUE"),logic_error);
      REQUIRE_THROWS_WITH(inf.isAssigned("TRUE.OP.TRUE"),
         "Undefined variable: TRUE.OP.TRUE");
      REQUIRE_THROWS_AS(inf.assign("TRUE.OP.TRUE","FALSE"),logic_error);
      REQUIRE_THROWS_WITH(inf.assign("TRUE.OP.TRUE","FALSE"),
         "Undefined variable: TRUE.OP.TRUE");
      REQUIRE(inf.isDefined("ALIEN.VAR") == false);
      inf.define("ALIEN.VAR");
      inf.assign("ALIEN.VAR","ALIEN.VALUE");
      REQUIRE(inf.ifRulesAvailable() == true);
      REQUIRE(inf.m_ifRules.size() == 6);
      REQUIRE(inf.questionAvailable() == false);
      inf.prepare(inf.m_ifRules.at(0));
      REQUIRE(inf.m_stack.size() == 2);
      REQUIRE(inf.m_stack.top() == "TRUE.OP.TRUE");
      REQUIRE(inf.ifRulesAvailable() == false);
      REQUIRE(inf.questionAvailable() == true);
      // Too laborious to test:
      // - success()
      // - isApplicable()
      // - isReady()
      // - fire()
      // - prepare()
      // - topIfRule()
      // - removeTopIfRule()
      // - removeIfRules()
   }*/
}
