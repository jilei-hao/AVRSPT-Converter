#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>

#include <itkImageFileReader.h>
#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkIdentityTransform.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>
#include <itkRecursiveGaussianImageFilter.h>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include "mesh_helpers.h"
#include "common_tools.h"

typedef uint16_t LabelType;
typedef double RealType;
typedef itk::Image<LabelType, 3u> LabelImage3DType;

static RealType REAL_INF = std::numeric_limits<RealType>::infinity();

struct ImageStepConfig
{
  uint16_t resampleRate;
  double gaussianSigma;
  ImageStepConfig(uint16_t rs, double sigma)
    :resampleRate(rs), gaussianSigma(sigma) {};
};

struct MeshStepConfig
{
  double decimateRate;
  uint16_t smoothIteration = 20;
  double smoothPassband = 0.1;
  uint16_t smoothFeatureAngle = 0;
  MeshStepConfig(double dc, uint16_t iter, double pb, uint16_t fa)
    : decimateRate(dc), smoothIteration(iter), smoothPassband(pb), smoothFeatureAngle(fa) {};
};

struct LabelConfig
{
  std::vector<ImageStepConfig> imgConfigs;
  std::vector<MeshStepConfig> meshConfigs;
};

LabelConfig createLabelConfig(char size) // S, M, L
{
  LabelConfig ret;
  ret.imgConfigs.push_back(ImageStepConfig(200, 2));
  ret.imgConfigs.push_back(ImageStepConfig(200, 2));

  switch(size)
  {
    case 'S': 
    {
      ret.meshConfigs.push_back(MeshStepConfig(0.2, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.5, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.5, 20, 0.1, 0));
      break;
    }
    case 'M':
    {
      ret.meshConfigs.push_back(MeshStepConfig(0.2, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.5, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.5, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.5, 20, 0.1, 0));
      break;
    }
    case 'L':
    {
      ret.meshConfigs.push_back(MeshStepConfig(0.2, 100, 0.01, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.75, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.75, 20, 0.1, 0));
      break;
    }
  }

  return ret;
}

LabelImage3DType::Pointer
resampleImage(LabelImage3DType::Pointer img, uint16_t rate)
{
  std::cout << "-- [resampleImage] rate: " << rate << std::endl;
  double ratio = (double)rate/100.0;

  typedef itk::NearestNeighborInterpolateImageFunction<LabelImage3DType> NNInterpolatorType;
  
  auto fltResample = itk::ResampleImageFilter<LabelImage3DType, LabelImage3DType>::New();
  fltResample->SetInput(img);

  auto transform = itk::IdentityTransform<double, 3>::New();
  auto interpolator = NNInterpolatorType::New();
  transform->SetIdentity();
  fltResample->SetTransform(transform);
  fltResample->SetInterpolator(interpolator);

  const LabelImage3DType::RegionType &inRegion = img->GetLargestPossibleRegion();
  const LabelImage3DType::SizeType &inSize = inRegion.GetSize();
  auto &inDirection = img->GetDirection();
  auto &inOrigin = img->GetOrigin();

  // set out spacing.
  const LabelImage3DType::SpacingType &inSpacing = img->GetSpacing();
  double outSpacing[3];
  LabelImage3DType::SizeType outSize;
  for (int i = 0; i < 3; ++i)
  {
    outSpacing[i] = inSpacing[i] / ratio;
    outSize[i] = inSize[i] * ratio;
  }

  fltResample->SetOutputOrigin(inOrigin);
  fltResample->SetSize(outSize);
  fltResample->SetOutputSpacing(outSpacing);
  fltResample->SetOutputDirection(inDirection);
  fltResample->Update();

  std::cout << "---- before region: " << inRegion << std::endl;
  img->Print(std::cout);
  std::cout << "---- after region: " << fltResample->GetOutput()->GetLargestPossibleRegion() << std::endl;
  fltResample->GetOutput()->Print(std::cout);
  
  return fltResample->GetOutput();
}

template<typename TInputPixel, typename TOutputPixel>
typename itk::Image<TOutputPixel, 3u>::Pointer
thresholdImage(typename itk::Image<TInputPixel, 3u>::Pointer img, TInputPixel lo, TInputPixel hi,
  TOutputPixel in, TOutputPixel out)
{
  std::cout << "-- [thresholdImage] lo: " << lo << "; hi: " << hi 
            << "; in: " << in << "; out: " << out << std::endl;

  // threshold
  auto fltThreshold = 
    itk::BinaryThresholdImageFilter<itk::Image<TInputPixel, 3u>, itk::Image<TOutputPixel, 3u>>::New();

  fltThreshold->SetInput(img);
  fltThreshold->SetLowerThreshold(lo);
  fltThreshold->SetUpperThreshold(hi);
  fltThreshold->SetInsideValue(in);
  fltThreshold->SetOutsideValue(out);
  fltThreshold->Update();

  return fltThreshold->GetOutput();
}

LabelImage3DType::Pointer
gaussianSmooth(LabelImage3DType::Pointer img, double sigma)
{
  std::cout << "-- [gaussianSmooth] sigma: " << sigma << std::endl;
  typedef itk::Image<RealType, 3u> RealImage3DType;
  auto fltGaussian = itk::RecursiveGaussianImageFilter<LabelImage3DType, RealImage3DType>::New();

  fltGaussian->SetInput(img);
  fltGaussian->SetSigma(sigma);
  fltGaussian->Update();

  auto imageTail = fltGaussian->GetOutput();

  return thresholdImage<RealType, LabelType>(imageTail, 0.5, REAL_INF, 1u, 0u);

}

LabelImage3DType::Pointer 
processImage(LabelImage3DType::Pointer img_ml, LabelType label, 
  std::vector<ImageStepConfig> &config)
{
  std::cout << "[processImage] label: " << label << std::endl;
  LabelImage3DType::Pointer ret;

  // threshold
  auto imageTail = thresholdImage<LabelType, LabelType>(img_ml, label, label, 1u, 0u);

  int cnt = 0;
  for (ImageStepConfig &isc : config)
  {
    ++cnt;
    // upsample
    auto img_rs = resampleImage(imageTail, isc.resampleRate);
    auto fndebug = CommonTools::ssprintf("/Users/jileihao/data/avrspt/smoothing/trim_%02d_rs_iter_%02d.nii.gz", label, cnt);
    itk::WriteImage(img_rs, fndebug);

    imageTail = gaussianSmooth(img_rs, isc.gaussianSigma);
    fndebug = CommonTools::ssprintf("/Users/jileihao/data/avrspt/smoothing/trim_%02d_sm_iter_%02d.nii.gz", label, cnt);
    itk::WriteImage(imageTail, fndebug);

  }

  

  // create

  return ret;
}


int main (int argc, char *argv[])
{
  std::cout << "======================================\n";
  std::cout << "-- AVRSPT Model Generator\n";
  std::cout << "======================================\n";

  if (argc < 3)
  {
    std::cout << "usage: ModelGen input.nii.gz output.vtp" << std::endl;
    return EXIT_FAILURE;
  }

  std::string fnImageIn = argv[1];
  std::string fnMeshOut = argv[2];

  std::cout << "-- Input File Name: " << fnImageIn << std::endl;
  std::cout << "-- Output File Name: " << fnMeshOut << std::endl;

  // read image
  auto imgIn = itk::ReadImage<LabelImage3DType>(fnImageIn);
  imgIn->Print(std::cout);

  std::map<LabelType, LabelConfig> labelConfigs;
  labelConfigs[1] = createLabelConfig('M');
  labelConfigs[2] = createLabelConfig('M');
  labelConfigs[3] = createLabelConfig('M');
  labelConfigs[4] = createLabelConfig('L');
  labelConfigs[5] = createLabelConfig('S');
  labelConfigs[6] = createLabelConfig('S');

  auto imageTail = gaussianSmooth(imgIn, 2);
  auto fndebug = CommonTools::ssprintf("/Users/jileihao/data/avrspt/smoothing/trim_sm.nii.gz");
  itk::WriteImage(imageTail, fndebug);
  

  for (auto kv : labelConfigs)
  {
    auto img_sm = processImage(imgIn, kv.first, kv.second.imgConfigs);
    return 0;
  }









  return EXIT_SUCCESS;
}