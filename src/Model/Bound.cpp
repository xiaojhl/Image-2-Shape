#include "Bound.h"
#include <math.h>

Bound::Bound()
  : minX(0), maxX(0), minY(0), maxY(0), minZ(0), maxZ(0)
{
  // define constructor here in case of compling error when using shared_ptr
}

Bound::~Bound()
{

}

double Bound::getRadius()
{
	double radius = sqrt(pow((maxX - minX) / 2,2) + pow((maxY - minY) / 2,2) +pow((maxZ - minZ) / 2,2));
	return radius;
}

void Bound::getCenter(float center[3])
{
  center[0] = (minX + maxX) / 2;
  center[1] = (minY + maxY) / 2;
  center[2] = (minZ + maxZ) / 2;
}