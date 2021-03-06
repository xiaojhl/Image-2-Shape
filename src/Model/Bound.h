#ifndef Bound_H
#define Bound_H
#include <cv.h>

class Bound
{
public:
    Bound();
    ~Bound();
	  double getRadius();
    void getCenter(float center[3]);
    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    CvPoint3D32f centroid;
};

#endif