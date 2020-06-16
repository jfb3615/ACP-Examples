#include <iostream>




bool comparesGreater(int a, int b) {
  if (a > b)
   return true;
 return false;
}

bool comparesGreater(double a, double b) {
  if (a > b)
   return true;
 return false;
}

int main(int argc, char *argv[]) {

 using namespace std;

 cout << "1 (int) compares greater than 2 (int)? " << comparesGreater(1, 2) << endl; // false
 cout << "1.5 (float) compares greater than 1.22 (float)? " << comparesGreater(1.5, 1.22)  << endl; // true

 cout << "3.402E-38 (double) compares greater than 2.7E-35 (double)? " << comparesGreater(3.402E-38, 2.7E-35) << endl; // false

 // cout << "'a' (char) compares greater than 2.48 (float)? " << comparesGreater('a', 2.48) << endl; // --> it gives 'ambiguos' error!!
 // cout << "'a' (char) compares greater than 'c' (char)? " << comparesGreater('a', 2.48) << endl; // TODO: vedi come sono covertiti char->int

 return 0;
}
