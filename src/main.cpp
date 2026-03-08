#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <sstream>
#include <set>
#include <fstream>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;
int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  // varibles and data structures used
  vector<string> environ_pth;
  set<string> builtin_commands = {"echo", "exit", "type", "pwd", "cd", "ls"};
  vector<string> current_path_vector;

  auto create_environment_pth = [&]()
  {
    string pth = getenv("PATH");
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

  auto file_exists_environment = [&](string fileName)
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
    vector<string> ans;
    stringstream ss(input);
    string word;
    while (ss >> word)
    {
      ans.push_back(word);
    }
    return ans;
  };
  auto convert_path_vector = [&](string input)
  {
    vector<string> str;
    stringstream ss(input);
    string token;
    while (getline(ss, token, '/'))
    {
      str.push_back(token);
    }
    return str;
  };
  auto convert_vector_path = [&](vector<string> vr)
  {
    string s = "";
    for (auto itr : vr)
    {
      s += '/' + itr;
    }
    return s;
  };
  auto path_exists_check = [&](string path)
  {
    return fs::exists(path) && fs::is_directory(path);
  };
  auto remove_qoutes = [&](string s)
  {
    vector<string> vct;
    int flag = 0;
    int flag2 = 0;
    string ans = "";
    string current_string = s;
    for (int i = 0; i < current_string.size(); i++)
    {
      char itr = current_string[i];
      if (!flag && itr == '\"')
      {
        flag2 ^= 1;
        continue;
      }
      if (!flag2 && itr == '\'')
      {
        flag ^= 1;
        continue;
      }
      if (itr != ' ')
      {
        if (itr != '\\')
        {
          ans += itr;
          continue;
        }
        if (flag || flag2)
        {
          if (flag)
          {
            ans += itr;
            continue;
          }
          else
          {
            char nxt = current_string[i + 1];
            ans += nxt;
            i++;
          }
        }
        else
        {
          char nxt = current_string[i + 1];
          ans += nxt;
          i++;
        }
        continue;
      }
      if (flag || flag2)
      {
        ans += itr;
      }
      else
      {
        if (ans.size())
        {
          vct.push_back(ans);
          ans = "";
        }
      }
    }
    if (ans.size())
    {
      vct.push_back(ans);
    }
    return vct;
  };
  // function running
  create_environment_pth();
  current_path_vector = convert_path_vector(filesystem::current_path().string().substr(1));

  while (true)
  {
    cout << "$ ";
    string command;
    getline(std::cin, command);
    vector<string> input = remove_qoutes(command);
    bool output = 0;
    bool error = 0;
    int size = input.size();
    string file = "";
    string stout = "";
    string sterr = "";
    if (input.size() >= 2 && (input[size - 2] == ">" || input[size - 2] == "1>"))
    {
      file = input.back();
      output = 1;
      input.pop_back();
      input.pop_back();
    }
    else if (input.size() >= 2 && input[size - 2] == "2>")
    {
      file = input.back();
      error = 1;
      input.pop_back();
      input.pop_back();
    }
    if (input[0] == "exit")
    {
      exit(0);
    }
    else if (input[0] == "cd")
    {
      vector<string> current;
      if (input[1][0] == '~')
      {
        string pth = getenv("HOME");
        current_path_vector = convert_path_vector(pth.substr(1));
      }
      else if (input[1][0] == '/')
      {
        if (path_exists_check(input[1]))
        {
          // remove the initial / because converting it will add extra blank space due to delimeter use
          current_path_vector = convert_path_vector(input[1].substr(1));
        }
        else
        {
          cout << "cd: " << input[1] << ": No such file or directory" << endl;
        }
      }
      else
      {
        current = current_path_vector;
        vector<string> vr = convert_path_vector(input[1]);
        for (auto itr : vr)
        {
          if (itr == "..")
          {
            current.pop_back();
          }
          else if (itr == ".")
          {
            continue;
          }
          else
          {
            current.push_back(itr);
          }
        }
        if (path_exists_check(convert_vector_path(current)))
        {
          current_path_vector = current;
        }
        else
        {
          cout << "cd: " << input[1] << ": No such file or directory" << endl;
        }
      }
    }
    else if (input[0] == "cat")
    {
      string ans = "";
      int bg = 0;
      vector<string> error;
      for (auto itr : input)
      {
        if (bg == 0)
        {
          bg = 1;
          continue;
        }
        ifstream file(itr);
        if (fs::exists(itr))
        {
          string line;
          while (getline(file, line))
          {
            ans += line + '\n';
          }
          if (ans.back() == '\n')
          {
            ans.pop_back();
          }
        }
        else
        {
          error.push_back(itr);
        }
        file.close();
      }
      for (auto it : error)
      {
        sterr += "cat: " + it + ": No such file or directory\n";
      }
      //ans += '\n';
      stout = ans;
    }
    else if (input[0] == "ls")
    {
      string ans = "";
      string path = input[1];
      int flag = 1;
      if (input[1] == "-1")
      {
        path = input[2];
        flag = 0;
      }
      if (!(fs::exists(path)))
      {
        sterr += "ls: " + input[2] + ": No such file or directory";
      }
      else
      {
        vector<string> filename;
        for (const auto &entry : fs::directory_iterator(path))
        {
          string curr = entry.path().filename().string();
          if (!flag)
          {
            curr += '\n';
          }
          filename.push_back(curr);
        }
        sort(filename.begin(), filename.end());
        if (flag)
        {
          filename.back() += '\n';
        }
        for (auto it : filename)
        {
          ans += it;
        }
        stout = ans;
      }
    }
    else if (input[0] == "pwd")
    {
      cout << convert_vector_path(current_path_vector) << endl;
    }
    else if (input[0] == "echo")
    {
      vector<string> current = remove_qoutes(command.substr(5));
      current.clear();
      for (int i = 1; i < input.size(); i++)
      {
        current.push_back(input[i]);
      }
      string ans = "";
      for (auto itr : current)
      {
        ans += itr + " ";
      }
      // cout << ans << endl;
      stout = ans;
      // if (!output)
      // {
      //   cout << ans << endl;
      // }
      // else
      // {
      //   stout = ans + '\n';
      // }
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
        auto [exists, path] = file_exists_environment(command.substr(5));
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
    else
    {
      auto [exists, path] = file_exists_environment(input[0]);
      if (exists)
      {
        system(command.c_str());
      }
      else
      {
        cout << command << ": command not found" << endl;
      }
    }
    auto file_content_add = [&](string content, string path)
    {
      std::ofstream file(path); // path to file
      file << content;
      file.close();
    };
    if (output)
    {
      file_content_add(stout, file);
    }
    else
    {
      if (stout != "")
        cout << stout<<endl;
    }

    if (error)
    {
      file_content_add(sterr, file);
    }
    else
    {
      if (sterr != "")
        cout << sterr<<endl;
    }
  }
}