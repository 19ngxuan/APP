#include <string>
#include <fstream>
#include <sstream>
#include <optional>
#include <utility>
#include <set>
#include <vector>
#include <cstdio>
using namespace std;

#include "ec_inferenceengine.hpp"
#include "ec_knowledgebase.hpp"
#include "ec_tokenizer.hpp"
#include "ec_parser.hpp"
#include "ec_inferenceengine.hpp"
#include "ec_fltkgui.hpp"
using namespace ec_inferenceengine;
using namespace ec_knowledgebase;
using namespace ec_tokenizer;
using namespace ec_parser;
using namespace ec_interface;

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_ask.H> // For specifiers in format-strings see
                       // https://www.cplusplus.com/reference/cstdio/printf/
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_File_Chooser.H>
#include "ec_icon.xpm"
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_Input_Choice.H>

// Some global variables
const string EC_NAME { "EC with GUI" };
const string EC_VERSION { "1.0" };
const int fmtBufferSize { 255 };
char fmtBuffer[fmtBufferSize + 1];
string kbFullName { };
bool tracing { false };
Fl_Window* window { nullptr };
Fl_Menu_Bar* menue { nullptr };
Fl_Text_Display* display { nullptr };
Fl_Text_Buffer* buffer { nullptr };
Fl_Help_Dialog* help { nullptr };
int startConsultationIndex { -1 };
Tokenizer *tokenizer { nullptr };
Parser *parser { nullptr };
KnowledgeBase kb;
ifstream input;
ostringstream error;
string answer { };

[[nodiscard]] string fileNameOnly(const string& fullName)
{
   auto pos {fullName.rfind('/') };
   if (pos == string::npos)
   {
      return fullName;
   }
   return fullName.substr(pos + 1); // 2nd parameter len 
                                    // defaults to string::npos
}

void adjustTextDisplay()
{
   auto lines { buffer->count_lines(0,(~0 >> 1)) };
   display->scroll((lines > 5 ? lines - 5 : lines),1);
   window->show();
}

void failed_cb(Fl_Widget* w,void* v)
{
   adjustTextDisplay();
   pair<string,string> *data { static_cast<pair<string,string>*>(v) };
   char fmtString[] 
      { "Error in Knowledge Base.\n"
        "%s searched for but not found.\n"
        "%s UNKNOWN\n"
      };
   snprintf(fmtBuffer,fmtBufferSize,fmtString,
            data->first.c_str(),data->second.c_str());
   buffer->append(fmtBuffer);
}

void success_cb(Fl_Widget* w,void* v)
{
   adjustTextDisplay();
   pair<string,string>* data { static_cast<pair<string,string>*>(v) };
   char fmtString[] { "%s%s\n\nI have completed this analysis.\n" };
   snprintf(fmtBuffer,fmtBufferSize,fmtString,
            data->first.c_str(),data->second.c_str());
   buffer->append(fmtBuffer);
}

void assigned_cb(Fl_Widget* w,void* v)
{
   pair<string,string> *data { static_cast<pair<string,string>*>(v) };
   char fmtString[] { "It has been learned that \n%s\nis %s.\n" };
   snprintf(fmtBuffer,fmtBufferSize,fmtString,
            data->first.c_str(),data->second.c_str());
   buffer->append(fmtBuffer);
}

void looking_cb(Fl_Widget* w,void* v)
{
   string *data { static_cast<string*>(v) };
   char fmtString[] { "Currently looking for: %s\n" };
   snprintf(fmtBuffer,fmtBufferSize,fmtString,
            data->c_str());
   buffer->append(fmtBuffer);
}

void choice_n_cb(Fl_Widget*,void*)
{
   fl_message("Please select answer\nand click \"Ok\" afterwards.");
}

string choice_n(const vector<string>& answers)
{
   // Todo: Set font explicitly.
   // Measure dimension of visible area inside
   // dialog as well as dimensions of 
   // choice input and ok button and compute
   // optimal positions for placing
   // choice and ok.
   Fl_Double_Window dialog(200,100);
   dialog.set_modal();
   Fl_Input_Choice* choice = new Fl_Input_Choice(85,20,80,20,"Answer: ");
   dialog.callback(choice_n_cb,choice);
   for (const auto& answer : answers)
   {
      choice->add(answer.c_str());
   }
   choice->value(0);
   Fl_Button* ok = new Fl_Button(70,60,60,20,"&Ok");
   dialog.end();
   dialog.show();
   // The following code is inspired by
   // http://www-lehre.inf.uos.de/~ftschelh/veranstaltungen/mm/fltk/ask.C.html
   // The or'ed condition was stolen from fl_ask.cxx, line 75
   do
   {
      Fl::wait();
   } while (Fl::readqueue() != ok || Fl::readqueue() == ok && Fl::event() == FL_KEYBOARD);
   // Using Fl::readqueue() is not recommended.
   // Instead a callback function should be used.
   // Examining fl_choice() in fl_ask.cxx shows,
   // that this implies using the global variable
   // ret_val in function innards(). The consequences
   // of doing so, are not exactly known. Therefore
   // the solution above has been chosen.
   return choice->value();
}

void ask_cb(Fl_Widget* w,void* v)
{
   adjustTextDisplay();   
   pair<string,set<string>>* data 
      { static_cast<pair<string,set<string>>*>(v) };
   vector<string> answers;
   buffer->append(data->first.c_str());
   buffer->append("\n");
   for (const auto& a : data->second)
   {
      answers.push_back(a);
   }
   if (answers.size() == 2)
   {
      auto choice { fl_choice("%s",answers.at(0).c_str(),
                                answers.at(1).c_str(),
                                0,data->first.c_str())
                  };
      answer = answers.at(choice);
   }
   if (answers.size() == 3)
   {
      auto choice { fl_choice("%s",answers.at(0).c_str(),
                                answers.at(1).c_str(),
                                answers.at(2).c_str(),
                                data->first.c_str())
                  };
      answer = answers.at(choice);
   }
   if (answers.size() > 3)
   {
      answer = choice_n(answers); 
   }
   buffer->append(answer.c_str());
   buffer->append("\n");
}

struct Waiting
{
   Waiting()
   {
      fl_cursor(FL_CURSOR_WAIT);
   }
   ~Waiting()
   {
      fl_cursor(FL_CURSOR_DEFAULT);
   }
};

void load_cb(Fl_Widget* w,void* v)
{
   string oldName { kbFullName };
   kbFullName = fl_file_chooser("Open File?","*.kb","");
   string name { fileNameOnly(kbFullName) };
   if (kbFullName == oldName)
   {
      fl_message("Knowledge base %s is alreay loaded.",name.c_str());
      return;
   }
   if (input.is_open())
   {
      input.close();
   }
   input.open(kbFullName);
   if (!input)
   {
      fl_message("Error opening %s.",name.c_str());
      kbFullName = "";
      return;
   }
   Waiting waiting;
   delete tokenizer;
   tokenizer = new Tokenizer(input);
   delete parser;
   parser = new Parser(*tokenizer,error);
   optional<KnowledgeBase> result;
   bool ok { false };
   try
   {
      result = parser->parseKnowledgeBase();
      ok = true;
   }
   catch (const logic_error& exception)
   {
      fl_message("Caught exception %s.\n"
                 "%s is probably not a valid knowledge base.\n%s",
                 exception.what(),name.c_str(),error.str().c_str()
                );
   }
   if (ok && !result.has_value())
   {
      if (!result.has_value())
      {
         fl_message("Cannot load knowledge base %s.\n"
                    "%s",name.c_str(),error.str().c_str()
                   );
      }
   }
   if (!ok || !result.has_value())
   {
      kbFullName = "";
      delete parser;
      parser = nullptr;
      delete tokenizer;
      tokenizer = nullptr;
      return;
   }
   kb = result.value();
   window->copy_label((EC_NAME + ": "s + name).c_str());
   menue->mode(startConsultationIndex,0); // No such enum constant
                                          // FL_MENU_ACTIVE
   if (buffer)
   {
      buffer->text(0);
   }
}

void quit_cb(Fl_Widget* w,void* v)
{
   switch (fl_choice("Do yo really want to quit?","Yes","No",0))
   {
      case 0: delete parser;
              delete tokenizer;
              delete help;
              exit(0); 
              break;
      case 1: break;
   }
}

void toggle_cb(Fl_Widget* w,void* v)
{
   if (tracing)
   {
      fl_message("Tracing is now switched OFF.");
   }
   else
   {
      fl_message("Tracing is now switched ON.");
   }
   tracing = !tracing;
}

void start_cb(Fl_Widget* w,void* v)
{
//   Caution: crahses if no kb has been loaded before
//   --> gray out "Start consultation"
   buffer->text(0);
   if (tracing)
   {
      char fmtString[] { "There were %u rule-lines, %u questions and %u\n"
                         "legal answers specified in the knowledge base.\n"
                       };
      snprintf(fmtBuffer,fmtBufferSize,fmtString,
               kb.numberOfRuleLines(),
               kb.numberOfQuestionRules(),
               kb.numberOfLegalAnswers()
              );
      buffer->append(fmtBuffer);
   }
   FltkGUI gui(ask_cb,success_cb,failed_cb,looking_cb,assigned_cb,answer);
   InferenceEngine inf(kb,gui,tracing);
   inf.run();
}

void help_cb(Fl_Widget* w,void* v)
{
   if (help != nullptr)
   {
      help->show();
      return;
   }
   help = new Fl_Help_Dialog;
   help->load("ec_gui.html");
   help->show();
   /* The following two lines are a workaround to set the title
    * of the Fl_Help_Dialog.
    */
   Fl::first_window()->copy_label("EC GUI 1.0 - Help");
   Fl::first_window()->show();
}

void about_cb(Fl_Widget* w,void* v)
{
   fl_message("%s %s",EC_NAME.c_str(),EC_VERSION.c_str());
}

int main(int argc,char* argv[]) 
{
   Fl_Pixmap* pixmap { new Fl_Pixmap(ec_icon) }; // for icon
   Fl_RGB_Image* image { new Fl_RGB_Image(pixmap) }; // for icon

   window = new Fl_Window(800,600); 
   window->copy_label(EC_NAME.c_str());
   window->resizable();
   Fl_Menu_Item menuItems[] =
   {
      { "&File"                      , 0, 0, 0, FL_SUBMENU },
         { "&Open knowledge base..." , 0, (Fl_Callback *)load_cb },
         { "&Exit"                   , 0, (Fl_Callback *)quit_cb },
         { 0 }                       ,
      { "&Run"                       , 0, 0, 0, FL_SUBMENU },
         { "&Toggle tracing"         , 0, (Fl_Callback *)toggle_cb },
         { "&Start consultation"     , 0, (Fl_Callback *)start_cb,0,
                                       FL_MENU_INACTIVE },
         { 0 }                       ,
      { "&Help"                      , 0, 0, 0, FL_SUBMENU },
         { "&EC-Help"                , 0, (Fl_Callback *)help_cb },
         { "&About"                  , 0, (Fl_Callback *)about_cb },
         { 0 },
      { 0 }
   };
   menue = new Fl_Menu_Bar(0,0,800,30);
   menue->copy(menuItems);
   startConsultationIndex = menue->find_index(start_cb);
   buffer = new Fl_Text_Buffer();
   buffer->text(0);
   display = new Fl_Text_Display(1,28,799,594);
   display->buffer(buffer);
   window->add(display);
   window->default_icon(image); // for icon
   window->end();
   window->resizable(display);
   window->callback(quit_cb); // Closing the window should ask for confirmation
   window->show(argc, argv);
   if (startConsultationIndex == -1)
   {
      fl_message("Menu entry \"Start consultation\" not found.\n"
                 "Program aborted.");
      return 1;
   }
   fl_message_hotspot(0); // Do not follow mouse pointer when opening new
                          // dialogs.
   return Fl::run();
}
