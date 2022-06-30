#ifndef EC_INFERENCEENGINE_HPP
#define EC_INFERENCEENGINE_HPP

#include "ec_knowledgebase.hpp"
#include <stack>
#include <map>
#include <string>
#include "ec_interface.hpp"

namespace ec_inferenceengine
{
   class InferenceEngine
   {
      public:
         InferenceEngine(const ec_knowledgebase::KnowledgeBase& kb,
                         ec_interface::Interface& interface,
                         bool tracing = false);
         void run();
      private:
         void define(const std::string& var);
         [[nodiscard]] bool isDefined(const std::string& var) const;
         [[nodiscard]] const std::string& 
            value(const std::string& var) const;
         void assign(const std::string& var,const std::string& val);
         [[nodiscard]] bool isAssigned(const std::string& var) const;
         [[nodiscard]] bool success() const;
         [[nodiscard]] bool ifRulesAvailable();
         [[nodiscard]] bool questionAvailable() const;
         void ask();
         [[nodiscard]] bool isApplicable(const ec_rules::IfRule& ifRule) const;
         [[nodiscard]] bool isReady(const ec_rules::IfRule& ifRule) const;
         [[nodiscard]] bool fire(const ec_rules::IfRule& ifRule);
         [[nodiscard]] bool prepare(const ec_rules::IfRule& ifRule);
         const ec_rules::IfRule& topIfRule() const;
         void removeTopIfRule();
         void removeIfRules(const std::string& var);
         std::stack<std::string> m_stack;
         std::vector<ec_rules::IfRule> m_ifRules;
         std::map<std::string,std::string> m_variables;
         ec_knowledgebase::KnowledgeBase m_kb;
         ec_interface::Interface& m_interface;
         bool m_tracing;
   };
}

#endif // EC_INFERENCEENGINE_HPP
