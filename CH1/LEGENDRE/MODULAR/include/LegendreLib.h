#ifndef _LEGENDRE_H_
#define _LEGENDRE_H_
#include <string>
#include <vector>

// Parse the command line, return an integer representing the degree
// of the Legendre Polynomial to be plotted.
int parseInput(int argc, char *argv[]);

// Manufacture a plotting canvas with x and y axes.  
std::vector<std::string> getCanvas(int width, int height);

// Plot the P_l(x) on the canvas. 
void plotLegendre(std::vector<std::string> & canvas , int l);

#endif
