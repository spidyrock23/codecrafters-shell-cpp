#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;
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
    else if (command.substr(0, 5) == "type "){
      string s = command.substr(5);
      if(s == "echo" || s=="exit" || s=="type"){
        cout << command.substr(5) << " is a shell builtin" << endl;
      }
      else{
        cout << command.substr(5) << ": command not found" << endl;
      }
    }
    else
    {
      cout << command << ": command not found" << endl;
    }
  }
}
