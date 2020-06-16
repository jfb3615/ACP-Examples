// DrawCircle_1.cpp
//
// by Riccardo Maria BIANCHI, Jun 2015
//

// SoQt includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
// Coin includes
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransparencyType.h>
#include <Inventor/SoRenderManager.h>

#include<Inventor/nodes/SoLineSet.h>
#include<Inventor/nodes/SoDrawStyle.h>
#include<Inventor/nodes/SoCoordinate3.h>
#include<Inventor/nodes/SoCone.h>
#include<Inventor/nodes/SoCylinder.h>

#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>

#include <QDebug>

#include <iostream>
  
#include <random> // C++11


#define _USE_MATH_DEFINES
#include <math.h> // together with _USE_MATH_DEFINES, it defines M_PI



// Setup a random numbers generator for branches angle, with the random seed, to have different tree shape each time
std::random_device rd; // take a random seed from the operating system
std::mt19937 mt(rd()); // init the random numbers with the random seed
std::uniform_real_distribution<double> distAngle_u(0, 2*M_PI); // set the range for the random numbers, in this case a uniform distribution


const double K = 9.0e9; // for electrical field, in SI units


/* Map a float value x in range [in_min, in_max] to range [out_min, out_max] */
float mapValueRange(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* Map a float value to range [0., 1.] */
float mapValueRangeZeroOne(float x, float in_min, float in_max, float out_min = 0.0, float out_max = 1.0)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* Map a float value x in range [0, 255] to range [0., 1.] */
float mapValueRange255ToOne(float x, float in_min = 0, float in_max = 255, float out_min = 0.0, float out_max = 1.0)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}



void getValueBetweenTwoFixedColors(float value, int &red, int &green, int &blue)
{
  int aR = 0;   int aG = 0; int aB=255;  // RGB for our 1st color (blue in this case).
  int bR = 255; int bG = 0; int bB=0;    // RGB for our 2nd color (red in this case).
 
  red   = (float)(bR - aR) * value + aR;      // Evaluates as 255*value + 0. (0 -> 0, 1 -> 255) 
  green = (float)(bG - aG) * value + aG;      // Evaluates as 0.
  blue  = (float)(bB - aB) * value + aB;      // Evaluates as -255*value + 255. (0 -> 255, 1 -> 0)
}

/* Divergent color map
 * Where the key points for the curve are:
 *
 *     F=0: (0,0,128) 
 *     F=0.2: (255,255,255) 
 *     F=1: (128, 0, 0)
 *
 * see: http://stackoverflow.com/q/19419043/320369
 */
void getValueDivergentMap05(float value, int &red, int &green, int &blue)
{ 
  if(value <= .5){
      red = value*510;
      green = value*510;
      blue = value*254 + 128;
  }else{
      red = 255 - (value-.5)*254;
      green = 255 - (value-.5)*510;
      blue = 255 - (value-.5)*510;  
  }
}
/*
void getValueDivergentMap02(float value, int &red, int &green, int &blue)
{
  if(value <= .5){
      red = value*510;
      green = value*510;
      blue = value*254 + 128;
  }else{
      red = 255 - (value-.5)*254;
      green = 255 - (value-.5)*510;
      blue = 255 - (value-.5)*510;  
  }
}
*/


 /* color map with control points
  *
  * see: http://dsp.stackexchange.com/a/4679
  */
void getColorValueBetweenTwoColors(float value, int &red, int &green, int &blue, int aRed, int aGreen, int aBlue, int bRed, int bGreen, int bBlue)
{
  // (aRed, aGreen, aBlue) = RGB for our 1st color (e.g. blue)
  // (bRed, bGreen, bBlue) = RGB for our 2nd color (e.g. red)
 
  red   = (float)(bRed - aRed) * value + aRed;      // Evaluates as 255*value + 0. (0 -> 0, 1 -> 255) 
  green = (float)(bGreen - aGreen) * value + aGreen;      // Evaluates as 0.
  blue  = (float)(bBlue - aBlue) * value + aBlue;      // Evaluates as -255*value + 255. (0 -> 255, 1 -> 0)
}
void getValueColorMap(float value, int &red, int &green, int &blue)
{
    // look-up values
    // 0.0  -> (0, 0, 255)    (dark blue)
    // 0.25 -> (128, 0, 128)  (purple)
    // 1.0  -> (255, 0, 0)    (red)
    // 
    if( value < 0.05) getColorValueBetweenTwoColors(value, red, green, blue, 0, 0, 255, 128, 0, 128);
    if( value >= 0.05) getColorValueBetweenTwoColors(value, red, green, blue, 128, 0, 128, 255, 0, 0);
}


void simulateFluid()
{
    
    
    
    
}



/*
double setupElectricalField() 
{
    const double K = 9.0e9;
    const int NQMAX = 2;
    double q[NQMAX], xq[NQMAX], yq[NQMAX];
    nq = 2; // 2 charges
    q[0] = 1.e-6; // charge 1
    q[1] = -1.e-6; // charge 2
    xq[0] = -1; // x charge 1 
    yq[0] = 0;
    xq[1] = 1; // x charge 2
    yq[1] = 0;
    
}
*/
void compute_Electricalfield(double x, double y, double &Ex, double &Ey,
		   int nq, double q[], double xq[], double yq[])
{
    int i;
    Ex = Ey = 0;
    for (i = 0; i < nq; i++) {

	double dx = x - xq[i];
	double dy = y - yq[i];
	double dr2 = dx*dx + dy*dy;
	double dr3i = K*q[i]/(dr2 * sqrt(dr2));

	Ex += dx*dr3i;
	Ey += dy*dr3i;
    }
}
/*
void getElectricalFieldVector(int i, int j, double &Ex, double &Ey)
{
    
    compute_field(i, j, Ex, Ey, nq, q, xq, yq);
}
*/


// Declare a class to represent the Field
class FlowField {

public:
    
    // constructor
    FlowField();

    // setters
    void setField();    
    
    // getters
    std::pair<float, float> getField(int i, int j) { return field[i][j]; }
    double getMax() { return _valueMax; }
    
private:
  // Declaring a 2D array 
    //  (C++11 array, see: http://en.cppreference.com/w/cpp/container/array)
  //std::vector< std::vector < double > > field;
  // std::array<std::array<int, 3>, 3> arr = {{{5, 8, 2}, {8, 3, 1}, {5, 3, 9}}};
    std::pair<float, float> field[20][20];
    double _valueMax; // to store the max value of the field, useful for normalization
    
    
    
  // How many columns and how many rows in the grid?
  //int cols, rows; //TODO: now we use static arrays only
  
  // Resolution of grid relative to window
  // width and height in pixels
  //int resolution; // TODO: when we will switch to other types of array
};

FlowField::FlowField() {
    // cells size
   //resolution = 10;
   
   // Total columns equals width divided by resolution.
   //cols = width/resolution; // TODO:
   
   // Total rows equals height divided by resolution.
   // rows = height/resolution; // TODO:
   // field = new PVector[cols][rows];
    
    // init field
    for (int i=0; i<20; ++i) {
        for (int j=0; j<20; ++j) {
            field[i][j] = std::pair<float, float>(0.0, 0.0);
        }
    }
    
    // init _valueMax
    _valueMax = 0.0;
 }


void FlowField::setField() {
    
    // for an electrical field    
    const int NQMAX = 3;
    int nq;
    double q[NQMAX], xq[NQMAX], yq[NQMAX];
    nq = 3; // 2 charges
    q[0] = 1.e-6; // charge 1
    q[1] = -1.e-6; // charge 2
    q[2] = -1.e-6; // charge 3
    xq[0] = 10; // x charge 1 
    yq[0] = 15;
    xq[1] = 10; // x charge 2
    yq[1] = 5;
    xq[2] = 15; // x charge 3
    yq[2] = 10;
    
    _valueMax = 0.;
    
    for (int i=0; i<20; ++i) {
        for (int j=0; j<20; ++j) {
            
            float theta;
            
            // set all theta = 0
            theta = 0.;
            // set the field with angle theta and length of the field vector
            theta = distAngle_u(mt); // set all field vectors to a random angle in [0, 2pi]

            
            // *** set the field vectors ***
            
            // fake fixed values
            //field[i][j] = std::pair<float, float>( 3.14, 0.5 ); 
            
            // fake random values
            //field[i][j] = std::pair<float, float>( theta, len ); 
                        
            // Set an electrical field            
            double Ex, Ey;
            compute_Electricalfield(i, j, Ex, Ey, nq, q, xq, yq);
            double Elen = sqrt(Ex*Ex + Ey*Ey);
            theta = asin(Ey/Elen);
            if (Ex < 0) theta = M_PI - theta; // because theta and pi-theta has te same Ey component
            field[i][j] = std::pair<float, float>( theta, Elen );  
            
            // finding the max value of the field magnitude
            if (Elen > _valueMax) _valueMax = Elen;
            //if (Ey > _valueMax) _valueMax = Ey;
        }
    }
}








/*
double compute_DipoleField()
{
    int q = 1; // charge
    double e0 = ;
    
    double factor = q / (4 * M_PI * e0);
    
    double ex = 
}

double compute_PointDipoleField( doublde x, double y)
{
    int q = 1; // charge
    
    // Ex = q / 
}
*/

// Declare a class to draw the field on the screen
class DrawField {

public:
    
    DrawField(FlowField* field) : _field(field) {};
    
    void drawFieldLines(SoSeparator*, int objectDraw, bool unitVectors=false, bool normalize=false, bool heatMap=false);
    void drawLine(float x1, float y1, float x2, float y2, SoSeparator* group);
    void drawArrow(float x1, float y1, float theta, double len, SoSeparator* group);
    
private:
    
    FlowField* _field;
    double _max;
    
};


void DrawField::drawLine(float x1, float y1, float x2, float y2, SoSeparator* group)
{
    //std::cout << "\n - drawLine - x1,y1: " << x1 << "," << y1 << " - x2,y2: " << x2 << "," << y2 << std::endl;
    
    SoSeparator *sep = new SoSeparator;
    
    // Declaring a set of 3D coordinates.
    // The SoCoordinate3 seems to contain an array of vectors representing
    // the points in the polyline, so you add them by calling Set1Value on the
    // point attribute, passing in the index of the point and the vector each time.
    SoCoordinate3* points = new SoCoordinate3;

    // Setting two points: the start and the end of the line
    const SbVec3f p1 =  SbVec3f(x1, y1, 0);
    const SbVec3f p2 =  SbVec3f(x2, y2, 0);

    //Using the two points as coordinates
    points->point.set1Value(0, p1); 
    points->point.set1Value(1, p2);

    //static int numPoints[] = {2}; // this tells to draw a line between the first and second 3D-point
    
    SoLineSet *pLineSet = new SoLineSet;
  
    sep->addChild(points);
    sep->addChild(pLineSet);
    
    group->addChild(sep);
}

void DrawField::drawArrow(float x0, float y0, float theta, double len, SoSeparator* group)
{
    std::cout << "\n - drawArrow - x0,y0: " << x0 << "," << y0 << " - theta,mag: " << theta << "," << len << std::endl;            
    
    /* Arrow parameters:
     *   - the total length of the arrow is 'arrowLength'
     *   - the line of the arrow is 2/3 of teh total length
     *   - the arrow head is 1/3 of the total length
     */
    float arrowLength = len;
    float cylinderHeight = 0.75 * arrowLength; // length of the arrow body
    float cylinderRadius = 0.02 ;//* arrowLength; // 'thickness' of the arrow body
    float coneHeight = arrowLength - cylinderHeight; // length of the arrow head
    float coneRadius = 0.1 ;//* arrowLength; // 'thickness' of the arrow head
    
    qDebug() << x0 << y0+(cylinderHeight/2.0);
    qDebug() << x0 << y0+(cylinderHeight/2.0)+(coneHeight/2.0);
    
    
    // Map the vector magnitude to a color
    
    
    
    // Declare a SoSeparator object to store the arrow    
    SoSeparator *sep = new SoSeparator;
    
    /* Define a transformation to rotate the field vector of its angle theta.
     *
     * In order to do that we need a transformations defining:
     *   - and angle of rotation theta, around the Z axis;
     *   - a center of rotation at the vector origin.
     *
     * Note: we choose theta=0 as the trigonometric angle whose cos=1, sin=0, i.e. 0 radians
     *       But Coin3D starts the amplitude of the rotation from the top of the window,
     *       so we begin the angle amplitude from (-pi/2).
     */ 
    SoTransform* transf = new SoTransform;
    transf->rotation.setValue( SbVec3f(0,0,1), (-M_PI/2.)+theta );
    transf->center.setValue(SbVec3f(x0,y0,0));
    
    
    /* Define a transformation to place the cylinder for the arrow body.
     *
     * In order to do that we need:
     *   - a translation to x0
     *   - a translation to y0/2
     * being x0 and y0 the coordinates of the vector origin
     *
     * Note: Coin3D places shapes at the origin.
     * The cylinder below is of length cylinderHeight and lies along the y axis. 
     * Therefore, translating to (0, cylinderHeight/2, 0) places the cylinder 
     * with one end at the vector origin and the other at (0, cylinderHeight, 0).
     */
    SoTransform* tr1 = new SoTransform;
    tr1->translation.setValue(SbVec3f(x0, y0+(cylinderHeight/2.0), 0));
    
    // Define a SoCylinder object for the arrow body
    SoCylinder* cyl = new SoCylinder;
    cyl->radius.setValue(cylinderRadius); // 'thickness' of the arrow body
    cyl->height.setValue(cylinderHeight); // length of the arrow body
    
    /*
     * Define a transformation to place the cone for the arrow head.
     *
     * Transformations in the same separator sum up
     * so here we only need half of the cylinder, to go to the tip of the cylinder
     * plus half of the cone, to put the base of the cone on the top of the cylinder
     * Moreover, we have already done a translation along x, so here we set 0 for the x translation
     */
    SoTransform* tr2 = new SoTransform;
    tr2->translation.setValue(SbVec3f(0, (cylinderHeight/2.0)+(coneHeight/2.0), 0));
    
    // Define a SoCone object for the arrow head
    SoCone* cone = new SoCone;
    cone->bottomRadius.setValue(coneRadius);
    cone->height.setValue(coneHeight);


    // Add the nodes to the local separator
    sep->addChild(transf);  
    sep->addChild(tr1);
    sep->addChild(cyl);
    sep->addChild(tr2);
    sep->addChild(cone);
    
    // Add the separator to the argument group
    group->addChild(sep);
}

void DrawField::drawFieldLines(SoSeparator* group, int objectDraw, bool unitVectors, bool normalize, bool heatMap)
{        
  
    // First, set up the line style, for all lines
    SoDrawStyle* style = new SoDrawStyle();
    style->lineWidth.setValue(2.0f); // line thickness
    //style->lineWidth = stroke/2.;
    group->addChild(style);
    
    // get and store the max value of the field
    _max = _field->getMax();
    
    int red,green,blue;
    
    //loop over field cells
    for (int i=0; i<20; ++i) {
        for (int j=0; j<20; ++j) {
            
            std::pair<float, float> fieldVec = _field->getField(i, j);
            
           /* Draw lines or arrows representing the field vectors
            * 
            * Coordinates of the line for the field vector:
            *   - origin (i, j)
            *   - length = vecLen
            *   - angle = theta
            */
            
            // Origin of the field vector
            float x0 = i;
            float y0 = j;
            
            // Orientation angle and length of the field vector
            float theta = fieldVec.first;
            float vecLen = fieldVec.second;
            
            SoSeparator* elementSep = new SoSeparator;
            
            
            if (heatMap) {
                // Get a normalized value of the field magnitude, to compute the 'heat'
                double normField = mapValueRangeZeroOne(vecLen, 0., _max);
                // Color the Line/Arrow with a color based on the input value, from a gradient range
                //getValueBetweenTwoFixedColors(normField, red, green, blue);
                //getValueDivergentMap05(normField, red, green, blue);
                getValueColorMap(normField, red, green, blue);
                std::cout << "max: " << _max << ", len: " << vecLen << ", norm: " << normField << ", r: " << red << ", b: " << blue << std::endl;
                // Map color values from [0, 255] to [0.0, 1.0]
                float redF = mapValueRange255ToOne(red);
                float greenF = mapValueRange255ToOne(green);
                float blueF = mapValueRange255ToOne(blue);
                std::cout << "redF: " << redF << ", greenF: " << greenF << ", blueF: " << blueF << std::endl;
                // Define a material for the grid
                SoMaterial *heatMaterial = new SoMaterial;                
                heatMaterial->diffuseColor.setValue(redF, greenF, blueF); // set material color
                // Add the material to the separator
                elementSep->addChild(heatMaterial); // add the material to the separator
            }
            if (unitVectors) {
                // Use unit vectors for the field, instead of the actual field magnitude as vector length
                vecLen = 0.5;
            }
            if (normalize) {
                // Normalize field magnitude/vector length to [0, 1] range
                vecLen = mapValueRangeZeroOne(vecLen, 0., _max, 0., 1.);
            }
                    
            
            if (objectDraw == 1) { 
                // Draw a line
                float x1 = x0 + cos(theta) * vecLen;
                float y1 = y0 + sin(theta) * vecLen;
                drawLine(x0, y0, x1, y1, elementSep );
            }
            else if (objectDraw == 2) {
                // Draw an arrow
                drawArrow(x0, y0, theta, vecLen, elementSep );
            }
            else {
                std::cout << "'objectDraw' not valid!!" << std::endl;
            }
            
            group->addChild(elementSep);
            
            
        } // end of for(int j...)
    } // end of for(int i...)
} // end of drawField()


// A method to draw a grid
SoSeparator *makeGrid(int cellsX, int cellsY, float offsetX, float offsetY)
{    
    
    // A SoSeparator to store the grid
    SoSeparator *grid = new SoSeparator;
    grid->ref();

    // Define a material for the grid
    SoMaterial *gridMaterial = new SoMaterial;
    gridMaterial->diffuseColor.setValue(0.4f, 0.4f, 0.4f);
    grid->addChild(gridMaterial); // add the material to the separator

    int n_cells_x = cellsX; // number of grid cells along the X axis
    int n_cells_y = cellsY; // number of grid cells along the Y axis
    
    // Define the number of vertices needed to define the grid
    int n_vertices = 2 * (n_cells_x + 1) + 2 * (n_cells_y + 1);
    
    // 2D array to store the three coordinates of the grid vertices
    float verticesGrid[ n_vertices ][3]; 
    
    // Set the coordinates of the grid vertices
    // first along the X axis...
    int n = 0;
    for (int i=0; i <= n_cells_x; ++i)
    {
        verticesGrid[n][0] = float(i + offsetX);
        verticesGrid[n][1] = float(0 + offsetY);
        verticesGrid[n][2] = float(0.);
        ++n;
                    
        verticesGrid[n][0] = float(i + offsetX);
        verticesGrid[n][1] = float(n_cells_y + offsetY);
        verticesGrid[n][2] = float(0.);
        ++n;
    }
    // ...then, along the Y axis 
    for (int j=0; j <= n_cells_y; ++j)
    {
        verticesGrid[n][0] = float(0 + offsetX);
        verticesGrid[n][1] = float(j + offsetY);
        verticesGrid[n][2] = float(0.);
        ++n;
                
        verticesGrid[n][0] = float(n_cells_x + offsetX);
        verticesGrid[n][1] = float(j + offsetY);
        verticesGrid[n][2] = float(0.);
        ++n;
    }  
    
    // Print the vertices' coordinates on screen  
    /*
    for (int i=0; i<n_vertices; ++i)
        {       
                if ( i%2 == 0 && i != 0 ) std::cout << std::endl;
                
                std::cout << "{ ";
                for (int j=0; j<3; ++j) 
                     std::cout << verticesGrid[i][j] << ", "; 
                std::cout << " }";
        } 
        std::cout << std::endl;
    */
    
    
    
    // Create a SoCoordinate3 object to store the grid vertices' coordinates in the scene graph
    SoCoordinate3 *gridCoords = new SoCoordinate3;
    gridCoords->point.setValues(0, n_vertices, verticesGrid); // (startIndex, numberObjectsWeWantToShow, arrayOfVertices)
    grid->addChild(gridCoords); // add the vertices' coordinates to the scene graph; they will be used by the SoLineSet below


    
    
    // Number of lines defining the grid
    int n_lines = n_cells_x + n_cells_y + 2;
    
    // Array to store the number of vertices in each grid line
    int32_t  numVerticesGridLines[n_lines];
    
    /* Here we declare the number of the vertices, stored in the SoCoordinate3 object, used by each line:
     * - two vertices for the first line: vertex 1, vertex 2
     * - two vertices (vertex 2 and 3) for the second line
     * - ...and so forth
     */
    for (int i=0; i<n_lines; ++i) {
        numVerticesGridLines[i] = 2; // this sets: {2, 2, 2, ...}
        //std::cout << "numVerticesGridLines[" << i << "]: " << numVerticesGridLines[i] << std::endl;
    }
    
    
    // Define the SoLineSet object to draw the grid lines in the scene graph
    SoLineSet *gridLines = new SoLineSet;
    gridLines->numVertices.setValues(0, n_lines, numVerticesGridLines); // (startIndex, numberObjectsWeWantToShow, arrayOfVertices)
    grid->addChild(gridLines); // add the grid lines to the scene graph

    
    return grid;

}






int main()
{
  // Init the Qt windowing system
  // and get a pointer to the window 
  QWidget *window = SoQt::init("test");
  
  // Set the main node for the "scene graph" 
  SoSeparator *root = new SoSeparator;
  root->ref();

  
  SoSeparator* group = new SoSeparator();
  
  // create a field
  //int n_cells = 20; // FIXME: field number of cells should be dynamic
  FlowField* field = new FlowField();
  field->setField();
  
  // create the drawer and draw the field vectors as lines (1) or arrows (2)
  DrawField* drawField = new DrawField(field);
  drawField->drawFieldLines(group, 2, true, false, true);
  
  //root->addChild(matSphere);  
  root->addChild(group);
  
  // Draw a grid and add it to the scene graph
  root->addChild( makeGrid(20, 20, -0.5, -0.5) );
  
  // Init the viewer and get a pointer to it
  SoQtExaminerViewer *b = new SoQtExaminerViewer(window);

  // Set the main node as content of the window and show it
  b->setSceneGraph(root);
  b->show();
  
  // set draw style and transparency type
  //b->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
  //b->setDrawStyle(SoQtViewer::STILL, SoQtViewer::VIEW_WIREFRAME_OVERLAY); // WIREFRAME on top of normal view
  //b->setDrawStyle(SoQtViewer::STILL, SoQtViewer::VIEW_LINE); // WIREFRAME
  
  // set background color to white (default is black)
  b->setBackgroundColor( SbColor(1, 1, 1) );
  
  // Start the windowing system and show our window 
  SoQt::show(window);
  // Loop until exit.
  SoQt::mainLoop();

  // When exiting the main loop...
  // deleting the field...
  delete field;
  // ... and delete the viewer and leave the handle to the main node 
  delete b;
  root->unref();

  // exit.
  return 0;
}




