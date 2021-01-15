#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::vector<char *> allocs;

int argc;
const char ** argv;
std::string args;
std::string argsLessFirst;
char * c_str(std::string & str)
{
  unsigned size = str.size() + 1; // include NULL at end
  char * result = new char[size];
  allocs.push_back(result); // record for cleanup
  for (unsigned i = 0; i < size; ++i) result[i] = str[i];
  return result;
}

void printInputs()
{
  std::cout << "argc (argument count): " << argc << std::endl;
  std::cout << "argv (argument vector): " << args << std::endl;
  for (int i = 0; i < argc; ++i)
  {
    std::cout << "    argv[" << i << "] (argument #" << i << "): " << argv[i] << std::endl;
  }
}

void openExe(const char * commandline)
{
  // simple, but portable version (blocking call)
  //system(commandline);

  // Windows specific, but complicated version (non-blocking call)
  STARTUPINFO startInfo;
  PROCESS_INFORMATION processInfo;
  ZeroMemory(&startInfo, sizeof(startInfo));
  startInfo.cb = sizeof(startInfo);
  ZeroMemory(&processInfo, sizeof(processInfo));
  CreateProcess(nullptr, c_str(std::string(commandline)), nullptr, nullptr, false, 0, nullptr, nullptr, &startInfo, &processInfo);
  CloseHandle(processInfo.hProcess);
  CloseHandle(processInfo.hThread);
}

void explanationAndStub()
{
  if (args.find("Hello world!") != args.npos)
  {
    std::cout << "The inputs for the run of this program are:" << std::endl;
    printInputs();
  }
  else
  {
    std::string programFolder(argv[0]);
    unsigned lastFSlash = programFolder.find_last_of('/');
    if (lastFSlash == programFolder.npos) lastFSlash = 0;
    unsigned lastBSlash = programFolder.find_last_of('\\');
    if (lastBSlash == programFolder.npos) lastBSlash = 0;
    unsigned lastSlash = lastFSlash > lastBSlash ? lastFSlash : lastBSlash;
    if (lastSlash > 0) programFolder = programFolder.substr(0, lastSlash);
    std::string settingsFilename("\"" + programFolder + "\\CustomCommandLineSettings.txt\"");

    std::cout << "Could not find \"CustomCommandLineSettings.txt\" in folder \"" + programFolder + "\"" << std::endl;
    std::cout << "Creating stub file and running C++ arguments explanation instead..." << std::endl;
    std::cout << std::endl;

    std::cout << "In a C++ application, the \"main\" function can recieve two inputs." << std::endl;
    std::cout << "The first input is the number of arguments passed to the program when run." << std::endl;
    std::cout << "The second input is a pointer to an array of arguments (as c-style strings)." << std::endl;
    std::cout << std::endl;
    std::cout << "When recieving these inputs, the declaration of \"main\" looks like this:" << std::endl;
    std::cout << "\"int main(int argc, const char ** argv)\"" << std::endl;
    std::cout << std::endl;

    std::cout << "For example, these are the inputs for this program as it was run:" << std::endl;
    printInputs();
    std::cout << std::endl;

    std::cout << "However, this is very simplistic." << std::endl;
    std::cout << "Let's do a more complex example." << std::endl;
    std::cout << "Let's call this program from this program and add an argument." << std::endl;
    std::cout << "The argument we will add will be \"Hello world!\"" << std::endl;
    std::cout << std::endl;
    args.append(" \"Hello world!\"");

    openExe(args.c_str());
    Sleep(1000);

    std::ofstream settingsFile("CustomCommandLineSettings.txt");
    if (settingsFile.is_open())
    {
      settingsFile << "[INSERT FILEPATH HERE (Ex: \"C:\\Program Files (x86)\\Microsoft Office\\root\\Office16\\EXCEL.EXE\")]" << std::endl;
      settingsFile << "[INSERT SPACE DELINEATED EXTRA SWITCHES HERE (Ex: /r)]" << std::endl;
      settingsFile.close();
    }
    else
    {
      std::cout << "Could not create stub settings file: " + settingsFilename << std::endl;
    }
    std::cout << std::endl;

    std::cout << std::endl << "Press ENTER to close" << std::endl;
    getchar();
  }
}

int main(int argc, const char ** argv)
{
  ::argc = argc;
  ::argv = argv;
  args = argv[0];
  argsLessFirst = "";
  for (int i = 1; i < argc; ++i)
  {
    argsLessFirst.append(" ");
    std::string arg(argv[i]);
    if (arg.find(' ') < 0)
      argsLessFirst.append(argv[i]);
    else
    {
      argsLessFirst.append("\"");
      argsLessFirst.append(argv[i]);
      argsLessFirst.append("\"");
    }
  }
  args += " " + argsLessFirst;

  std::ifstream settingsFile("CustomCommandLineSettings.txt");
  if (settingsFile.is_open())
  {
    std::string filepath, switches;
    std::getline(settingsFile, filepath);
    std::getline(settingsFile, switches);
    openExe((filepath + " " + switches + " " + argsLessFirst).c_str());
    settingsFile.close();
  }
  else
  {
    explanationAndStub();
  }

  // cleanup
  for (char * alloc : allocs) delete[] alloc;
}
