
/*
 * Modern C++ : the 'auto' keyword
 *
 */


// --- 'AUTO'

#include <iostream>
#include <memory> // for smart pointers
#include <algorithm>
#include <cmath>  // for std::abs
#include <vector>

std::string getString() {
  std::string myStr = "C++11";
  return myStr;
}


class Widget {
  // Some dummy data;
private:
  double x[10];
  double y[10];
};

// --- SMART POINTERS

// C++98
void useRawPointer() {
  Widget* widget = new Widget();
  // ... make something with the pointer
  delete widget; // do not forget to delete! if not, a memory leak occurs!
}

// C++11
void useSmartPointer() {
  std::unique_ptr<Widget> widget( new Widget() );
  // ... make something with the pointer
} // the pointer is automatically deleted here!

// C++11, with 'auto'
void useSmartPointerAuto() {
  auto widget = std::make_shared<Widget>(); // 'auto' declaration
} // automatically deleted

// --- LAMBDA FUNCTIONS

// a function to sort by absolute value and in reverse order
bool absReverse(int a, int b) {
          return (std::abs(a) > std::abs(b));
        }


// --- INITIALIZER LISTS
#include <initializer_list>

template <class T> class MyClass {
  public:
    MyClass(std::initializer_list<T> l) : vec(l) {
        std::cout << "MyClass object constructed with a " << l.size() << "-element list\n";
    }
    typedef typename std::vector<T>::iterator myIterator;
    myIterator begin() {return vec.begin();}
    myIterator end() {return vec.end();}
  private:
    std::vector<T> vec;
};

// --- UNIFORM INITIALIZATION
struct PosStruct {
  int x, y, z;
};

class PosClass {
  public:
    PosClass(int x, int y, int z): m_x(x), m_y(y), m_z(z) {};
  private:
    int m_x;
    int m_y;
    int m_z;
};

// --- IN-CLASS INITIALIZATION OF DATA MEMBERS:
class MyOtherClass
{
    int ii = 7; //C++11 only
  public:
    MyOtherClass();
};




int main(int argc, char *argv[]) {

  using namespace std;

  // --- AUTO
  cout << "--- AUTO" << endl;


  // C++98
  int iVar = 15;
  std::string strVar = getString();

  // C++11
  auto iVar_2 = 15;
  auto strVar_2 = getString();

  cout << iVar_2 << " - " << strVar_2 << endl;


  // --- SMART POINTERS
  cout << "--- SMART POINTERS" << endl;

  useRawPointer();
  useSmartPointer();
  useSmartPointerAuto();

  // --- RANGE-BASED (FOREACH) LOOP
  cout << "--- RANGE-BASED (FOREACH) LOOP" << endl;

  // C++98
  std::vector<int> iVec;
  iVec.push_back(5);
  iVec.push_back(11);
  for( vector<int>::iterator it = iVec.begin(); it != iVec.end(); ++it ) {
      std::cout << *it << std::endl;
  }

  // C++11
  for( auto item : iVec ) {
      cout << item << endl;
  }

  // C++11 on arrays
  int iArr[] = {1,2,3,4,5};

  for (int i : iArr)
  {
    cout << i << endl;
  }

  // --- ITERATORS: NONMEMBER BEGIN AND END
  cout << "--- ITERATORS: NONMEMBER BEGIN AND END" << endl;

  std::vector<int> iV;
  int iA[10];

  // C++98
  sort( iV.begin(), iV.end() ); // container member methods
  sort( &iA[0], &iA[0] + sizeof(iArr)/sizeof(iArr[0]) ); // array does not offer 'begin' and 'end' methods

  // C++11
  sort( begin(iV), end(iV) ); // container non-member methods
  sort( begin(iA), end(iA) ); // the new methods handle arrays as well


  // --- LAMBDA FUNCTIONS
  cout << "--- LAMBDA FUNCTIONS" << endl;

  // populate the vector
  iV.push_back(1);
  iV.push_back(-2);
  iV.push_back(3);
  for( auto item : iV ) {
    cout << item << " ";
  }
  cout << endl;

  // default sort
  sort( begin(iV), end(iV) );
  for( auto item : iV ) {
    cout << item << " ";
  }
  cout << endl;

  // sort with custom function defined outside of 'main'
  sort( begin(iV), end(iV), absReverse );
  for( auto item : iV ) {
    cout << item << " ";
  }
  cout << endl;

  // sort with custom lambda function defined in-place
  sort( begin(iV), end(iV),
        // start of lambda expression
        [](int a, int b) {
            return (std::abs(a) > std::abs(b));
        } // end of lambda expression
      ); // end of 'sort'
  for( auto item : iV ) {
    cout << item << " ";
  }
  cout << endl;


  // generate vector elements through lambda functions
  int value = 0;
  generate(iV.begin(), iV.end(), [=] () mutable { return value++; });
  for( auto item : iV ) {
    cout << item << " ";
  }
  cout << endl;

  // compute quantities with vector elements through lambda functions
  int grain = 0;
  int sum = 0;
  for_each(iV.begin(), iV.end(), [=, &sum] (int& element) mutable throw() { element += ++grain; sum += element; } );
  for( auto item : iV ) {
    cout << item << " ";
  }
  cout << endl;
  cout << "grain: " << grain << " - sum: " << sum << endl;


  // generate vector elements through lambda functions
  std::vector<int> ivector(3); // vector of 10 items initialized to 0
  for( auto item : ivector ) {
    cout << item << " ";
  }
  cout << endl;

  int val = 0;
  generate(ivector.begin(), ivector.end(), [=] () mutable { return val++; });
  for( auto item : ivector ) {
    cout << item << " ";
  }
  cout << endl;


  // --- INITIALIZER LISTS
  cout << "--- INITIALIZER LISTS" << endl;
  MyClass<int> myC{ 10, 15, 20};
  for( auto item : myC ) cout << item << " ";
  cout << endl;

  // --- UNIFORM INITIALIZATION
  cout << "--- UNIFORM INITIALIZATION" << endl;

  // C++98
  PosStruct pos1; // calls default constructor
  cout << pos1.x << endl;
  PosStruct pos2(); // should call default constructor...but no! We declare a function!
  // cout << pos2.x << endl; // --> of course, this would give an error!
  // PosStruct pos3(2,5,11); // error, because of a missing constructor for the struct
  PosStruct pos4 = {2,5,11};

  // C++11
  PosStruct pos5;
  PosStruct pos6 {};
  PosStruct pos7 {2,5,11};
  PosStruct pos8 = {2,5,11};




  // --- C++98
  PosStruct ps1;
  PosStruct ps2 = {2,5,11};

  PosClass pc1(2,5,11);
  PosClass pc2 = {2,5,11};

  std::vector< PosClass > vecPosClass;
  vecPosClass.push_back( PosClass(1,2,3) );
  vecPosClass.push_back( PosClass(4,5,6) );
  vecPosClass.push_back( PosClass(7,8,9) );

  // PosStruct is an aggregate class, we can initialize an array this way
  PosStruct arrOfPosStruct[] = { {1,2,3}, {4,5,6}, {7,8,9} };

  // PosClass is an ordinary non-aggregate class, we have to write a call to the constructor three times
  PosClass arrOfPosClass[] = { PosClass(1,2,3), PosClass(4,5,6), PosClass(7,8,9) };

  // ordinary types
  int ii = 3;
  int jj = 1;
  std::string str("ABC");


  // --- C++11
  PosStruct _ps1{};
  PosStruct _ps2{2,5,11};
  PosStruct _ps3 = {2,5,11};

  PosClass _pc1{2,5,11};
  PosClass _pc2 = {2,5,11};

  std::vector< PosClass > _vecPosClass = { {1,2,3}, {4,5,6}, {7,8,9} };

  // PosStruct is an aggregate class, we can initialize an array this way
  PosStruct _arrOfPosStruct[] = { {1,2,3}, {4,5,6}, {7,8,9} };

  // PosClass is an ordinary non-aggregate class, now we can initialize the same way
  PosClass _arrOfPosClass[] = { {1,2,3}, {4,5,6}, {7,8,9} };

  // we can use it with ordinary types as well
  int _ii {3};
  int _jj {1};
  std::string _str{"ABC"};


  return 0;
}
