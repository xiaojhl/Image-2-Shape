#ifndef FeatureGuidedVis_H
#define FeatureGuidedVis_H

#include "BasicViewer.h"
#include "DispObject.h"

class FeatureGuided;

class FeatureGuidedVis : public DispObject
{
public:
  FeatureGuidedVis();
  virtual ~FeatureGuidedVis();

  virtual bool display();

  void init(FeatureGuided* init_data_ptr);

protected:
  FeatureGuided* data_ptr;
  GLubyte bgmap[800][800][3];
  float alpha;
  float alpha_bridging;
  int display_step;

private:
  FeatureGuidedVis(const FeatureGuidedVis&);
  void operator = (const FeatureGuidedVis&);
};

#endif