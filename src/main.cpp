#include <iostream>
#include <string>
#include<cstdlib>
using namespace std;
int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  while (true)
  {
    std::cout << "$ ";
    std::string command;
    std::getline(std::cin, command);
    if(command=="exit"){
      std::exit(EXIT_FAILURE);
    }
    std::cout << command << ": command not found" << std::endl;
  }
}
