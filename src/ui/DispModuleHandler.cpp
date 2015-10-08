#include "DispModuleHandler.h"
#include "MainCanvasViewer.h"
#include "TrackballViewer.h"
#include "VectorFieldViewer.h"
#include "MainCanvas.h"
#include "TrackballCanvas.h"
#include "VectorFieldCanvas.h"

#include "Model.h"
#include "FeatureGuided.h"

#include "AlgHandler.h"

#include <QWidget>
#include <QGridLayout>

DispModuleHandler::DispModuleHandler(QWidget* parent)
{
  QGridLayout *gridLayout_3;
  gridLayout_3 = new QGridLayout(parent);
  gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
  main_canvas_viewer.reset(new MainCanvasViewer(parent));
  main_canvas_viewer->setObjectName(QStringLiteral("main_canvas_viewer"));
  gridLayout_3->addWidget(main_canvas_viewer.get(), 0, 0, 2, 2);
  trackball_viewer.reset(new TrackballViewer(parent));
  trackball_viewer->setObjectName(QStringLiteral("trackball_viewer"));
  gridLayout_3->addWidget(trackball_viewer.get(), 0, 2, 1, 2);
  source_vector_viewer.reset(new VectorFieldViewer(parent));
  source_vector_viewer->setObjectName(QStringLiteral("src_vec_field_viewer"));
  gridLayout_3->addWidget(source_vector_viewer.get(),1,2,1,1);
  target_vector_viewer.reset(new VectorFieldViewer(parent));
  target_vector_viewer->setObjectName(QStringLiteral("trg_vec_field_viewer"));
  gridLayout_3->addWidget(target_vector_viewer.get(),1,3,1,1);

  main_canvas.reset(new MainCanvas);
  trackball_canvas.reset(new TrackballCanvas);
  trackball_viewer->setMainCanvasViewer(main_canvas_viewer);
  trackball_viewer->setSourceVectorViewer(source_vector_viewer);
  trackball_viewer->setTargetVectorViewer(target_vector_viewer);

  source_vector_canvas.reset(new VectorFieldCanvas);
  target_vector_canvas.reset(new VectorFieldCanvas);

  source_vector_canvas->setRenderMode(VectorField::SOURCE_MODE);
  target_vector_canvas->setRenderMode(VectorField::TARGET_MODE);

  alg_handler.reset(new AlgHandler);
}

void DispModuleHandler::loadModel(std::shared_ptr<Model> model, std::string model_file_path)
{
  trackball_canvas->setModel(model);
  trackball_viewer->deleteDispObj(trackball_canvas.get());
  trackball_viewer->addDispObj(trackball_canvas.get());
  trackball_viewer->resetCamera();

  main_canvas->setModel(model);
  main_canvas_viewer->deleteDispObj(main_canvas.get());
  main_canvas_viewer->addDispObj(main_canvas.get());
  main_canvas_viewer->setBackgroundImage(QString::fromStdString(model_file_path + "/photo.png"));
  main_canvas_viewer->updateGLOutside();

  alg_handler->setShapeModel(model);
}

void DispModuleHandler::exportOBJ()
{
  trackball_canvas->getModel()->exportOBJ(0);
}

void DispModuleHandler::snapShot()
{
  main_canvas_viewer->getSnapShot();
}

void DispModuleHandler::updateGeometry()
{
  alg_handler->doProjOptimize();
  main_canvas_viewer->setGLActors(alg_handler->getGLActors());
  trackball_viewer->setGLActors(alg_handler->getGLActors());

  updateCanvas();
}

void DispModuleHandler::initFeatureModel()
{
  if (trackball_canvas->getModel())
  {
    std::shared_ptr<FeatureGuided> share_feature_model(new FeatureGuided(trackball_canvas->getModel(), trackball_canvas->getModel()->getDataPath() + "/featurePP.png"));

    source_vector_canvas->setFeatureModel(share_feature_model);
    source_vector_viewer->deleteDispObj(source_vector_canvas.get());
    source_vector_viewer->addDispObj(source_vector_canvas.get());
    source_vector_viewer->updateGLOutside();

    target_vector_canvas->setFeatureModel(share_feature_model);
    target_vector_viewer->deleteDispObj(target_vector_canvas.get());
    target_vector_viewer->addDispObj(target_vector_canvas.get());
    target_vector_viewer->updateGLOutside();

    alg_handler->setFeatureModel(share_feature_model);
  }
}

void DispModuleHandler::updateCanvas()
{
  trackball_viewer->updateBuffer();
  trackball_viewer->updateGLOutside();

  main_canvas_viewer->updateBuffer();
  main_canvas_viewer->updateGLOutside();
}

void DispModuleHandler::setEdgeThreshold(int val)
{
  main_canvas->setEdgeThreshold(float(val) / 100.0);
  main_canvas_viewer->updateGLOutside();
}

void DispModuleHandler::setUseFlat(int state)
{
  main_canvas->setUseFlat(state);
  main_canvas_viewer->updateGLOutside();
}

void DispModuleHandler::showCrspLines(int state)
{
  source_vector_viewer->isDrawAllLines(bool(state));
  target_vector_viewer->isDrawAllLines(bool(state));

  source_vector_viewer->updateGLOutside();
  target_vector_viewer->updateGLOutside();
}

void DispModuleHandler::showProjCrsp(int state)
{
  main_canvas_viewer->setIsDrawActors(bool(state));
  trackball_viewer->setIsDrawActors(bool(state));

  main_canvas_viewer->updateGLOutside();
  trackball_viewer->updateGLOutside();
}

void DispModuleHandler::deleteLastCrspLine_Source()
{
  source_vector_viewer->deleteLastLine();
}

void DispModuleHandler::deleteLastCrspLine_Target()
{
  target_vector_viewer->deleteLastLine();
}