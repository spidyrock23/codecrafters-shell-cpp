#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <sstream>
//#include<system>
using namespace std;
namespace fs = std::filesystem;
int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage

  vector<string> environ_pth;
  auto create_environment_pth = [&]()
  {
    const char *env = getenv("PATH");
    string pth = env;
    string s = "";
    for (auto itr : pth)
    {
      if (itr == ':')
      {
        environ_pth.push_back(s);
        s = "";
        continue;
      }
      s += itr;
    }
    environ_pth.push_back(s);
  };
  
  auto file_exists = [&](string fileName)
  {
    bool exists = 0;
    string path = "";
    string fn = fileName;
    for (auto itr : environ_pth)
    {
      fs::path fullPath = fs::path(itr) / fn;
      if (fs::exists(fullPath))
      {
        struct stat fileStat;
        if (stat(fullPath.string().c_str(), &fileStat) == 0)
        {
          if (fileStat.st_mode & S_IEXEC)
          {
            path = fullPath.string();
            exists = 1;
            break;
          }
        }
      }
    }
    pair<int, string> ans = {exists, path};
    return ans;
  };
  auto seperate_string = [&](string input)
  {
    vector<string>ans;
    stringstream ss(input);
    string word;
    while(ss>>word){
      ans.push_back(word);
    }
    return ans;
  };

  //function run
  create_environment_pth();
  while (true)
  {
    cout << "$ ";
    string command;
    getline(std::cin, command);
    string initial = command.substr(0, 5);
    if (command == "exit")
    {
      exit(0);
    }
    if (initial == "echo ")
    {
      cout << command.substr(5) << std::endl;
    }
    else if (initial == "type ")
    {
      string s = command.substr(5);

      if (s == "echo" || s == "exit" || s == "type") // builtin command
      {
        cout << command.substr(5) << " is a shell builtin" << endl;
      }
      else
      {
        auto [exists, path] = file_exists(command.substr(5));
        if (exists)
        {
          cout << command.substr(5) << " is " << path << endl;
        }
        else
        {
          cout << command.substr(5) << ": not found" << endl;
        }
      }
    }
    else{
      vector<string> input = seperate_string(command);
      auto [exists, path] = file_exists(input[0]);
      if(exists){
        system(command.c_str());
      }
      else{
        cout << command << ": command not found" << endl;
      }
    }
  }
}
