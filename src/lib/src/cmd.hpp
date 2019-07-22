#pragma once

#include <string>

class CmdArgs {
  public:
    CmdArgs(int argc, char** argv);
    
    const char* exec();
    const char* cwd();
    const char* dir(const char* defaultValue = nullptr);
    const char* script(const char* defaultValue = nullptr);
    bool has(const char* longFlag, const char* shortFlag);
    const char* get(const char* longFlag, const char* shortFlag);

  protected:
  
  private:
    bool empty();
    const char* value(int i);

    int flagIndexOf(const char* longFlag, const char* shortFlag);

    int indexOf(const char* c);

    int m_argc;
    char** m_argv;
    std::string m_exec;
    std::string m_cwd;
};