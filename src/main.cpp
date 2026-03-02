#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <sstream>
#include<set>
//#include<system>
using namespace std;
namespace fs = std::filesystem;
int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage

  //varibles and data structures used
  vector<string> environ_pth;
  set<string> builtin_commands = {"echo","exit","type","pwd","cd"};
  vector<string> current_vector;

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
  auto convert_path = [&](string input)
  {
    vector<string> str;
    stringstream ss(input);
    string token;
    while(getline(ss,token,'/'))
    {
      str.push_back(token);
    }
    return str;
  };
  auto convert_back = [&](vector<string> vr)
  {
    string s = "";
    for(auto itr : vr)
    {
      s += '/' + itr;
    }
    return s;
  };
  auto path_exists = [&](string path)
  {
    bool an = fs::exists(path) && fs::is_directory(path);
    return an;
  };
  current_vector = convert_path(filesystem::current_path().string().substr(1));
  
  // function running
  create_environment_pth();

  while (true)
  {
    cout << "$ ";
    string command;
    getline(std::cin, command);
    vector<string> input = seperate_string(command);
    if (input[0] == "exit")
    {
      exit(0);
    }
    else if (input[0]=="cd"){
      vector<string> current;
      if(input[1][0] =='/'){
        if(path_exists(input[1]))
        {
          current_vector = convert_path(input[1].substr(1));
        }
        else{
          cout << "cd: " << input[1] << ": No such file or directory" << endl;
        }
      }
      else{
        current = current_vector;
        vector<string> vr = convert_path(input[1]);
        for(auto itr : vr){
          if(itr == ".."){
            current.pop_back();
          }
          else if (itr=="."){
            continue;
          }
          else{
            current.push_back(itr);
          }
        }
        if(path_exists(convert_back(current))){
          current_vector = current;
        }
        else{
          cout << "cd: " << input[1] << ": No such file or directory" << endl;
        }
      }
    }
    else if(input[0]=="pwd"){
      cout << convert_back(current_vector) << endl;
    }
    else if (input[0] == "echo")
    {
      cout << command.substr(5) << endl;
    }
    else if (input[0] == "type")
    {
      string s = command.substr(5);
      if (builtin_commands.count(s)) // builtin command
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
