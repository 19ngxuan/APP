// EC.cpp Ulrich Eisenecker, August 30,2021

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <utility> // because of pair<>

using namespace std;

constexpr bool debugMode { false };

// Logger is for debugging only
class Logger
{
   public:
      Logger(const string& name,const string& arguments = ""):
         m_name { name },m_arguments { arguments }
      {
         if constexpr (debugMode)
         {
            string indentation(++m_activeFunctions,'>');
            cout << indentation << ' '
                 << m_name << ' '
                 << m_arguments << endl;
         }
      }
      ~Logger()
      {
         if constexpr (debugMode)
         {
            string indentation(m_activeFunctions--,'<');
            cout << indentation << ' '
                 << m_name << ' '
                 << m_arguments << endl;
         }
      }
   private:
      static size_t m_activeFunctions;
      const string m_name { },
                   m_arguments { };
};

// Static data member must be initialized outside the class
size_t Logger::m_activeFunctions { 0 };

// Converts lower cases to upper cases in ASCII character set
[[nodiscard]] string toupper(string s)
{
   for (char& c : s)
      c = toupper(c);
   return s;
}

class LegalAnswers
{
   public:
      [[nodiscard]] size_t size() const
      {
         return m_answers.size();
      }
      bool add(const string& answer)
      {
         m_answers.push_back(answer);
         return true;
      }
      void setActive()
      {
         m_active = true;
      }
      [[nodiscard]] bool isActive() const
      {
         return m_active;
      }
      [[nodiscard]] bool isLegal(const string& answer) const
      {
         if (!isActive())
         {
            return true;
         }
         else
         {
            return (find(m_answers.begin(),m_answers.end(),answer) != 
                    m_answers.end());
         }
      }
      void output() const
      {
         if (size() == 0)
         {
            return;
         }
         for (const auto& element : m_answers)
         {
            cout << element << endl;
         }
      }
      void output(ostream& os) const
      {
         if (size() == 0)
         {
            return;
         }
         os << "legalanswers are ";
         for (const auto& element : m_answers)
         {
            os << element << ' ';
         }
         os << "*\n" << endl;
      }
   private:
      vector<string> m_answers;
      bool m_active { false };
};

class Variables
{
   public:
      [[nodiscard]] size_t size() const
      {
         return m_variables.size();
      }
      bool add(const string& variable)
      {
         for (const auto& element : m_variables)
         {
            if (element.first == variable)
            {
               return false;
            }
         }
         m_variables.push_back(make_pair(variable,""s));
         return true;
      }
      bool set(const string& variable,const string& value)
      {
         for (auto& element : m_variables)
         {
            if (element.first == variable)
            {
               element.second = value;
               return true;
            }
         }
         return false;
      }
      [[nodiscard]] string get(const string& variable) const
      {
         for (const auto& element : m_variables)
         {
            if (element.first == variable)
            {
               return element.second;
            }
         }
         return ""s;
      }
      void reset()
      {
         for (auto& element : m_variables)
         {
            element.second = ""s;
         }
      }
      void output(ostream& os) const
      {
         for (const auto& element : m_variables)
         {
            os << element.first << " is " 
               << element.second << endl;
         }
      }
   private:
      vector<pair<string,string>> m_variables;
};

class KnowledgeBase;

class Questions
{
   public:
      [[nodiscard]] size_t size() const
      {
         return m_questions.size();
      }
      bool add(const string& variable,const string& subject)
      {
         for (const auto& element : m_questions)
         {
            if (element.first == variable)
            {
               return false;
            }
         }
         m_questions.push_back(make_pair(variable,subject));
         return true;
      }
      void output(ostream& os) const
      {
         for (const auto& element : m_questions)
         {
            os << "question " << element.first << " is\n\"" 
               << element.second << "\"\n" << endl;
         }
      }
      bool ask(KnowledgeBase& kb,const string& variable);
   private:
      vector<pair<string,string>> m_questions;
};

class Rule
{
   public:
      bool addCondition(const string& variable,const string& value)
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
      bool addConclusion(const string& variable,const string& value)
      {
         if (m_variable != ""s || m_value != ""s)
         {
            return false;
         }
         m_variable = variable;
         m_value = value;
         return true;
      }
      void output(ostream& os) const
      {
         // output if-part
         os << "IF";
         container_t::size_type count { 0 };
         for (const auto& element : m_conditions)
         {
            os << " " << element.first << " IS " << element.second;
            if (++count < m_conditions.size())
            {
               os << "\nand";
            }
         }
         // output then-part
         os << "\nthen " << m_variable << " is " << m_value << '\n' << endl;
      }
      [[nodiscard]] bool isActive() const
      {
         return m_active;
      }
      void reset()
      {
         m_active = true;
      }
      bool prove(KnowledgeBase& kb,const string& variable);
   private:
      using container_t = vector<pair<string,string>>;
      container_t m_conditions;
      string m_variable { },
             m_value { };
      bool m_active { true };
};

class Rules
{
   public:
      [[nodiscard]] size_t size() const
      {
         return m_rules.size();
      }
      // No check for verbatim or semantic duplicates!
      void add(const Rule& rule)
      {
         m_rules.push_back(rule);
      }
      void output(ostream& os) const
      {
         for (const auto& element : m_rules)
         {
            element.output(os);
         }
      }
      void reset()
      {
         for (auto& element : m_rules)
         {
            element.reset();
         }
      }
      bool prove(KnowledgeBase& kb,const string& variable);
   private:
      vector<Rule> m_rules;
};

class KnowledgeBase
{
   public:
      bool error(const string& message) const
      {
         cerr << message << endl;
         return false;
      }
      [[nodiscard]] string inputToken(istream& is) const
      {
         string token;
         is >> ws;
         if (!is)
         {
            error("Unexpected end of file encountered in rule file.");
         }
         if (is.peek() == '\"')
         {
            is.get(); // discard initial quote
            while (is && is.peek() != '\"')
            {
               token += is.get();
            }
            if (is && is.peek() ==  '\"')
            {
               is.get(); // discard closing quote
            }
            if (token.length() > 80)
            {
               token = token.substr(0,80);
            }
         }
         else
         {
            while (is && is.peek() > 32 && is.peek() != '\"')
            {
               token += toupper(is.get());
            }
            if (token.length() > 40)
            {
               token = token.substr(0,40);
            };
         };
         return token;
      }
      bool inputIsAre(istream& is) const
      {
         string isAre { inputToken(is) }; 
         if (!is && isAre == "")
         {
            return false;
         }
         // /* with checking: */ return ( s == "IS"s || s == "ARE"s);
         return true;
      }
      bool inputVariableValue(istream& is,string& variable,string& value) const
      {
         // input variable
         variable = inputToken(is);
         if (!is && variable == "")
         {
            return false;
         }
         // input is/are
         if (!inputIsAre(is))
         {
            return false;
         }
         // input value
         value = inputToken(is);
         if (!is && value == "")
         {
            return false;
         }
         return true;
      }
      bool inputLegalAnswers(istream& is)
      {
         if (m_legalAnswers.isActive())
         {
            return error("LEGALANSWERS have been specified more than once "s +
                         "in the knowledge base."s);
         }
         m_legalAnswers.setActive();
         // input is/are
         if (!inputIsAre(is))
         {
            return false;
         }
         // input legal answers including terminator *
         string answer { };
         do
         {
            answer = inputToken(is);
            if (!is && answer == "")
            {
               return false;
            }
            if (answer != "*"s)
            { 
               m_legalAnswers.add(answer);
               if (m_legalAnswers.size() > 50)
               {
                  return error("Too many legalanswers encountered in the "s +
                               "LEGALANSWERS rule."s);
               }
            };
         } while (answer != "*"s);
         return true;
      }
      bool inputGoal(istream& is)
      {
         if (m_goal != ""s)
         {
            return error("Goal encountered more than once in "s +
                         "Knowledge Base."s);
         }
         if (!inputIsAre(is))
         {
            return false;
         }
         m_goal = inputToken(is);
         if (!is && m_goal == "")
         {
            return false;
         }
         m_variables.add(m_goal);
         return true;
      }
      bool inputRule(istream& is)
      {
         string variable,
                value,
                token;
         Rule rule;
         // input if-part
         do
         {
            if (!inputVariableValue(is,variable,value))
            {
               return false;            
            }
            m_variables.add(variable);
            rule.addCondition(variable,value);
            ++m_ruleLines;
            if (m_ruleLines >= 400)
            {
               return error("There are too many rules in the Knowledge Base "s +
                            "for me."s);
            }   
            token = inputToken(is);
            if (!is && token == "")
            {
               return false;
            }
         } while (token == "AND"s);
         
         // input then-part
         // if (token == "THEN") // ESIE accepts any token here; found "the "
         // instead of "then " in "ANIMAL"
         if (token != ""s)
         {
            if (!inputVariableValue(is,variable,value))
            {
               return false;
            }
            m_variables.add(variable);
            ++m_ruleLines;
            rule.addConclusion(variable,value);
            m_rules.add(rule);
            return true;
         }
         return false;
      }
      bool inputQuestion(istream& is)
      {
         if (m_questions.size() >= 100)
         {
            return error("There are too many questions in the "s + 
                         "Knowledge Base for me."s);
         }
         // input variable
         string variable { inputToken(is) };
         if (!is && variable == "")
         {
            return false;
         }
         // input is/are
         if (!inputIsAre(is))
         {
            return false;
         }
         // input subject
         string subject { inputToken(is) };
         if (!is && subject == "")
         {
            return false;
         }
         m_variables.add(variable);
         m_questions.add(variable,subject);
         return true;
      }
      bool inputAnswer(istream& is)
      {
         if (m_answer.first != ""s || m_answer.second != ""s)
         {
            return error("Answer encountered more than once in "s +
                         "Knowledge Base."s);
         }
         if (!inputIsAre(is))
         {
            return false;
         }
         string subject { inputToken(is) };
         if (!is && subject == "")
         {
            return false;
         }
         // input variable
         string variable { inputToken(is) };
         if (!is && subject == "")
         {
            return false;
         }
         m_variables.add(variable);
         m_answer = make_pair(subject,variable);
         return true;
      }
      bool input(istream& is)
      {
         while (is)
         {
            string token { inputToken(is) };
            if (token == "LEGALANSWERS"s)
            {
               if (!inputLegalAnswers(is))
               {
                  return false;
               };
            }
            else if (token == "GOAL"s)
            {
               inputGoal(is);
            }
            else if (token == "IF"s)
            {
               inputRule(is);
            }
            else if (token == "QUESTION"s)
            {
               inputQuestion(is);
            }
            else if (token == "ANSWER"s)
            {
              inputAnswer(is);
            }
            else if (token != ""s)
            {
               error ("Invalid rule found in Knowledge Base.\n"s +
                      "Rule begins with: "s + token);
            };
         }
         bool goalAndAnswerFound { true };
         if (m_goal == "")
         {
            error ("Goal statement not found "s +
                   "in the Knowledge Base.");
            goalAndAnswerFound = false;
         }
         if (m_answer.first == "" && m_answer.second == "")
         {
            error ("Answer statement not found "s +
                   "in the Knowledge Base.");
            goalAndAnswerFound = false;         
         }
         return goalAndAnswerFound;
      }
      void output(ostream& os)
      {
         os << "goal is " << m_goal << endl << endl;
         os << "answer is " << '\"' << m_answer.first << "\" " 
            << m_answer.second << endl << endl;
         m_legalAnswers.output(os);
         os << endl;
         m_rules.output(os);
         os << endl;
         m_questions.output(os);
      }
      bool prove(const string& variable) 
      {
         Logger log("KnowledgeBase::prove",variable);
           return m_rules.prove(*this,variable) || 
                  m_questions.ask(*this,variable);
      }
      string inputCommand() const
      {
         string command;
         bool isKnown { false };
         do
         {
            cout << "==>" << flush;
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
      void run()
      {
         string command;
         do
         {
            command  = inputCommand();
            if (command == "TRACE ON"s && m_tracing == false)
            {
               m_tracing = true;
               report("There were "s + to_string(m_ruleLines) + 
                      " rule-lines, "s +
                      to_string(m_questions.size()) + " questions and "s + 
                      to_string(m_legalAnswers.size()) + "\nlegal answers "s + 
                      "specified in the knowledge base."s);
            };
            if (command == "TRACE OFF"s)
            {
               m_tracing = false;
            };
            if (command == "GO"s)
            {
               if (prove(m_goal))
               {
                  cout << m_answer.first << m_variables.get(m_answer.second) 
                       << endl << endl;
               }
               else
               {
                  cout << "Error in Knowledge Base.\n"
                       << m_goal << " searched for but not found.\n"
                       << m_answer.first << "UNKNOWN\n" << endl;
               }
               cout << "I have completed this analysis." << endl;
               m_variables.reset();
               m_rules.reset();
            }
         } while (command != "EXIT"s);
         cout << "Have a nice day!" << endl;
      }
      void report(const string& message) const 
      {
         if (m_tracing)
         {
            cout << message << endl;
         }
      }
      bool isLegalAnswer(const string& answer) const
      {
         return m_legalAnswers.isLegal(answer);
      }
      void outputLegalAnswers() const
      {
         m_legalAnswers.output();
      }
      bool setVariable(const string& variable,const string& value)
      {
         return m_variables.set(variable,value);
      }
      [[nodiscard]] string getVariable(const string& variable) const
      {
         return m_variables.get(variable);
      }
      bool askVariable(const string& variable)
      {
         return m_questions.ask(*this,variable);
      }
   private:
      string m_goal;
      pair<string,string> m_answer;
      LegalAnswers m_legalAnswers;
      Rules m_rules;
      Questions m_questions;
      Variables m_variables;
      bool m_tracing { false };
      size_t m_ruleLines { 0 };
};

int main()
{
   ifstream file { };
   do
   { 
      string name { },
             answer { };
      cout << "File name where rules found?: ";
      getline(cin,name);
      file.open(name.c_str());
      if (!file)
      {
         cout << "File \"" << name << "\" does not exist. "
              << "Do you wish to try again? (Y/N)" << flush;
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
   KnowledgeBase kb;
   if (kb.input(file))
   {
      kb.run();
   }
}

bool Questions::ask(KnowledgeBase& kb,const string& variable)
{
   Logger log("Questions::ask",variable);
   for (auto question : m_questions)
   {
      if (question.first == variable)
      {
         string answer { };
         bool ok { false };;
         do
         {
            cout << question.second << endl;
            getline(cin,answer);
            answer = toupper(answer);
            if (kb.isLegalAnswer(answer))
            {
               kb.setVariable(question.first,answer);
               kb.report("It has been learned that "s + question.first +
                         "\nis "s + answer + "."s);
               ok = true;
            }
            else
            {
               cout << "I'm sorry, but your answer is not acceptable.\n"
                    << "Please be sure you are typing the abswer you "s  
                    << "want fully and correctly,\n"
                    << "and please choose your answer from on of these:"
                    << endl;
               kb.outputLegalAnswers();
            }
         } while (!ok);
         return true;
      }
   }
   return false;
}

bool Rule::prove(KnowledgeBase& kb,const string& variable) 
{
   Logger log("Rule::prove",variable);
   if (variable != m_variable)
   {
      return false;
   }
   for (const auto& condition : m_conditions)
   {
      string value { kb.getVariable(condition.first) };
      if (value == ""s)
      {
         if (!kb.prove(condition.first) && !kb.askVariable(condition.first))
         {
            return false;
         }
         value = kb.getVariable(condition.first);
         kb.report("Currently looking for: "s + variable + "."s);
      }
      if (value != condition.second)
      {
         return false;
      }
   }
   kb.setVariable(m_variable,m_value);
   kb.report("Currently looking for: "s + variable + "."s);
   kb.report("It has been learned that "s + m_variable +
             "\nis " + m_value + ".");
   m_active = false;
   return true;
}

bool Rules::prove(KnowledgeBase& kb,const string& variable)
{
   Logger log("Rules::prove",variable);
   kb.report("Currently looking for: "s + variable + "."s);
   for (auto& rule : m_rules)
   {
      if (rule.isActive())
      {   
         if (rule.prove(kb,variable))
         {
            return true;
         }
      }
   }
   return false;
}
