#include <iostream>
#include <string>
#include <termios.h>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <sstream>
#include <set>
#include <fstream>
#include <algorithm>
#include <unistd.h>

using namespace std;
namespace fs = std::filesystem;
struct termios original;

void enable_raw()
{
  tcgetattr(STDIN_FILENO, &original);

  struct termios raw = original;
  raw.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void disable_raw()
{
  tcsetattr(STDIN_FILENO, TCSANOW, &original);
}
int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // varibles and data structures used
  vector<string> environ_pth;
  set<string> builtin_commands = {"echo", "exit", "type", "pwd", "cd", "ls", "history"};
  set<string> redirect = {"2>", "2>>", ">", ">>", "1>", "1>>"};
  vector<string> current_path_vector;
  vector<string> store_history;

  // functions used
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
  auto remove_quotes = [&](string current_string)
  {
    vector<string> vct;
    int flag = 0;
    int flag2 = 0;
    string ans = "";
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
  auto file_content_add = [&](string content, string path, bool append)
  {
    if (append)
    {
      std::ifstream check(path);
      bool has_content = check.peek() != std::ifstream::traits_type::eof();
      check.close();

      std::ofstream file(path, std::ios::app);
      if (has_content)
        file << "\n"
             << content;
      else
        file << content;
      file.close();
    }
    else
    {
      std::ofstream file(path);
      file << content;
      file.close();
    }
  };
  // function running
  create_environment_pth();
  current_path_vector = convert_path_vector(filesystem::current_path().string().substr(1));

  while (true)
  {
    cout << "$ ";
    string command;
    int cursor_pos = 0;
    int history_index = -1;
    enable_raw();
    auto redraw = [&]()
    {
      cout << "\r$ ";
      cout << command;
    };
    while (true)
    {
      char c;
      read(STDIN_FILENO, &c, 1);
      if (c == '\n')
      {
        //cout << command << endl;
        break;
      }
      else if (c == 127) // backspace
      {
        if (cursor_pos > 0)
        {
          command.erase(cursor_pos - 1, 1);
          cursor_pos--;
          printf("\b \b");
        }
      }
      else if (c == 27)
      {
        char seq[2];
        read(STDIN_FILENO, &seq[0], 1);
        read(STDIN_FILENO, &seq[1], 1);
        if (seq[0] == '[')
        {
          if (seq[1] == 'A')
          {
            if (!store_history.empty())
            {

              if (history_index < (int)store_history.size() - 1)
                history_index++;

              command = store_history[store_history.size() - 1 - history_index];
              cursor_pos = command.size();
            }
          }
          if (seq[1] == 'B')
          {
            if (history_index > 0)
              history_index--;
            else
              history_index = -1;
            if (history_index == -1)
              command = "";
            else
              command = store_history[store_history.size() - 1 - history_index];
            cursor_pos = store_history.size();
            redraw();
          }
          if (seq[1] == 'C')
          {
            if (cursor_pos < command.size())
            {
              cout << command[cursor_pos];
              cursor_pos++;
            }
          }
          if (seq[1] == 'D')
          {
            if (cursor_pos > 0)
            {
              cursor_pos--;
              printf("\b");
            }
          }
        }
      }
      else
      {
        command.insert(cursor_pos, 1, c);
        cursor_pos++;
        cout << c;
      }
    }
    disable_raw();
    cout << '\n';
    //cout << command << endl;
    vector<string> input = remove_quotes(command);
    store_history.push_back(command);
    int output = 0;
    int error = 0;
    int size = input.size();
    string file = "";
    string stout = "";
    string sterr = "";
    if (input.size() > 2 && redirect.count(input[size - 2]))
    {
      file = input.back();
      if (input[size - 2] == ">" || input[size - 2] == "1>")
        output = 1;
      else if (input[size - 2] == ">>" || input[size - 2] == "1>>")
        output = 2;
      else if (input[size - 2] == "2>")
        error = 1;
      else
        error = 2;
      input.pop_back();
      input.pop_back();
    }
    if (input[0] == "exit")
    {
      exit(0);
    }
    else if (input[0] == "history")
    {
      int si = store_history.size();
      int bg = 0;
      if (input.size() > 1)
      {
        bg = max(0, si - stoi(input[1]));
      }
      for (int i = bg; i < si; i++)
      {
        stout += to_string(i + 1) + " " + store_history[i] + "\n";
      }
      stout.pop_back();
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
        if (fs::exists(input[1]) && fs::is_directory(input[1]))
        {
          // remove the initial / because converting it will add extra blank space due to delimeter use
          current_path_vector = convert_path_vector(input[1].substr(1));
        }
        else
        {
          sterr += "cd: " + input[1] + ": No such file or directory";
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
        if (fs::exists(convert_vector_path(current)) && fs::is_directory(convert_vector_path(current)))
        {
          current_path_vector = current;
        }
        else
        {
          sterr += "cd: " + input[1] + ": No such file or directory";
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
      if (sterr != "")
      {
        sterr.pop_back();
      }
      stout = ans;
    }
    else if (input[0] == "ls")
    {
      string ans = "";

      string path = convert_vector_path(current_path_vector);
      if (input.size() > 1)
      {
        path = input[1];
      }
      int flag = 1;
      if (input.size() > 1)
      {
        if (input[1] == "-1")
        {
          path = input[2];
          flag = 0;
        }
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
        if (!flag)
        {
          if (filename.size())
          {
            filename.back().pop_back();
          }
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
      stout += convert_vector_path(current_path_vector);
    }
    else if (input[0] == "echo")
    {
      string ans = "";
      for (int i = 1; i < input.size(); i++)
      {
        ans += input[i] + " ";
      }
      stout = ans;
    }
    else if (input[0] == "type")
    {
      string s = command.substr(5);
      if (builtin_commands.count(s)) // builtin command
      {
        stout += command.substr(5) + " is a shell builtin";
      }
      else
      {
        auto [exists, path] = file_exists_environment(command.substr(5));
        if (exists)
        {
          stout += command.substr(5) + " is " + path;
        }
        else
        {
          sterr += command.substr(5) + ": not found";
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
        sterr += command + ": command not found";
      }
    }

    if (output)
    {
      file_content_add(stout, file, output - 1);
    }
    else
    {
      if (stout != "")
        cout << stout << endl;
    }
    if (error)
    {
      file_content_add(sterr, file, error - 1);
    }
    else
    {
      if (sterr != "")
        cout << sterr << endl;
    }
  }
}