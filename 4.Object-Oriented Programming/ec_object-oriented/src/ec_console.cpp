#include "ec_console.hpp"
#include "ec_helper.hpp"

namespace ec_interface
{
   using namespace std;
   using namespace ec_helper;

   Console::Console(istream& is,ostream& os):m_is { is },
                                             m_os {os }
   {
   }
   string Console::askQuestion(const string& text,
                               const set<string>& legalAnswers) const
   {
      string answer;
      do
      {
         m_os << text
              << endl;
         getline(m_is,answer);
         answer = toupper(answer);
         if (!legalAnswers.contains(answer))
         {
            cout << "I'm sorry, but your answer is not acceptable.\n"s
                 << "Please be sure you are typing the answer you "s  
                 << "want fully and correctly,\n"s
                 << "and please choose your answer from one of these:"s
                 << endl;
            for (const auto& answer : legalAnswers)
            {
               cout << answer << endl;
            }
         }
      } while (!legalAnswers.contains(answer));
      return answer;
   }
   void Console::success(const string& text,
                         const string& value) const
   {
      m_os << text 
           << value 
           << "\n"
           << endl;
   }
   void Console::failed(const string& var,const string& text) const 
   {
      m_os << "Error in Knowledge Base.\n"
           << var 
           <<" searched for but not found.\n"
           << text
           << "UNKNOWN\n"
           << endl;
   }
   void Console::lookingForVariable(const string& var) const
   {
      m_os << "Currently looking for: "
           << var
           << '.'
           << endl;
   }
   void Console::variableAssigned(const string& var,const string& value) const
   {
      m_os << "It has been learned that "
           << var
           << '\n'
           << "is "
           << value
           << '.'
           << endl;
   }
}
