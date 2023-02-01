#include <algorithm>
#include <iostream>
#include <itkImageFileReader.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkExtractImageFilter.h>
#include <itkImageFileWriter.h>
#include <itkResampleImageFilter.h>
#include <itkImageToVTKImageFilter.h> // require ITK's Module_ITKVtkGlue=ON

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataWriter.h>

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
  char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end)
  {
  return *itr;
  }
  return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
  return std::find(begin, end, option) != end;
}

inline std::string ssprintf(const char *format, ...)
{
  if(format && strlen(format))
    {
    char buffer[4096];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer, 4096, format, args);
    va_end (args);
    return std::string(buffer);
    }
  else
    return std::string();
}

int main(int argc, char * argv[])
{
  char ** argBegin = argv;
  char ** argEnd = argv + argc;
  char * fnInput = nullptr;
  char * fnOutputPtrn = nullptr;

  if (cmdOptionExists(argBegin, argEnd, "-i"))
  {
    fnInput = getCmdOption(argBegin, argEnd, "-i");
    if (!fnInput)
    {
      std::cerr << "Empty or Invalid Input File Name!" << std::endl;
      return EXIT_FAILURE;
    }
    std::cout << "-- InputFileName: " << fnInput << std::endl;
  }

  double scale = 1.0;
  if (cmdOptionExists(argBegin, argEnd, "-rs"))
  {
    char * strScale = getCmdOption(argBegin, argEnd, "-rs");
    
    if (strScale) 
    {
      scale = std::stod(strScale);
    }
    std::cout << "-- Resample Scale: " << scale << std::endl;
  }

  if (cmdOptionExists(argBegin, argEnd, "-oo"))
  {
    fnOutputPtrn = getCmdOption(argBegin, argEnd, "-oo");

    if (fnOutputPtrn)
      std::cout << "-- Output Filename Pattern: " << fnOutputPtrn << std::endl;
  }

  bool isSeg = cmdOptionExists(argBegin, argEnd, "-seg");

  // typedefs
  constexpr unsigned int dim = 4;
  using PixelType = short;
  using Image4DType = itk::Image<PixelType, dim>;
  using Image3DType = itk::Image<PixelType, dim - 1>;
  using IndexValueType = typename itk::Index<dim>::IndexValueType;

  // reading the 4d image
  Image4DType::Pointer inputImage;
  try
  {
    std::cout << "-- Reading 4D Image..." << std::endl;
    inputImage = itk::ReadImage<Image4DType>(fnInput);
  }
  catch (const itk::ExceptionObject & err)
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }


  // extracting 3d slices
  using LinearInterpolatorType = itk::LinearInterpolateImageFunction<Image3DType, double>;
  using NNInterpolatorType = itk::NearestNeighborInterpolateImageFunction<Image3DType, double>;
  using ResampleFilterType = itk::ResampleImageFilter<Image3DType, Image3DType>;
  using ExtractFilterType = itk::ExtractImageFilter<Image4DType, Image3DType>;
  using Writer3DType = itk::ImageFileWriter<Image3DType>;
  using TransformType = itk::IdentityTransform<double, 3>;
  
  Image4DType::RegionType inputRegion = inputImage->GetLargestPossibleRegion();
  Image4DType::SizeType inputSize = inputRegion.GetSize();
  const unsigned int nT = inputSize[dim - 1];

  std::cout << "-- Total Number of Time Points: " << nT << std::endl;

  for (unsigned int crnt_tp = 0; crnt_tp < nT; ++crnt_tp)
  {
    std::cout << "-- Processing tp: " << crnt_tp << std::endl;
    
    auto extractFilter = ExtractFilterType::New();
    extractFilter->SetDirectionCollapseToSubmatrix();
    Image4DType::SizeType size = inputRegion.GetSize();
    size[dim - 1] = 0; // collapse the time dimension
    Image4DType::IndexType start = inputRegion.GetIndex();
    start[dim - 1] = crnt_tp;

    Image4DType::RegionType desiredRegion;
    desiredRegion.SetSize(size);
    desiredRegion.SetIndex(start);

    extractFilter->SetExtractionRegion(desiredRegion);
    extractFilter->SetInput(inputImage);
    extractFilter->Update();

    Image3DType::Pointer imageTail = extractFilter->GetOutput();

    // Resampling
    Image3DType::RegionType region = imageTail->GetLargestPossibleRegion();
    Image3DType::SizeType inputSize = region.GetSize();
    Image3DType::SpacingType inputSpacing = imageTail->GetSpacing();
    Image3DType::PointType inputOrigin = imageTail->GetOrigin();

    Image3DType::SizeType rsSize = inputSize;    
    Image3DType::SpacingType rsSpacing = inputSpacing;
    Image3DType::PointType rsOrigin = inputOrigin;

    for (unsigned int d = 0; d < dim - 1; ++d)
    {
      rsSize[d] = inputSize[d] * scale;
      rsSpacing[d] = inputSpacing[d] / scale;
      rsOrigin[d] = inputOrigin[d] + 0.5 * (rsSpacing[d] - inputSpacing[d]);
    }

    itk::SmartPointer<itk::InterpolateImageFunction<Image3DType, double>> interpolator;
    
    if (isSeg)
      interpolator = NNInterpolatorType::New();
    else
      interpolator = LinearInterpolatorType::New();

    auto resampleFilter = ResampleFilterType::New();
    resampleFilter->SetInput(imageTail);
    resampleFilter->SetInterpolator(interpolator);
    resampleFilter->SetSize(rsSize);
    resampleFilter->SetOutputSpacing(rsSpacing);
    resampleFilter->SetOutputOrigin(rsOrigin);
    resampleFilter->SetOutputDirection(imageTail->GetDirection());
    resampleFilter->SetDefaultPixelValue(0);
    resampleFilter->UpdateLargestPossibleRegion();
    imageTail = resampleFilter->GetOutput();

    // Convert to VTI
    using VTKFilterType = itk::ImageToVTKImageFilter<Image3DType>;
    auto vtkFilter = VTKFilterType::New();
    vtkFilter->SetInput(imageTail);
    vtkFilter->Update();

    vtkSmartPointer<vtkImageData> vtkTail = vtkFilter->GetOutput();

    vtkNew<vtkXMLImageDataWriter> vtkWriter;
    vtkWriter->SetInputData(vtkTail);
    vtkWriter->SetFileName(ssprintf(fnOutputPtrn, crnt_tp).c_str());
    vtkWriter->Write();
  }

  return 0;
}