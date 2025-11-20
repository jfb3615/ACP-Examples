// Get the number of particles:                                               
inline unsigned int AbsModel::getNumParticles() const {return nParticles;}

// Get the particle mass                                                      
inline double AbsModel::getParticleMass() const {return mass;}

// Get the positions                                                                        
inline const Eigen::Vector3d & AbsModel::getPosition(unsigned int i) const { return x[i];}

// Get the velocities                                                                       
inline const Eigen::Vector3d & AbsModel::getVelocity(unsigned int i) const {return v[i];}

// Get the time
inline double AbsModel::getTime() const { return time;}




