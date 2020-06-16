#include <iostream>
#include <vector>

#include <set>
#include <map>
#include <algorithm>
#include <queue>
#include <numeric>

int main(int argc, char *argv[]) {
  
  // VECTOR
  std::vector<int> iVec;
  iVec.reserve(1000); // memory allocation
  
  std::cout << iVec.size() << std::endl;   //prints 0
  std::cout << iVec.capacity() << std::endl; //prints 1

iVec.push_back( 5 );
iVec.push_back( 50 );

std::cout << iVec.size() << std::endl;   //prints 0
std::cout << iVec.capacity() << std::endl; //prints 1


// MAP & SET
std::map<char,int> cMap;

cMap['a']=10;
cMap['b']=30;

std::cout << "size: " << cMap.size() << std::endl;   //prints 2
std::cout << "element 'a': " << cMap['a'] << std::endl;   //prints 2
std::cout << "element 'b': " << cMap['b'] << std::endl;   //prints 2


// PRIORITY_QUEUE
std::priority_queue< int > q1;
for(int n : {2,9,6,5,4,3,0,8,7,1})
    q1.push(n);

std::priority_queue< int, std::vector<int>, std::greater<int> > q2;
for(int n : {1,5,8,9,3,0,2,6,7,4})
    q2.push(n);

while ( ! q1.empty() ) {
    std::cout << q1.top() << " "; // get the top element
    q1.pop();                     // delete the top element
}
std::cout << std::endl;

while ( ! q2.empty() ) {
    std::cout << q2.top() << " "; // get the top element
    q2.pop();                     // delete the top element
}
std::cout << std::endl;


// VECTOR ITERATORS
std::vector<int> vec;

vec.push_back(1);
vec.push_back(2);
vec.push_back(3);

std::vector<int>::iterator vecIt; // a RandomAccess iterator

for ( vecIt = vec.begin(); vecIt != vec.end(); ++vecIt ) {
  std::cout << (*vecIt) << std::endl; // read the element value (r-value)
}
for ( vecIt = (vec.end()-1); vecIt != (vec.begin()-1); --vecIt ) {
  std::cout << (*vecIt) << std::endl; // read the element value (r-value)
}
for ( vecIt = vec.begin(); vecIt != vec.end(); vecIt++) {
  (*vecIt) = 10;  // write to the vector element (l-value)
  std::cout << (*vecIt) << std::endl; // read the iterator value
}

std::vector<int>::reverse_iterator rVecIt;        // a reversed RandomAccess iterator
size_t idx = 1;
for ( vecIt = vec.begin(); vecIt != vec.end(); ++vecIt, ++idx) {
  (*vecIt) = idx;  // write to the vector element (l-value)
  std::cout << (*vecIt) << std::endl; // read the iterator value
}
for ( rVecIt = vec.rbegin(); rVecIt != vec.rend(); rVecIt++) {
  std::cout << (*rVecIt) << std::endl; // read the element value (r-value)
}


// MAP ITERATORS
std::map< std::string, int> iMap;
std::map< std::string, int >::const_iterator mapIt;

iMap["A"] = 5;
iMap["B"] = 50;

for ( mapIt = iMap.begin(); mapIt != iMap.end(); ++mapIt ) {
  std::cout << "key: " << (*mapIt).first << " - value: " << (*mapIt).second << std::endl;
}


// ALGORITHMS


std::vector<int> intVec;
std::vector<int>::iterator it;
intVec.push_back(3);
intVec.push_back(2);
intVec.push_back(5);
intVec.push_back(4);
intVec.push_back(1);

// sort the elements
std::sort( intVec.begin(), intVec.end() );

// print all container's elements
for ( std::vector<int>::iterator vecIt = intVec.begin(); vecIt != intVec.end(); ++vecIt )
  std::cout << (*vecIt) << std::endl;

// find the minimum value
it = std::min_element( intVec.begin(), intVec.end() );
std::cout << "min: " << *it << std::endl;

// // find the maximum value
it = std::max_element( intVec.begin(), intVec.end() );
std::cout << "max: " << *it << std::endl;

// find the element whose value is 2
it = std::find( intVec.begin(), intVec.end(), 2 );
std::cout << "item: " << *it << std::endl;

// declare a new empty vector
std::vector<int> intVec_copy;
// resize its capacity to be able to store the element from the first vector
intVec_copy.resize( intVec.size() );

std::copy( intVec.begin(), intVec.end(), intVec_copy.begin() );
// print all container's elements
for ( std::vector<int>::iterator vecIt = intVec_copy.begin(); vecIt != intVec_copy.end(); ++vecIt )
std::cout << (*vecIt) << std::endl;

std::cout << "sum: " << std::accumulate( intVec_copy.begin(), intVec_copy.end(), 0 ) << std::endl;
std::cout << "sum(-10): " << std::accumulate( intVec_copy.begin(), intVec_copy.end(), -10 ) << std::endl;

return 0;
}
