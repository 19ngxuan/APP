/*!
 \file EcUser2.cpp 
 * \brief Defines and executes a programmatic user of ec.
 *
   This program simulates a user of EC. The program requires
   two command line parameters, namely the filename of a
   knowledge base and the name of a file which records
   the inference process of all possible combinations of
   answers to questions.
   Only knowledge bases which include the LEGALANSWERS rule
   can be processed. If it is not present the program has
   no idea about valid answers.

   This program introduces string_view and format.
   
   string_view is especially designed for holding string literals.
   Therefore, string_view exempiars can be used only read-only.
   The string_view class offers all const memberfunctions of string.
   Exemplares of string_view are intended to replace const char*.
   For this reason, they are declared as constexpr.

   format() is a function of the formatting library fmt, which has been
   introduced into the C++ 20 standard. Unfortunately, it is not yet
   included in the most C++ compiler distros.

   Using it, requires some manual steps.

   1. Visit https://fmt.dev/latest/index.html
   2. Download the most recent version of the fmt library.
   3. Install CMake on your system, if necessary.
   4. Execute CMake
   5. Enter the directory of the fmt library in the field
      "Where is the source code: " and in the field
      "Where to build the binaries: " as well. 
   6. Afterwards, press Generate.
   7. Now open a terminal window and switch to the fmt library
      directory.
   8. Enter make.
   9. Enter make install

   To use the fmt library you must include
   #include <fmt/format.h>

   If you want to use it without further provisions,
   you must include the line
   #define FMT_HEADER_ONLY
   before the line include <fmt/format.h>.

   If you do not do that, you must compile your program
   with the option -lfmt added.

   The format function is in the namespace fmt. Therefore,
   it mst be called fmt::format. If you don't like that,
   you can import format selectively with
   using fmt::format;
   You should not import all identifieres of namespace fmt with
   using namespace fmt
   because this will incur name collisions.
 
 \author Ulrich W. Eisenecker
 \date February 17, 2022
 */

#include <iostream>
using namespace std;
#include <fmt/format.h>
using fmt::format;
#include <set>
#include <vector>
#include <fstream>
#include <version> // Because of __cpp_lib_source_location.
//#define  __cpp_lib_source_location
#ifdef __cpp_lib_source_location
#include "source_location/source_location.hpp"
using namespace nostd; // because of source_location
#endif // __cpp_lib_source_location

/*!
 * \brief If true, logging is active.
 * 
 * If true, logger instances outpus log 
 * information to the ostream log.
 * After changing the value, the program must 
 * be recompiled.
 * It must be declared as constexpr to allow 
 * its evaluation during compile time.
*/
constexpr bool loggingActive { false };

/*!
 * \brief File to which the logging information is sent.
 *
 * This is the output stream to which the logging 
 * information is sent. By default, cerr is used 
 * for logging. If a different output stream is 
 * desired, the reference log must be initialized
 * with it.
 * If the reference to the log file is changed, the
 * program must be recompiled.
*/
ostream& logFile { std::cerr };

/*! 
 * \brief Logger exemplars log function calls.
 *
 * When entering a function, a Logger exemplar 
 * is created with two strings as arguments. 
 * The first string contains the function name, 
 * the second string is optional and describes 
 * the function parameters. When the function 
 * is exited, the Logger exemplar is destroyed. 
 * This calls the Logger destructor, which logs 
 * the exit of the function.
 * It is also possible to log recursive function 
 * calls.
 */
#ifdef  __cpp_lib_source_location
class Logger
{
   public:
      /*! 
       * \brief Logger constructor.
       *
       * If <source_location> is available, the Logger constructor
       * takes two arguments
       \param [in] arguments If there are no arguments, the empty
       * is used as default value.
       \param [in] source_locatiion This parameter should never be 
       * provided explicitly.
       */
      Logger(const string& arguments = "",
             const source_location location = source_location::current()):
         m_arguments { arguments },
         m_location { location }
      {
         /*!
            The condition of if constexpr is evaluated at 
            compile time. Therefore, if loggingActive is false,
            no code is generated for the statement in the 
            if branch. In this case, the constructor has 
            an empty implementation and may be completely
            optimized away.
          */
         if constexpr (loggingActive)
         {
            string indentation(++m_activeFunctions,'>');
            logFile << indentation << ' ';
            printInfo();
         }
      }
      /*!
       * \brief Logger destructor.
       */
      ~Logger()
      {
         /*! 
            The condition of if constexpr is evaluated at 
            compile time. Therefore, if loggingActive is false,
            no code is generated for the statement in the 
            if branch. In this case, the destructor has 
            an empty implementation and may be completely
            optimized away.
          */
         if constexpr (loggingActive)
         {
            string indentation(m_activeFunctions--,'<');
            logFile << indentation << ' ';
            printInfo();
         }
      }
   private:
      /*!
       * \brief Counts the number of active functions on the function call
       * stack.
       */
      static inline size_t m_activeFunctions { 0 };
      /*!
      \brief Prints source location information.
      */
      void printInfo() const
      {
         logFile << m_location.file_name()
                 << '[' << m_location.line() << ']'
                 << m_location.function_name()
                 << '(' << m_arguments << ')'
                 << endl;
      }
      /*!
       * \brief Contains the name of the function.
       */
     const string m_arguments { }; /*!< Stores parameter values or names. */
     const source_location m_location; /*!< Stores source location object. */
};
#else
class Logger
{
   public:
      /*! 
       * \brief Logger constructor.
       *
       * There is only one Logger constructor. It requires
       * two string arguments. The first contains the name 
       * of the function, the second is optional and 
       * contains information about function parameters. 
       * If there are no function parameters, the value for 
       * the second parameter can be omitted from the call.
       */
      Logger(const string& name,const string& arguments = ""s):
         m_name { name },m_arguments { arguments }
      {
         /*!
            The condition of if constexpr is evaluated at 
            compile time. Therefore, if loggingActive is false,
            no code is generated for the statement in the 
            if branch. In this case, the constructor has 
            an empty implementation and may be completely
            optimized away.
          */
         if constexpr (loggingActive)
         {
            string indentation(++m_activeFunctions,'>');
            logFile << indentation << ' '
                    << m_name << '('
                    << m_arguments << ')' << endl;
         }
      }
      /*!
       * \brief Logger destructor.
       */
      ~Logger()
      {
         /*! 
            The condition of if constexpr is evaluated at 
            compile time. Therefore, if loggingActive is false,
            no code is generated for the statement in the 
            if branch. In this case, the destructor has 
            an empty implementation and may be completely
            optimized away.
          */
         if constexpr (loggingActive)
         {
            string indentation(m_activeFunctions--,'<');
            logFile << indentation << ' '
                    << m_name << "()"
                    << endl;
         }
      }
   private:
      /*!
       * \brief Counts the number of active functions on the function call
       * stack.
       */
      static inline size_t m_activeFunctions { 0 };
      /*!
       * \brief Contains the name of the function.
       */
      const string m_name { },
      /*!
       * \brief Contains a description of the function parameters.
       */
                   m_arguments { };
};
#endif //  __cpp_lib_source_location

/*! Identifiers starting with "error" are
 *  error messages.
 */

/*!
 * \addtogroup error-messages 
 * @{
 */
constexpr string_view
/*!
 * \brief Multi-line string 
 */
errorWrongCall { "2 command line arguments were expected,\n"
                 "but {} was/were specified.\n"
               },
/*!
 * \brief Multi-line string 
 */
errorCorrectCall { "Correct call:\n"
                   "ecuser knowledgebasefilename protocolfilename\n"
                 },
errorProgramTerminated { "Program terminated." },
errorKb { "Cannot open knowledge base file \"{}\".\n" },
errorRf { "Cannot open result file \"{}\".\n" },
errorConsume { "EcUser::consume(): expected \"{}\", got \"{}\"." },
/*!
 * \brief Multi-line string 
 */
errorBadState { "EcUser is bad state, exploration of knowledge "
                "base impossible!" 
              },
/*!
 * \brief Multi-line string 
 */
errorCouldntTellKbName { "Could not tell the name of the \"{}\" "
                         "knowledge base to ec."
                       },
/*!
 * \brief Multi-line string 
 */
errorCouldntGetNumberQuestions { "Could not retrieve the number "
                                 "of questions."
                               },
/*!
 * \brief Multi-line string 
 */
errorCouldntGetNumberLA { "Could not retrieve the number "
                          "of legal answers."
                        },
/*!
 * \brief Multi-line string 
 */
errorCouldntGetLA { "Could not retrieve the values "
                    "of the legal answers."
                  },

/*!
 * \brief Multi-line string 
 */
errorCouldntGetNumberRL { "Could not retrieve the number "
                          "of rule lines."
                        },
errorCouldntGetRL { "Could not retrieve the rule lines section." },
/*!
 * \brief Multi-line string 
 */
errorCouldntGetNumberQ { "Could not retrieve the number "
                          "of questions."
                        },
errorCouldntConsume { "Could not consume \"{}\"." },
errorDuplicateAnswer { "Duplicate answer \"{}\"." },
/*! @}
 */
/*!
 * Identifiers starting with "ec" are part  
 * of output from ec.
 */

/*!
 * \addtogroup ec-output 
 * @{
 */
ecAskForKb { "File name where rules found?: " },
ecPrompt { "==>" },
ecThereWere { "There were " },
ecTraceOn { "TRACE ON" },
ecRuleLines { " rule-lines, " },
ecQuestions { " questions and " },
ecLegalAnswers { "\nlegal answers specified in the knowledge base.\n" },
ecTraceOff { "TRACE OFF" },
ecGo { "GO" },
ecNonsenseAnswer { "93#480ru.:" }, /*!< Caution: It is unlikely that this
                                    * value is a legal answers value, but
                                    * this is not guaranteed!
                                    */
/*!
 * \brief Multi-line string 
 */
ecInvalidAnswer { "I'm sorry, but your answer is not acceptable.\n"
                  "Please be sure you are typing the answer you "  
                  "want fully and correctly,\n"
                  "and please choose your answer from one of these:\n"
                },
ecQuestionMark { "?" }, /*!< Caution: This is not a reliable indicator 
                         * for a question. */
ecErrorInKb { "Error in Knowledge Base." },
ecAnalysisComplete { "\nI have completed this analysis.\n" },
ecExit { "EXIT" },
ecUnknown { "UNKNOWN" };
/*! @}
 */
/*!
 * All identifies staring with "res" are used in preparing 
 * the output for the result file.
 */
/*!
 * \addtogroup result-formatting 
 * @{
 */
const string
resGoal { " ::= " },
resQuestionAnswer { " -- " },
/*!
 * \brief Multi-line string 
 */
resSummary { "There have been {} consultations,\n"
             "{} of them being successful.\n"
             "{} different results.\n" };
/*! @}
 */

/*!
 * \brief Outputs error message.
 *
 * Sends error message to cerr.
 \param [in] msg String containing error message.
 \return Always false.
 */
bool error(const std::string_view& msg)
{
   cerr << "Error: " << msg << '\n' << endl;
   return false;
}

/*!
 * \brief Increments number contained in answers by 1.
 *
 * The answers parameter contains a number in a 
 * numeral system based on max digits.
 * This number is incremented by 1.
 \param [in,out] answers vector whose digits represent a number.
                It is passed by reference.
 \param [in] max Base of the numeral system.
 \return True, if next number was computed,
         false otherwise.
 */
[[nodiscard]] static bool increment(vector<int>& answers,
                                    const int& max)
{
#ifdef __cpp_lib_source_location
   Logger log("answers,"s + to_string(max));
#else
   Logger log("EcUser::increment",format("answers,{}",max));
#endif // __cpp_lib_source_location

   /*! The following loop must not be changed to
    * loop from answers.size()-1 until i >= 0,
    * because i is unsigned. Changing the loop
    * would cause the terminating --i to decrement
    * 0u, which results in the largest possible
    * value that i can hold. This value would
    * then cause answers.at(i) to crash.
    */
   for (auto i { answers.size() }; i > 0; --i)
   {
      if (answers.at(i - 1) < (max - 1))
      {
         answers.at(i - 1) += 1;
         return true;
      }
      else
      {
         answers.at(i - 1) = 0;
      }
   }
   return false;
}

/*!
 * \brief EcUser class.
 *
 * An instance of this class simulates a user of ec.
 * The user reads all output from ec and sends the
 * input ec expects.
 * The knowledge base MUST have at least two values
 * for legal answers.
 * Question rules MUST contain a question mark.
 * The text contained in question and answer rules 
 * MUST not exceed one line.
 */
class EcUser
{
   public:
      /*!
       * \brief A constructor.
       *
       * The constructor establishes the communication between 
       * EcUser and ec. It loads a knowledge base into ec and 
       * retrieves the number of questions and the number of
       * values of the Legal Answers rule.
       * The values of the Legal Answers rule are not retrieved 
       * and the consultation is not started. 
       \param [in] kbName Name of the knowledge base. The caller MUST 
              have verified that a knowledge base with this name 
              exists.
       \param [in,out] res Result file. The caller must have opened the file.
       \param [in,out] eci The file from which ec reads its input.
              The default value is std::cout.
       \param [in,out] eco The file to which ec sends its output.
              The defalt value is std::cin.      
       */
       EcUser(const std::string& kbName,std::ostream& res,
              std::ostream& eci = std::cout,std::istream& eco = std::cin
             ):m_kbName(kbName),m_res(res),m_eci(eci),
               m_eco(eco),m_ok(true)
      {
#ifdef __cpp_lib_source_location
         Logger log(kbName + ",res,eci,eco");
#else
         Logger log("EcUser::EcUser",format("\"{},res,eci,eco",kbName));
#endif // __cpp_lib_source_location
         if (!setKbName())
         {
            m_ok = error(format(errorCouldntTellKbName,m_kbName));
         };
         if (m_ok && !getNumberOfQuestions())
         {
            m_ok = error(errorCouldntGetNumberQuestions);         
         };
         if (m_ok && !getNumberOfLegalAnswers())
         {
            m_ok = error(errorCouldntGetNumberLA); 
         };
         if (m_ok && !getLegalAnswers())
         {
            m_ok =  error(errorCouldntGetLA); 
         };
      }
         /*!
          * \brief Peforms all possible consultations.
          *
          * Performs as many consultations as may exist for the
          * reported numbers of questions and legal values, that is,
          * exponent(m_numberOfLegalAnswers,m_numberOfQuestions).
          * Semantically valid results are stored with corresponding
          * questions and answers.
          */
         bool explore()
         {
#ifdef __cpp_lib_source_location
            Logger log;
#else
            Logger log("EcUser::explore","");
#endif // __cpp_lib_source_location
            set<string> results;
            if (!m_ok)
            {
               return error(errorBadState);
            }
            vector<int> answers(m_numberOfQuestions,0);
            unsigned long long totalRuns { },
                               successfulRuns { };
            bool firstRun { true };
            do
            {
               ++totalRuns;
               if (totalRuns % 1000 == 0)
               {
                  cerr << ".";
               }
               if (firstRun)
               {
                  firstRun = false;
               }
               else
               {
                  m_eci << ecGo
                        << endl;
               };
               set<string> questions;
               string result { consult(answers,questions) };
               if (result.find(ecUnknown) == string::npos)
               {
                  ++successfulRuns;
                  string s { result + resGoal};
                  auto answer { answers.size() };
                  for (const auto& question : questions)
                  {
                     s += question + 
                          resQuestionAnswer +
                          *next(m_legalAnswers.cbegin(),
                                answers.at(answer - 1)) +
                          "  ";
                     --answer;
                  }
                  results.insert(s);
               }
            } while (increment(answers,m_numberOfLegalAnswers));
            m_eci << ecExit 
                  << endl;
            string s;
            getline(m_eco,s);
            report(totalRuns,successfulRuns,results);
            return true;
         }
   private:
      /*!
       * \brief Consumes s from eco.
       * 
       * This member function tries to consume all characters
       * of the argument string from input stream m_eci.
       \param s String with characters to be consumed.
       \return True, if all characters were consumed successfully,
               otherwise false.
       */
      [[nodiscard]] bool consume(const std::string_view& s)
      {
#ifdef __cpp_lib_source_location
         Logger log(static_cast<string>(s));
#else
         Logger log("EcUser::consume",format("\"{}\"",s));
#endif // __cpp_lib_source_location
         char found;
         string read;
         for (const auto& expected : s)
         {
            m_eco.get(found);
            if (m_eco)
            {
               read += found;
            }
            if (!m_eco || found != expected)
            {
               return error(format(errorConsume,s,read));
            }
         }
         return true;
      }
      /*!
       * \brief Set knowledge base name.
       *
       * Tells ec the name of the knowledge base to load.
       \return True, if name of knowledge base could be
               told to ec, otherwise false.
       */
       [[nodiscard]] bool setKbName()
       {
#ifdef __cpp_lib_source_location
          Logger log;
#else
          Logger log("EcUser::setKbName");
#endif // __cpp_lib_source_location
          if (!consume(ecAskForKb))
          {
             return false;
          }
          m_eci << m_kbName
                << endl;
          return true;
       }
       /*!
        * \brief Get number of questions.
        *
        * Tries to extract number of questions in knowledges,
        * If successful, m_numberOfQuestions contains the result.
        * If not successful, m_numberOfQuestions is undefined.
        \return True, if successful, false otherwise.
        */
       [[nodiscard]] bool getNumberOfQuestions()
       {
#ifdef __cpp_lib_source_location
          Logger log;
#else
          Logger log("EcUser::getNumberOfQuestions");
#endif // __cpp_lib_source_location
          if (!consume(ecPrompt))
          {
             return false;
          }
          m_eci << ecTraceOn
                << endl;
          if (!consume(ecThereWere))
          {
             return false;
          }
          int n;
          m_eco >> n;
          if (!m_eco)
          {
             return error(errorCouldntGetNumberRL);
          }
          if (!consume(ecRuleLines))
          {
             return error(errorCouldntGetRL);
          }
          m_eco >> m_numberOfQuestions;
          if (!m_eco)
          {
             return error(errorCouldntGetNumberQ);
          }
          return true;
       }
       /*!
        * \brief Get number of legal answers.
        *
        * Tries to extract number of legal answers in knowledges,
        * If successful, m_numberOfLegalAnswers contains the result.
        * If not successful, m_numberOfLegalAnswers is undefined.
        \return True, if successful, false otherwise.
        */
        [[nodiscard]] bool getNumberOfLegalAnswers()
        {
#ifdef __cpp_lib_source_location
           Logger log;
#else
           Logger log("EcUser::getNumberOfLegalAnswers");
#endif // __cpp_lib_source_location
           if (!consume(ecQuestions))
           {
              return false;
           }
           m_eco >> m_numberOfLegalAnswers;
           if (!m_eco)
           {
              return false;
           }
           if (!consume(ecLegalAnswers))
           {
              return false;
           }
           if (!consume(ecPrompt))
           {
              return false;
           }
           m_eci << ecTraceOff
                 << endl;
           return true;

        }
        /*!
         * \brief Get values of legal answers.
         *
         * Get all the values of the legal answers. To do this,
         * a consultation must be started and an invalid answer
         * must be entered. After that the values of the legal  
         * answers can be parsed from ec's answer.
         * The member function additionally checks for
         * repeated values and too few values.
         \return True, if no error was detected, false otherwise.
         */
        [[nodiscard]] bool getLegalAnswers()
        {
#ifdef __cpp_lib_source_location
           Logger log;
#else
           Logger log("EcUser::getLegalAnswers");
#endif // __cpp_lib_source_location
           if (!consume(ecPrompt))
           {
              return error(format(errorCouldntConsume,ecPrompt));
           }
           m_eci << ecGo 
                 << endl;
           string s;
           getline(m_eco,s);
           m_eci << ecNonsenseAnswer
                 << endl;
           if (!consume(ecInvalidAnswer))
           {
              return error(format(errorCouldntConsume,
                           ecInvalidAnswer));
           }
           for (auto i { 1 }; i <= m_numberOfLegalAnswers; ++i)
           {
              getline(m_eco,s);
              if (m_legalAnswers.contains(s))
              {
                 return error(format(errorDuplicateAnswer,s));
              }
              else
              {
                 m_legalAnswers.insert(s);
              }
           }
           return true;
         }
         /*!
          * \brief Perform consultation.
          *
          * This member function performs a consulttipn of the
          * knowledge base. Which value of legal answers to choose
          * for which question is determined by the passed answers
          * parameter.
          \param [in] answers Contains the indices of legal answers values
            for each question.
          \param [out] questions Reference parameter that contains the asked
            questions after the consultation is finished.
          \return String containing the result of the consultation.
          */
         [[nodiscard]] std::string consult(const std::vector<int>& answers,
                                           std::set<std::string>& questions)
         {
#ifdef __cpp_lib_source_location
            Logger log("answers,questions");
#else
            Logger log("EcUser::consult","answers,questions");
#endif // __cpp_lib_source_location           
            bool isQuestion { false };
            auto answerIndex { answers.size() };
            string s;
            do
            {
               getline(m_eco,s);
               /*!
                * Checking for the presence of some string is an 
                * unreliable heuristic for identifying a question.
                */
               if (s.find(ecQuestionMark) == string::npos)
               {
                  isQuestion = false;
               }
               else
               {
                  isQuestion = true;
                  questions.insert(s);
                  m_eci << *next(m_legalAnswers.cbegin(),
                                 answers.at(answerIndex - 1))
                        << endl;
                  --answerIndex;                         
               }
            } while (isQuestion);
            if (s == ecErrorInKb)
            {
               //m_eco.ignore(80); /*! Texts in ec may not exceed 80 chars!*/
               string t;
               getline(m_eco,t);
               /*!
                  The two statements 
                  string t;
                  getline(m_eco,t);
                  replace the single statement
                  m_eco.ignore(80);
                  which seem to cause subtle problems with file formats
                  converted from DOS to MAC.
               */
               getline(m_eco,s);
            }
            if (!consume(ecAnalysisComplete))
            {
               error(format(errorCouldntConsume,ecAnalysisComplete));
            }
            if (!consume(ecPrompt))
            {
               error(format(errorCouldntConsume,ecPrompt));
            }
            return s;
         }
         /*!
          * \brief Writes result file.
          *
          * Writes all the results of exploring the knowledge base
          * to the m_res file.
          \param [out] totalRuns Total number of consultations.
          \param [out] successfulRuns Number of successful consultations.
          \param [out] results Successful consultations in canonical format.
          */
         void report(const unsigned long long& totalRuns,
                     const unsigned long long& successfulRuns,
                     const set<string>& results)
         {
#ifdef __cpp_lib_source_location
            Logger log(to_string(totalRuns) + 
                       "," + to_string(successfulRuns) +
                       ",results");
#else
            Logger log ("EcUser::report",format("{},{},results",totalRuns,
                        successfulRuns));
#endif // __cpp_lib_source_location
            m_res << format(resSummary,totalRuns,
                            successfulRuns,results.size()
                           )
                  << endl;
            for (const auto& result : results)
            {
               m_res << result
                     << endl;
            }
         }
    private:
       const string m_kbName; /*!< name of the knowledge base. */
       ostream& m_res; /*!< result file. */
       ostream& m_eci; /*!< "cin" of ec. */
       istream& m_eco; /*!< "cout" of ec. */
       int m_numberOfQuestions; /*!< number of questions in knowledge base. */
       int m_numberOfLegalAnswers; /*!< number if legal answers in knowledge
                                        base. */
       set<string> m_legalAnswers; /*!< legal answers values. */
       bool m_ok; /*!< Set to false, if any error while executing constructor
                       occured. */
};

int main(int argc,char * args[])
{
   // Validate command line arguments
   if (argc != 3)
   {
      cerr << format(errorWrongCall,(argc - 1))
           << errorCorrectCall
           << errorProgramTerminated
           << endl;
           return 1;
   }
   ifstream knowledgebase { args[1] };
   if (!knowledgebase)
   {
      cerr << format(errorKb,args[1])
           << errorProgramTerminated
           << endl;
           return 2;

   }
   knowledgebase.close();
   ofstream resultfile { args[2] };
   if (!resultfile)
   {
      cerr << format(errorRf,args[2])
           << errorProgramTerminated 
           << endl;
           return 3;
   }
   EcUser ecUser(args[1],resultfile,cout,cin);
   if (!ecUser.explore())
   {
      cerr << errorProgramTerminated
           << endl;
           return 4;
   };
}
