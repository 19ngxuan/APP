#ifndef EC_FLTKGUI_HPP
#define EC_FLTKGUI_HPP

#include <string>
#include <set>
#include <iostream>
#include "ec_interface.hpp"
#include <FL/Fl_Widget.H>

namespace ec_interface
{
   using CallBackFunction = void (*)(Fl_Widget *,void *);

   class FltkGUI: public ec_interface::Interface
   {
      public:
         FltkGUI(CallBackFunction ask_cb,
                 CallBackFunction success_cb,
                 CallBackFunction failed_cb,
                 CallBackFunction looking_cb,
                 CallBackFunction assigned_cbi,
                 std::string& answer
                 );
         [[nodiscard]] virtual std::string 
            askQuestion(const std::string& text,
                        const std::set<std::string>& legalAnswers) const override;
         virtual void success(const std::string& text,
                              const std::string& value) const override;
         virtual void failed(const std::string& var,
                             const std::string& text) const override;
         virtual void lookingForVariable(const std::string& var) const override;
         virtual void variableAssigned(const std::string& var,
                                       const std::string& value) const override;
      private:
         CallBackFunction m_ask_cb;
         CallBackFunction m_success_cb;
         CallBackFunction m_failed_cb;
         CallBackFunction m_looking_cb;
         CallBackFunction m_assigned_cb;
         std::string& m_answer;
   };
}
#endif // EC_FLTKGUI_HPP
