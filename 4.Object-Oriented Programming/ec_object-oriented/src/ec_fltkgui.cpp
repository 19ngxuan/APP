#include "ec_fltkgui.hpp"
#include <utility>
#include <string>

namespace ec_interface
{
   using namespace std;

   FltkGUI::FltkGUI(CallBackFunction ask_cb,
                    CallBackFunction success_cb,
                    CallBackFunction failed_cb,
                    CallBackFunction looking_cb,
                    CallBackFunction assigned_cb,
                    string& answer
                   ): m_ask_cb {  ask_cb },
                      m_success_cb { success_cb },
                      m_failed_cb { failed_cb },
                      m_looking_cb { looking_cb },
                      m_assigned_cb { assigned_cb },
                      m_answer { answer }
   {
   }
   string FltkGUI::askQuestion(const string& text,
                               const set<string>& legalAnswers) const
   {
      pair<string,set<string>> data { text,legalAnswers };
      m_ask_cb(nullptr,&data);
      return m_answer;
   }
   void FltkGUI::success(const string& text,
                         const string& value) const
   {
      pair<string,string> data { text, value };
      m_success_cb(nullptr,&data);
   }
   void FltkGUI::failed(const string& var,const string& text) const 
   {
      pair<string,string> data { var, text };
      m_failed_cb(nullptr,&data);
   }
   void FltkGUI::lookingForVariable(const string& var) const
   {
      auto nonConstString { var };
      m_looking_cb(nullptr,&nonConstString);
   }
   void FltkGUI::variableAssigned(const string& var,const string& value) const
   {
      pair<string,string> data { var, value };
      m_assigned_cb(nullptr,&data);
   }
}
