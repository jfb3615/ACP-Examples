#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main() {

  // std::string is a first class object replacing C-style
  // character strings.
  {
    std::string s1="Trout ";
    std::string s2="Fishing ";
    std::string s3="In ";
    std::string s4="America";
    std::cout << s1+s2+s3+s4 << std::endl;
  }

  // std::vector is a first class object replacing C-style
  // arrays here is one way to use them:
  {
    std::vector<double> myVector;
    // Input:
    myVector.push_back(2.0);
    myVector.push_back(3.14159);
    myVector.push_back(7.0);
    // Output:
    for (int i=0;i<myVector.size();i++) {
      std::cout << myVector[i] << std::endl;
    }
  }

  // Here is another way:
  {
    std::vector myVector{2.0,3.14159,7.0};    
    for (auto v : myVector )  std::cout << v << std::endl;
  }

  return 0;
}
