#include <iostream>
#include <string>
#include <vtkDecimatePro.h>
#include <vtkPolyDataReader.h>
#include <vtkNew.h>
#include <vtkActor.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkSmoothPolyDataFilter.h>


#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkLight.h>


int main (int argc, char *argv[])
{
  std::cout << "======================================\n";
  std::cout << "-- AVRSPT Model Processor\n";
  std::cout << "======================================\n";

  if (argc < 3)
  {
    std::cout << "usage: ModelProcessor input.nii.vtk output vtk" << std::endl;
    return EXIT_FAILURE;
  }

  std::string fnIn = argv[1];
  std::string fnOut = argv[2];

  std::cout << "-- Input File Name: " << fnIn << std::endl;
  std::cout << "-- Output File Name: " << fnOut << std::endl;

  vtkNew<vtkPolyDataReader> reader;
  reader->SetFileName(fnIn.c_str());
  reader->Update();

  vtkSmartPointer<vtkPolyData> poly_tail = reader->GetOutput();
  vtkSmartPointer<vtkPolyData> poly_head = reader->GetOutput();

  std::cout << "\n ========== Before Processing =============\n";
  std::cout << "-- Number of Polygons: " << poly_tail->GetNumberOfPolys() << std::endl;
  std::cout << "-- Size: " << poly_tail->GetActualMemorySize() << std::endl;
  
  vtkNew<vtkWindowedSincPolyDataFilter> flt_taubin;
  flt_taubin->SetInputData(poly_tail);
  flt_taubin->SetNumberOfIterations(50);
  flt_taubin->SetPassBand(0.01);
  flt_taubin->SetFeatureAngle(30);
  flt_taubin->BoundarySmoothingOn();
  flt_taubin->FeatureEdgeSmoothingOn();
  flt_taubin->NonManifoldSmoothingOn();
  flt_taubin->NormalizeCoordinatesOn();
  flt_taubin->Update();
  poly_tail = flt_taubin->GetOutput();

  vtkNew<vtkDecimatePro> flt_decimate;
  flt_decimate->SetInputData(poly_tail);
  flt_decimate->SetTargetReduction(0.8);
  //flt_decimate->PreserveTopologyOff();
  flt_decimate->Update();
  poly_tail = flt_decimate->GetOutput();

  // vtkNew<vtkSmoothPolyDataFilter> flt_smooth;
  // flt_smooth->SetInputData(poly_tail);
  // flt_smooth->SetNumberOfIterations(100);
  // flt_smooth->SetRelaxationFactor(0.1);
  // flt_smooth->FeatureEdgeSmoothingOff();
  // flt_smooth->BoundarySmoothingOn();
  // flt_smooth->Update();
  // poly_tail = flt_smooth->GetOutput();

  // vtkNew<vtkWindowedSincPolyDataFilter> flt_taubin_post;
  // flt_taubin_post->SetInputData(poly_tail);
  // flt_taubin_post->SetNumberOfIterations(50);
  // flt_taubin_post->SetPassBand(0.01);
  // flt_taubin_post->SetFeatureAngle(120);
  // flt_taubin_post->BoundarySmoothingOff();
  // flt_taubin_post->FeatureEdgeSmoothingOff();
  // flt_taubin_post->NonManifoldSmoothingOn();
  // flt_taubin_post->NormalizeCoordinatesOn();
  // flt_taubin_post->Update();
  // poly_tail = flt_taubin_post->GetOutput();
  

  std::cout << "\n ========== After Processing =============\n";
  std::cout << "-- Number of Polygons: " << poly_tail->GetNumberOfPolys() << std::endl;
  std::cout << "-- Size: " << poly_tail->GetActualMemorySize() << std::endl;

  std::cout << "\n ========== Rendering Result ... =============\n ";
  vtkNew<vtkPolyDataMapper> pre_mapper;
  vtkNew<vtkPolyDataMapper> post_mapper;
  vtkNew<vtkActor> pre_actor;
  vtkNew<vtkActor> post_actor;
  vtkNew<vtkRenderer> pre_renderer;
  vtkNew<vtkRenderer> post_renderer;

  vtkNew<vtkRenderWindow> render_window;
  vtkNew<vtkRenderWindowInteractor> interactor;
  vtkNew<vtkInteractorStyleTrackballCamera> interactor_style;
  vtkNew<vtkCamera> shared_camera;

  pre_mapper->AddInputDataObject(poly_head);
  pre_actor->SetMapper(pre_mapper);
  //pre_actor->GetProperty()->SetRepresentationToWireframe();
  //pre_actor->GetProperty()->SetColor(0.5,0.5,0.5);
  pre_renderer->AddActor(pre_actor);
  pre_renderer->SetViewport(0, 0, 0.5, 1);
  pre_renderer->SetActiveCamera(shared_camera);
  pre_renderer->SetBackground(0, 1, 0);

  vtkNew<vtkLight> light_a_0;
  light_a_0->SetLightTypeToCameraLight();
  light_a_0->SetPosition(0, 1, 1);
  light_a_0->SetIntensity(0.5);

  vtkNew<vtkLight> light_a_1;
  light_a_1->SetLightTypeToCameraLight();
  light_a_1->SetPosition(1, 0, 1);
  light_a_1->SetIntensity(0.5);

  vtkNew<vtkLight> light_a_2;
  light_a_2->SetLightTypeToCameraLight();
  light_a_2->SetPosition(0, -1, 1);
  light_a_2->SetIntensity(0.5);

  vtkNew<vtkLight> light_a_3;
  light_a_3->SetLightTypeToCameraLight();
  light_a_3->SetPosition(-1, 0, 1);
  light_a_3->SetIntensity(0.5);

  pre_renderer->AddLight(light_a_0);
  pre_renderer->AddLight(light_a_1);
  pre_renderer->AddLight(light_a_2);
  pre_renderer->AddLight(light_a_3);



  post_mapper->AddInputDataObject(poly_tail);
  post_actor->SetMapper(post_mapper);
  //post_actor->GetProperty()->SetRepresentationToWireframe();
  post_renderer->AddActor(post_actor);
  post_renderer->SetViewport(0.5, 0, 1, 1);
  post_renderer->SetActiveCamera(shared_camera);
  post_renderer->SetBackground(1, 0, 0);
  post_renderer->ResetCamera();

  post_renderer->AddLight(light_a_0);
  post_renderer->AddLight(light_a_1);
  post_renderer->AddLight(light_a_2);
  post_renderer->AddLight(light_a_3);

  render_window->AddRenderer(post_renderer);
  render_window->AddRenderer(pre_renderer);
  render_window->SetSize(3000, 1500);

  interactor->SetRenderWindow(render_window);
  interactor->SetInteractorStyle(interactor_style);

  render_window->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}