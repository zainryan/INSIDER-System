#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

std::vector<std::string> kernelNameVec;

std::string getKernelName(std::string sourceFileName) {
  return sourceFileName.substr(0, sourceFileName.find("."));
}

int main(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    kernelNameVec.push_back(getKernelName(std::string(argv[i])));
  }

  std::ifstream ifs_template(
      "/usr/insider/synthesis/template/reset_template.txt");
  std::string s;
  while (std::getline(ifs_template, s)) {
    cout << s << endl;
    if (s.find("void") != string::npos) {
      for (auto kernelName : kernelNameVec) {
        cout << "ST_Queue<bool> &reset_" << kernelName << ",\n";
      }
    }
  }
  for (auto kernelName : kernelNameVec) {
    cout << "reset_" << kernelName << ".write(0);\n";
  }
  cout << "}\n}\n}\n#endif";

  return 0;
}
