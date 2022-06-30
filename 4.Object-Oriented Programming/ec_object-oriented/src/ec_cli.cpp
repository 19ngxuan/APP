#include "ec_helper.hpp"
#include "ec_tokenizer.hpp"
#include "ec_rules.hpp"
#include "ec_knowledgebase.hpp"
#include "ec_parser.hpp"
#include "ec_inferenceengine.hpp"
#include "ec_interface.hpp"
#include "ec_console.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;
using namespace ec_helper;
using namespace ec_tokenizer;
using namespace ec_rules;
using namespace ec_knowledgebase;
using namespace ec_parser;
using namespace ec_inferenceengine;
using namespace ec_interface;

string inputCommand()
{
   string command;
   bool isKnown { false };
   do
   {
      cout << "==>"s << flush;
      getline(cin,command);
      command  = toupper(command);
      if (command == "TRACE ON"s || command == "TRACE OFF"s ||
          command == "GO"s || command == "EXIT"s)
      {
         isKnown = true;
      }
      else
      {
         error ("I don't understand that command.\n\n"s +
                "Valid options are: "s +
                "TRACE ON, TRACE OFF, GO, and EXIT."s);
      }
   } while (isKnown == false);
   return command;
}

void topLevel(const KnowledgeBase& kb)
{
   bool tracing { false };
   string command;
   do
   {
      command  = inputCommand();
      if (command == "TRACE ON"s && tracing == false)
      {
         tracing = true;
         cout << "There were "
              << to_string(kb.numberOfRuleLines())
              << " rule-lines, "
              << to_string(kb.numberOfQuestionRules())
              << " questions and "
              << to_string(kb.legalAnswers().size())
              << "\nlegal answers "
              << "specified in the knowledge base."
              << endl;
      };
      if (command == "TRACE OFF"s)
      {
         tracing = false;
      };
      if (command == "GO"s)
      {     
         Console con;
         InferenceEngine inf(kb,con,tracing);
         inf.run();
         cout << "I have completed this analysis."s << endl;
      }
   } while (command != "EXIT"s);
   cout << "Have a nice day!"s << endl;
}

int main()
{
   ifstream file { };
   do
   { 
      string name { },
             answer { };
      cout << "File name where rules found?: "s;
      getline(cin,name);
      file.open(name);
      if (!file)
      {
         cout << "File \""s << name << "\" does not exist. "s
              << "Do you wish to try again? (Y/N)"s << flush;
         do
         {
            getline(cin,answer);
            answer = toupper(answer);
         }
         while ( answer != "Y"s && answer != "N"s);
         if (answer == "N"s)
         {
            return 0;
         }
      }
   } while (!file);
   Tokenizer tok(file);
   Parser parser(tok);
   const auto result { parser.parseKnowledgeBase() };
   if (result.has_value())
   {
      topLevel(result.value());
   }
   else
   {
      cerr << "Error when parsing knowledge base, program aborted."
           << endl;
   }
}
