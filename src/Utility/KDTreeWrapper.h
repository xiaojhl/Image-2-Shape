#ifndef KDTreeWrapper_H
#define KDTreeWrapper_H

#include "kdtree.h"

#include <memory>
#include <vector>

class KDTreeWrapper
{
public:
  KDTreeWrapper();
  ~KDTreeWrapper();

  void initKDTree(std::vector<float>& data, size_t num_pts, int dim);
  void nearestPt(std::vector<float>& pt);
  void nearestPt(std::vector<float>& pt, int& pt_id);
  void nearestPt(std::vector<float>& pt, int& pt_id, float& dis);
  float nearestDis(std::vector<float>& pt);

private:
  std::shared_ptr<kdtree::KDTree> kdTree;
  kdtree::KDTreeArray kdTree_data;
private:
  KDTreeWrapper(const KDTreeWrapper&);
  void operator = (const KDTreeWrapper&);
};

#endif // !KDTreeWrapper_H
