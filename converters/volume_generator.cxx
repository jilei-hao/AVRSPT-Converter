#include <algorithm>
#include <iostream>
#include <itkImageFileReader.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkExtractImageFilter.h>
#include <itkImageFileWriter.h>

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
  }



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
    inputImage = itk::ReadImage<Image4DType>(fnInput);
  }
  catch (const itk::ExceptionObject & err)
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  // extracting 3d slices
  Image4DType::RegionType inputRegion = inputImage->GetBufferedRegion();
  Image4DType::SizeType   size = inputRegion.GetSize();
  unsigned int nT = inputRegion.GetSize()[dim - 1];

  for (unsigned int crnt_tp = 0; crnt_tp < nT; ++crnt_tp)
  {
    std::cout << "-- Processing tp: " << crnt_tp << std::endl;

    using ExtractFilterType = itk::ExtractImageFilter<Image4DType, Image3DType>;
    auto extractFilter = ExtractFilterType::New();
    extractFilter->SetDirectionCollapseToSubmatrix();

    size[dim - 1] = 0; // collapse the time dimension
    Image4DType::IndexType start = inputRegion.GetIndex();

    const unsigned int sliceNumber = crnt_tp;
    start[dim - 1] = sliceNumber;
    Image4DType::RegionType desiredRegion;
    desiredRegion.SetSize(size);
    desiredRegion.SetIndex(start);

    extractFilter->SetExtractionRegion(desiredRegion);
    extractFilter->SetInput(inputImage);
    extractFilter->Update();

    Image3DType::Pointer imageTail = extractFilter->GetOutput();

    // Resampling
    Image3DType::RegionType region = imageTail->GetLargestPossibleRegion();
    Image3DType::SizeType rsSize = region.GetSize();
    Image3DType::SpacingType rsSpacing = imageTail->GetSpacing();
    Image3DType::PointType rsOrigin = imageTail->GetOrigin();


    // Write output image slice
    using Writer3DType = itk::ImageFileWriter<Image3DType>;

    if (fnOutputPtrn)
    {
      Writer3DType::Pointer writer = Writer3DType::New();
      writer->SetInput(imageTail);
      writer->SetFileName(ssprintf(fnOutputPtrn, crnt_tp));
      writer->Write();
    }
  }
  // // resampling
  // const typename Image3DType::PointType rsOrigin = imageTail->GetOrigin();
  // typename Image3DType::SizeType rsSize = imageTail->GetSize();
  // typename Image3DType::SpacingType rsSpacing = imageTail->GetSpacing();
  // typename Image3DType::PointType  rsOrigin = imageTail->GetOrigin();

  // for (unsigned int d = 0; d < dim; ++d)
  // {
  //   outputSize[d] = inputSize[d] * scale;
  //   outputSpacing[d] = inputSpacing[d] / scale;
  //   rsOrigin[d] = inputOrigin[d] + 0.5 * (outputSpacing[d] - inputSpacing[d]);
  // }

  

  // using LinearInterpolatorType = itk::LinearInterpolateImageFunction<Image3DType, double>;
  // auto interpolator = LinearInterpolatorType::New();

  // using ResampleFilterType = itk::ResampleImageFilter<Image3DType, Image3DType>;

  // ResampleFilterType::InterpolatorType *interpolator;

  // auto resampleFilter = ResampleFilterType::New();

  // resampleFilter->SetInput(inputImage);
  // resampleFilter->SetInterpolator(interpolator);
  // resampleFilter->SetSize(outputSize);
  // resampleFilter->SetOutputSpacing(outputSpacing);
  // resampleFilter->SetOutputOrigin(outputOrigin);

  // auto imgRs = resampleFilter->GetOutput();



  return 0;
}