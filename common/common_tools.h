#ifndef __common_tools_h_
#define __common_tools_h_

#include <iostream>

class CommonTools
{
public:
  CommonTools();
  ~CommonTools();

  static inline char* getCmdOption(char ** begin, char ** end, const std::string & option)
  {
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
    return *itr;
    }
    return 0;
  }

  static inline bool cmdOptionExists(char** begin, char** end, const std::string& option)
  {
    return std::find(begin, end, option) != end;
  }

  static inline std::string ssprintf(const char *format, ...)
  {
    if(format && strlen(format))
      {
      char buffer[4096];
      va_list args;
      va_start (args, format);
      vsnprintf (buffer, 4096, format, args);
      va_end (args);
      return std::string(buffer);
      }
    else
      return std::string();
  }
};

#endif