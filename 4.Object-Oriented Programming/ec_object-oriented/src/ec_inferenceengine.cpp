/*
   Important note
   Tracing produces different output with respect to which 
   variable is currently being attempted to bind. Although 
   this information is only output at a single point in the 
   inference process, the outputs are more extensive than 
   in the original. The reasons for this have not yet been 
   found.
*/
#include "ec_inferenceengine.hpp"
#include <exception>
#include <fmt/format.h>
#include <vector>
#include <iostream>
#include <algorithm>

namespace ec_inferenceengine
{
   using namespace std;
   using namespace ec_knowledgebase;
   using namespace ec_rules;
   using fmt::format;
   using namespace ec_interface;

#define IE_DEBUG
#undef IE_DEBUG

   constexpr string_view
      ERROR_INTERNAL
         { "Internal error in {}." },
      ERROR_UNDEFINED_VARIABLE 
         { "Undefined variable: {}" },
      ERROR_VARIABLE_ALREADY_ASSIGNED
         { "The variable {} is already assigned the value -{}-.-{}-" },
      ERROR_VARIABLE_NOT_ASSIGNED
         { "Variable not assigned: {}" },
      ERROR_VARIABLE_ALREADY_DEFINED
         { "Variable already defined: {}" },
      ERROR_VARIABLE_ALREADY_PUSHED
         { "Variable {} is already on top of variable stack." };
   const string 
      DEAD_END_VARIABLE
         { "\"\tImpossible Value\t\"" },
      ERROR_CANNOT_ASSIGN_EMPTY_STRING
         { "Cannot assign empty string." },
      ERROR_IF_RULE_NOT_APPLICABLE
         { "If rule must be applicable!" },
      ERROR_IF_RULE_NOT_READY
         { "If rule must be ready!" },
      ERROR_KNOWLEDGEBASE_NOT_READY
         { "Knowledgebase not ready!" };

   InferenceEngine::InferenceEngine(const KnowledgeBase& kb,
                                    Interface& interface,
                                    bool tracing
                                   )
      :m_kb(kb),
       m_interface { interface },
       m_tracing { tracing }
   {
      if (!m_kb.isReady())
      {
         throw logic_error(ERROR_KNOWLEDGEBASE_NOT_READY);
      }
      define(kb.goalVariable());
      m_stack.push(kb.goalVariable());
   }
   void InferenceEngine::define(const string& var)
   {
      if (isDefined(var))
      {
         throw logic_error(format(ERROR_VARIABLE_ALREADY_DEFINED,var));
      }
      m_variables[var] = "";
   }
   bool InferenceEngine::isDefined(const string& var) const
   {
      if (m_variables.find(var) != m_variables.end())
      {
         return true;
      };
      return false;
   }
   const string& InferenceEngine::value(const string& var) const
   {
      if (!isAssigned(var))
      {
         throw logic_error(format(ERROR_VARIABLE_NOT_ASSIGNED,var));
      };
      return m_variables.find(var)->second;
   }
   bool InferenceEngine::isAssigned(const string& var) const
   {
      if (!isDefined(var))
      {
         throw logic_error(format(ERROR_UNDEFINED_VARIABLE,var));
      }
      if (m_variables.find(var)->second == "")
      {
         return false;
      }
      return true;
   }
   void InferenceEngine::assign(const string& var,const string& val)
   {
      if (!isDefined(var))
      {
         throw logic_error(format(ERROR_UNDEFINED_VARIABLE,var));
      };
      if (isAssigned(var))
      {
         throw logic_error(format(ERROR_VARIABLE_ALREADY_ASSIGNED,
                                  var,value(var),val));
      }
      if (val == "")
      {
         throw logic_error(ERROR_CANNOT_ASSIGN_EMPTY_STRING);
      }
      m_variables[var] = val;
   }
   bool InferenceEngine::success() const
   {
      if (isDefined(m_kb.goalVariable()) && isAssigned(m_kb.goalVariable()))
      {
         return true;
      }
      return false;
   }
#ifdef IE_DEBUG
   void output(const IfRule& rule,ostream& os = cout)
   {
      os << "if";
      auto counter { rule.conditions().size() };
      for (const auto& c : rule.conditions())
      {
         os << ' '
            << c.first
            << " is "
            << c.second;
         --counter;
         if (counter != 0)
         {
            os << " and";
         }
      }
      os << " then "
         << rule.conclusion().first
         << " is "
         << rule.conclusion().second;
   }
   void output(const vector<IfRule>& rules,ostream& os = cout)
   {
      for (const auto& r : rules)
      {
         output(r,os);
         os << endl;
      }
   }
   void output(const map<string,string>& variables,ostream& os = cout)
   {
      for (const auto& v : variables)
      {
         os << v.first 
            << " = "
            << v.second
            << endl;
      }
   }
#endif
   const IfRule& InferenceEngine::topIfRule() const
   {
      return m_ifRules.at(m_ifRules.size() -1);
   }
   void InferenceEngine::removeTopIfRule()
   {
      m_ifRules.erase(m_ifRules.end() - 1);
   }
   bool InferenceEngine::prepare(const IfRule& ifRule) 
   {
      // Prepares rule for further processing.
      // If all condition variables are already defined
      // there is nothing to prepare. Therefore,
      // the method returns false.
      // If exactly one condition variable was pushed onto
      // the variable stack, it returns true.
      for (const auto& condition :ifRule.conditions())
      {
         const string& var { condition.first };
         if (!isDefined(var))
         {
            define(var);
            // Despite it should not happen,
            // pushing the same variable twice
            // is considered a logic error.
            if (m_stack.size() > 0 && m_stack.top() == var)
            {
               throw logic_error(format(ERROR_VARIABLE_ALREADY_PUSHED,var));
            }
            m_stack.push(var);
            return true;
         }
      }
      return false;
   }
   bool InferenceEngine::isApplicable(const IfRule& ifRule) const
   {
      // If the variable of the conclusion of the if rule is
      // already assigned, the if rule is not applicable, 
      // because it must not be fired.
      if (isDefined(ifRule.conclusion().first) && 
          isAssigned(ifRule.conclusion().first))
      {
         return false;
      }
      for (const auto& condition :ifRule.conditions())
      {
         const string& var { condition.first };
         const string& val { condition.second };
         if (isDefined (var) && isAssigned(var))
         {
            // If the variable of any condition has a different
            // value than stated in the condition, the if rule
            // never can fire. Therefore, it is not applicable.
            if (value(var) != val)
            {
               return false;
            }
         }
      }
      return true;
   }
   bool InferenceEngine::isReady(const IfRule& ifRule) const
   {
      // Returns true, if all variables of a condition
      // are defined and assigned.
      // Important: isReady() does not imply isApplicable(),
      // because a variable can have a value which is not
      // equal to the expected value.
      // To enforece this, it is checked dynamically.
      if (!isApplicable(ifRule))
      {
         throw logic_error(ERROR_IF_RULE_NOT_APPLICABLE);
      }
      for (const auto& condition :ifRule.conditions())
      {
         const string& var { condition.first };
         const string& val { condition.second };
         if (!(isDefined(var) && isAssigned(var)))
         {
            return false;
         }
      }
      return true;
   }
   bool InferenceEngine::fire(const IfRule& ifRule)
   {
      // Returns true if all variables have values that
      // are equal to the expected values.
      // Before returning true, the conclusion is executed.
      // Important: fire() does not imply isReady().
      // To enforce this, it is checked dynamically.
      if (!isReady(ifRule))
      {
         throw logic_error(ERROR_IF_RULE_NOT_READY);
      }
      for (const auto& condition : ifRule.conditions())
      {
         if (value(condition.first) != condition.second)
         {
            return false;
         }
      }
      assign(ifRule.conclusion().first,ifRule.conclusion().second);
      if (m_tracing)
      {
         m_interface.variableAssigned(ifRule.conclusion().first,
                                      ifRule.conclusion().second);
      }
      removeIfRules(m_stack.top());
      m_stack.pop();
      return true;
   }
   bool InferenceEngine::questionAvailable() const
   {
      if (m_stack.empty())
      {
         return false;
      }
      auto result { m_kb.questionText(m_stack.top()) };
      if (result.has_value() == false)
      {
         return false;
      }
      return true;
   }
   void InferenceEngine::ask()
   {
      auto& var { m_stack.top() };
      auto val { m_interface.askQuestion(m_kb.questionText(var).value(),
                                         m_kb.legalAnswers()) };
      assign(var,val);
      if (m_tracing)
      {
         m_interface.variableAssigned(m_stack.top(),
                                      value(m_stack.top()));
      }
      removeIfRules(m_stack.top());
      m_stack.pop();
   }
   bool InferenceEngine::ifRulesAvailable()
   {
      // If the stack is empty, rules must not be searched,
      // therefore no rules are available.
      if (m_stack.empty())
      {
         return false;
      }
      // If the search result is empty, no rules are available
      auto ifRules { m_kb.ifRules(m_stack.top()) };
      if (ifRules.size() == 0)
      {
         return false;
      }
      bool found { false };
      reverse(ifRules.begin(),ifRules.end());
      // Only add rules which can be principally applied now.
      for (const auto& ifRule : ifRules)
      {
         if (isApplicable(ifRule))
         {
#ifdef IE_DEBUG
             cout << "ifRulesAvailable(), applicable: ";
             output(ifRule);
             cout << endl;
#endif// IE_DEBUG
             m_ifRules.push_back(ifRule); 
             found = true;
         }
#ifdef IE_DEBUG
         else 
         { 
            cout << "ifRulesAvailable(), rejected: ";
            output(ifRule);
            cout << endl;
         }
#endif// IE_DEBUG
      }
      // If no applicable rules could be added,
      // no new applicable rules were found.
      if (found == false)
      {
         return false;
      }
      return true;
   }
   void InferenceEngine::removeIfRules(const string& var)
   {
      // ifi = ifRuleIterator
      for (auto ifi { m_ifRules.begin() };ifi != m_ifRules.end();)
      {
         if (ifi->conditionUses(var))
         {
            ifi = m_ifRules.erase(ifi);
         }
         else
         {
           ++ifi;
         }
      } /*!< Taken from
             https://en.cppreference.com/w/cpp/container/vector/erase */
   }
   void InferenceEngine::run()
   {
      bool iterate { true };
      do
      {
         if (!m_stack.empty() && isAssigned(m_stack.top()))
         {
            m_stack.pop();
         }
         if (m_tracing)
         {
            m_interface.lookingForVariable(m_stack.top());
         }
         bool rules { ifRulesAvailable() };
         bool question { questionAvailable() };
         if (!rules && question)
         {
            ask();
         }
         if (rules)
         {
            auto x = prepare(topIfRule());
            if (isReady(topIfRule()) && !fire(topIfRule()))
            {
               removeTopIfRule();
            }
         }
         if (success())
         {
            iterate = false;
         }
         if (rules == false && question == false)
         {
            if (m_stack.size() == 1)
            {
               iterate = false;
            }
            else
            {
#ifdef IE_DEBUG
            cout << "run(), dead end variable found: "
                 << m_stack.top()
                 << endl;
#endif //IE_DEBUG
             assign(m_stack.top(),DEAD_END_VARIABLE);
            }
         }
#ifdef IE_DEBUG
         {
            cout << "run()\n"
                 << boolalpha
                 << "rules found: "
                 << rules
                 << endl
                 << "question found: "
                 << question
                 << endl
                 << "stack size: "
                 << m_stack.size()
                 << endl
                 << "top of stack: "
                 << (m_stack.size() > 0 ? m_stack.top() : "EMPTY")
                 << endl
                 << "#if rules: "
                 << m_ifRules.size()
                 << endl;
            output(m_ifRules);
            output(m_variables);
            string s;
            getline(cin,s);
         }
#endif // IE_DEBUG
      } while (iterate);
      if (success())
      {
         m_interface.success(m_kb.answerText(),value(m_kb.answerVariable()));
      }
      else 
      {
         m_interface.failed(m_kb.goalVariable(),m_kb.answerText());
      }
   }
}
