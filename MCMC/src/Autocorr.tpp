#include <stdexcept>

inline Autocorr::Autocorr(unsigned int T):T(T){}

inline void Autocorr::followVariable (const double * variable) {
  if (locked) throw std::runtime_error("The Autocorr is locked and cannot be extended with more variables");
  followedVariables.push_back(variable);
}

inline unsigned int Autocorr::getNumFollowedVariables() const{
  return followedVariables.size();
}


inline unsigned int Autocorr::getT() const{
  return T;
}
