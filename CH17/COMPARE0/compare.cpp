#include <iostream>




bool comparesGreater(int a, int b) {
  if (a > b)
   return true;
 return false;
}

// bool comparesGreater(float a, float b) {
//   if (a > b)
//    return true;
//  return false;
// }

int main(int argc, char *argv[]) {

 using namespace std;

 cout << "1 (int) compares greater than 2 (int)? " << comparesGreater(1, 2) << endl;
 cout << "1.5 (float) compares greater than 1.22 (float)? " << comparesGreater(1.5, 1.22) << " --> This will returns 'false', which is formally correct for C++ but obviously mathematically wrong!" << endl;

 cout << "'a' (char) compares greater than 2.48 (float)? " << comparesGreater('a', 2.48) << endl;
 cout << "'a' (char) compares greater than 'c' (char)? " << comparesGreater('a', 2.48) << endl; // TODO: vedi come sono covertiti char->int

 return 0;
}
