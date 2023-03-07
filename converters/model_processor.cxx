#include <iostream>
#include <string>
#include <vtkDecimatePro.h>
#include <vtkPolyDataReader.h>
#include <vtkNew.h>
#include <vtkActor.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataWriter.h>
#include <vtkPointData.h>


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

  if (argc < 5)
  {
    std::cout << "usage: ModelProcessor input.vtk output.vtk dc_factor flag_render\n";
    std::cout << "dc_factor [0-100] represents target reduction rate of the decimation algorithm\n";
    std::cout << "flag_render {r, nr} r = render, nr = not render\n";
    return EXIT_FAILURE;
  }

  std::string fnIn = argv[1];
  std::string fnOut = argv[2];
  double dc_factor = std::stod(argv[3]);
  std::string flag_render = argv[4];
  
  std::cout << "-- Input File Name: " << fnIn << std::endl;
  std::cout << "-- Output File Name: " << fnOut << std::endl;
  std::cout << "-- dc_factor: " << dc_factor << std::endl;

  dc_factor /= 100.0;

  vtkNew<vtkPolyDataReader> reader;
  reader->SetFileName(fnIn.c_str());
  reader->Update();

  vtkSmartPointer<vtkPolyData> poly_tail = reader->GetOutput();
  vtkSmartPointer<vtkPolyData> poly_head = reader->GetOutput();

  std::cout << "\n ========== Before Processing =============\n";
  std::cout << "-- Number of Polygons: " << poly_tail->GetNumberOfPolys() << std::endl;
  std::cout << "-- Size: " << poly_tail->GetActualMemorySize() << std::endl;
  
  // vtkNew<vtkWindowedSincPolyDataFilter> flt_taubin;
  // flt_taubin->SetInputData(poly_tail);
  // flt_taubin->SetNumberOfIterations(50);
  // flt_taubin->SetPassBand(0.01);
  // flt_taubin->Update();
  // poly_tail = flt_taubin->GetOutput();

  vtkNew<vtkDecimatePro> flt_decimate;
  flt_decimate->SetInputData(poly_tail);
  flt_decimate->SetTargetReduction(dc_factor);
  //flt_decimate->PreserveTopologyOn();
  flt_decimate->Update();
  poly_tail = flt_decimate->GetOutput();

  // vtkNew<vtkTriangleFilter> flt_triangle;
  // flt_triangle->SetInputData(poly_tail);
  // flt_triangle->Update();
  // poly_tail = flt_triangle->GetOutput();

  // auto pd = poly_tail->GetPointData();
  // auto nrm = pd->GetNormals();

  // int total = 0, cnt = 0;
  // for(size_t i = 0; i < (size_t)nrm->GetNumberOfTuples(); i++)
  //   for(size_t j = 0; j < (size_t)nrm->GetNumberOfComponents(); j++)
  //   {
  //     ++total;
  //     auto val = nrm->GetComponent(i,j);
  //     if (val < 0)
  //       nrm->SetComponent(i,j, -val);
  //   }

  // std::cout << "Total Normal Count: " << total << std::endl;
  // std::cout << "Negative Normal Count: " << cnt << std::endl;
        //nrm->SetComponent(i,j,-nrm->GetComponent(i,j));


  std::cout << "\n ========== After Processing =============\n";
  std::cout << "-- Number of Polygons: " << poly_tail->GetNumberOfPolys() << std::endl;
  std::cout << "-- Size: " << poly_tail->GetActualMemorySize() << std::endl;

  vtkNew<vtkPolyDataWriter> writer;
  writer->SetInputData(poly_tail);
  writer->SetFileName(fnOut.c_str());
  writer->Write();

  if (flag_render == "nr") // skip the rendering
    return EXIT_SUCCESS;

  
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
  light_a_0->SetIntensity(0.3);

  vtkNew<vtkLight> light_a_1;
  light_a_1->SetLightTypeToCameraLight();
  light_a_1->SetPosition(1, 0, 1);
  light_a_1->SetIntensity(0.3);

  vtkNew<vtkLight> light_a_2;
  light_a_2->SetLightTypeToCameraLight();
  light_a_2->SetPosition(0, -1, 1);
  light_a_2->SetIntensity(0.3);

  vtkNew<vtkLight> light_a_3;
  light_a_3->SetLightTypeToCameraLight();
  light_a_3->SetPosition(-1, 0, 1);
  light_a_3->SetIntensity(0.3);

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