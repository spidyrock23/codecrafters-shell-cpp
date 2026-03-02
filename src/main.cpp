#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <sys/stat.h>
using namespace std;
namespace fs = std::filesystem;
int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage

  while (true)
  {
    cout << "$ ";
    string command;
    getline(std::cin, command);

    if (command == "exit")
    {
      exit(0);
    }
    if (command.substr(0, 5) == "echo ")
    {
      cout << command.substr(5) << std::endl;
    }
    else if (command.substr(0, 5) == "type ")
    {
      string s = command.substr(5);

      if (s == "echo" || s == "exit" || s == "type")
      {
        cout << command.substr(5) << " is a shell builtin" << endl;
      }
      else
      {
        string fn = command.substr(5);
        int op = 0;
        
        const char *env = getenv("PATH");
        string pth = env;
        vector<string> str;
        string s = "";
        for (auto itr : pth)
        {
          if (itr == ':')
          {
            str.push_back(s);
            s = "";
            continue;
          }
          s += itr;
        }
        str.push_back(s);
        for (auto itr : str)
        {
          fs::path fullPath = fs::path(itr) / fn;
          if (fs::exists(fullPath))
          {
            struct stat fileStat;
            if (stat(fullPath.string().c_str(), &fileStat) == 0)
            {
              if (fileStat.st_mode & S_IEXEC)
              {
                cout << command.substr(5) << " is " << fullPath.string() << endl;
                op = 1;
                break;
              }
            }
          }
        }
        if (!op)
        {
          cout << command.substr(5) << ": not found" << endl;
        }
      }
    }
    else
    {
      cout << command << ": command not found" << endl;
    }
  }
}
