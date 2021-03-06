#include "AlgHandler.h"
#include "ProjOptimize.h"
#include "NormalTransfer.h"
#include "DetailSynthesis.h"
#include "FeatureGuided.h"
#include "Model.h"

AlgHandler::AlgHandler()
{
  init();
}

AlgHandler::~AlgHandler()
{

}

void AlgHandler::init()
{
  proj_optimize.reset(new ProjOptimize);
  normal_transfer.reset(new NormalTransfer);
  detail_synthesis.reset(new DetailSynthesis);


  feature_model = nullptr;
  shape_model = nullptr;
  decomp_img.reset(new DecompImg);
}

void AlgHandler::setFeatureModel(std::shared_ptr<FeatureGuided> model)
{
  feature_model = model;
}

void AlgHandler::setShapeModel(std::shared_ptr<Model> model)
{
  shape_model = model;
}

bool AlgHandler::workable()
{
  if (!feature_model || !shape_model)
  {
    std::cout << "Early return: feature model or shape model is not built correctly.\n";
    return false;
  }

  return true;
}

void AlgHandler::doProjOptimize()
{
  if (!workable())
  {
   return;
  }

  actors.clear();
  proj_optimize->updateShape(feature_model, shape_model);
  // Warning: cannot put updateSourceField() here. Because it need to
  // recompute source curve from visible crest line and
  // visible crest line is updated only if the main canvas has been
  // redrawn and primitive_id image has been updated.
  //feature_model->updateSourceField();
  std::vector<GLActor> temp_actors;
  proj_optimize->getDrawableActors(temp_actors);
  for (size_t i = 0; i < temp_actors.size(); ++i)
  {
    actors.push_back(temp_actors[i]);
  }
}

void AlgHandler::doInteractiveProjOptimize()
{
  if (!workable())
  {
    return;
  }

  actors.clear();
  proj_optimize->updateShapeFromInteraction(feature_model, shape_model);
  //feature_model->updateSourceField();
  std::vector<GLActor> temp_actors;
  proj_optimize->getDrawableActors(temp_actors);
  for (size_t i = 0; i < temp_actors.size(); ++i)
  {
    actors.push_back(temp_actors[i]);
  }
}

void AlgHandler::doNormalTransfer()
{
  if (!workable())
  {
    return;
  }

  normal_transfer->prepareNewNormal(shape_model);
  actors.clear();
  std::vector<GLActor> temp_actors;
  normal_transfer->getDrawableActors(temp_actors);
  for (size_t i = 0; i < temp_actors.size(); ++i)
  {
    actors.push_back(temp_actors[i]);
  }
}

void AlgHandler::doNormalCompute()
{
  /*decomp_img->setModel(shape_model);*/
  decomp_img->computeNormal(shape_model);
  actors.clear();
  std::vector<GLActor> temp_actors;
  decomp_img->getDrawableActors(temp_actors);
  for (size_t i = 0; i < temp_actors.size(); ++i)
  {
    actors.push_back(temp_actors[i]);
  }
}

void AlgHandler::doDetailSynthesis()
{

  detail_synthesis->testMeshPara(shape_model);
  shape_model->exportOBJ(0);
  doNormalTransfer();
  detail_synthesis->computeDisplacementMap(shape_model);
  shape_model->exportOBJ(0);
}