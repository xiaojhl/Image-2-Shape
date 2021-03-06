#include "SynthesisTool.h"
#include "MeshParameterization.h"

#include <vector>
#include <cv.h>

void SynthesisTool::init(cv::Mat& src_feature, cv::Mat& tar_feature, cv::Mat& src_detail)
{
  // pyramids stored from up to down
  // [0] ------
  // [1]  ----
  // [2]   --
  levels = 3;
  NeighborRange.resize(3);
  NeighborRange[0].height = 9;
  NeighborRange[0].width = 9;
  NeighborRange[1].height = 7;
  NeighborRange[1].width = 7;
  NeighborRange[2].height = 5;
  NeighborRange[2].width = 5;

  // get image for pyramids
  gpsrc_feature.clear();
  gptar_feature.clear();
  gpsrc_detail.clear();
  gptar_detail.clear();

  gpsrc_feature.push_back(src_feature.clone());
  gptar_feature.push_back(tar_feature.clone());
  gpsrc_detail.push_back(src_detail.clone());
  gptar_detail.push_back(cv::Mat::zeros(src_detail.rows, src_detail.cols, CV_32FC1));
  this->generatePyramid(gpsrc_feature, levels);
  this->generatePyramid(gptar_feature, levels);
  this->generatePyramid(gpsrc_detail, levels);
  this->generatePyramid(gptar_detail, levels);
}

void SynthesisTool::generatePyramid(std::vector<cv::Mat>& pyr, int level)
{
  for (int i = 1; i <= level; ++i)
  {
    cv::Mat dst;
    cv::pyrDown(pyr[i - 1], dst, cv::Size(pyr[i - 1].cols / 2, pyr[i - 1].rows / 2));
    pyr.push_back(dst);
  }
}

void SynthesisTool::doSynthesis()
{
  // find best match for each level
  for (int l = levels - 1; l >= 0; --l)
  {
    int width = gptar_detail.at(l).cols;
    int height = gptar_detail.at(l).rows;
    int findX;
    int findY;
    for (int i = 0; i < height; ++i)
    {
      for (int j = 0; j < width; ++j)
      {
        this->findBestMatch(gpsrc_feature, gptar_feature, l, j, i, findX, findY);
        gptar_detail.at(l).at<float>(i, j) = gptar_detail.at(l).at<float>(findY, findX);
      }
    }
  }
}

void SynthesisTool::findBestMatch(ImagePyramid& gpsrc, ImagePyramid& gptar, int level, int pointX, int pointY, int& findX, int& findY)
{
  findX = 0;
  findY = 0;
  double d = 0;
  double dMin = std::numeric_limits<double>::max();
  int sheight = gpsrc.at(level).rows;
  int swidth = gpsrc.at(level).cols;
  for (int i = NeighborRange[level].height; i < sheight - NeighborRange[level].height; ++i)
  {
    for (int j = NeighborRange[level].width; j < swidth - NeighborRange[level].width; ++j)
    {
      d = this->distNeighbor(gpsrc, gptar, level, j, i, pointX, pointY);

      if (d < dMin)
      {
        dMin = d;
        findY = i;
        findX = j;
      }
    }
  }
}

double SynthesisTool::distNeighbor(ImagePyramid& gpsrc, ImagePyramid& gptar, int level, int srcpointX, int srcpointY, int tarpointX, int tarpointY)
{
  // compute feature distance
  // the third dimension in the cv::Mat here could be much larger than 3
  int dim = gpsrc.at(0).dims;
  if (dim != 3)
  {
    std::cerr << "The dimension of the feature map isn't 3!.\n";
  }

  int fdim = gpsrc.at(0).size[2];
  double d = 0;
  double d1 = 0;
  int spy, spx, tpy, tpx;
  for (int i = 0; i < NeighborRange[level].height / 2; ++i)
  {
    for (int j = 0; j < NeighborRange[level].width; ++j)
    {
      spy=srcpointY-NeighborRange[level].height/2+i;
      spx=srcpointX-NeighborRange[level].width/2+j;
      tpy=tarpointY-NeighborRange[level].height/2+i;
      tpx=tarpointX-NeighborRange[level].width/2+j;
      if(tpy<0)
        tpy+=gptar.at(level).rows;
      if(tpx<0)
        tpx+=gptar.at(level).cols;
      if(tpy>=gptar.at(level).rows)
        tpy-=gptar.at(level).rows;
      if(tpx>=gptar.at(level).cols)
        tpx-=gptar.at(level).cols;

      d1 = 0.0;
      for (int k = 0; k < fdim; ++k)
      {
        d1 += pow(gpsrc.at(level).at<float>(spy, spx, k) - gptar.at(level).at<float>(tpy, tpx, k), 2);
      }
      d += sqrt(d1);
    }
  }

  spy = srcpointY;
  tpy = tarpointY;
  for (int j = 0; j < NeighborRange[level].width / 2; ++j)
  {
    spx=srcpointX-NeighborRange[level].width/2+j;
    tpx=tarpointX-NeighborRange[level].width/2+j;
    if(tpx<0)
      tpx+=gptar.at(level).cols;

    d1 = 0;
    for (int k = 0; k < fdim; ++k)
    {
      d1 += pow(gpsrc.at(level).at<float>(spy, spx, k) - gptar.at(level).at<float>(tpy, tpx, k), 2);
    }
    d += sqrt(d1);
  }

  if (level < gpsrc.size() - 1)
  {
    int srcpointXn = srcpointX/2;
    int srcpointYn = srcpointY/2;
    int tarpointXn = tarpointX/2;
    int tarpointYn = tarpointY/2;
    for (int i = 0; i < NeighborRange[level + 1].height; ++i)
    {
      for (int j = 0; j < NeighborRange[level + 1].width; ++j)
      {
        spy=srcpointYn-NeighborRange[level+1].height/2+i;
        spx=srcpointXn-NeighborRange[level+1].width/2+j;
        tpy=tarpointYn-NeighborRange[level+1].height/2+i;
        tpx=tarpointXn-NeighborRange[level+1].width/2+j;
        if(tpy<0)
          tpy+=gptar.at(level+1).rows;
        if(tpx<0)
          tpx+=gptar.at(level+1).cols;
        if(tpy>=gptar.at(level+1).rows)
          tpy-=gptar.at(level+1).rows;
        if(tpx>=gptar.at(level+1).cols)
          tpx-=gptar.at(level+1).cols;

        d1 = 0.0;
        for (int k = 0; k < fdim; ++k)
        {
          d1 += pow(gpsrc.at(level).at<float>(spy, spx, k) - gptar.at(level).at<float>(tpy, tpx, k), 2);
        }
        d += sqrt(d1);
      }
    }
  }

  return d;
}