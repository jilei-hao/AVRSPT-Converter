#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <vtkNIFTIImageReader.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include <PropagationAPI.h>
#include <PropagationIO.h>
#include <PropagationTools.h>

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

void usage () 
{
  std::cout << "Usage: CaseGen img4d outdir -s [seg-propagation-configs] [case-configs]\n";
  std::cout << "-- img4d: the base 4d grey image for all the processing\n";
  std::cout << "-- outdir: directory to store all the output files\n";
  std::cout << "-- seg-propagation-configs: -s seg3d tp_ref tp_target_list\n";
  std::cout << "   tp_target_list: comma separated list of the target time points\n";
  std::cout << "   use multiple -s to specify multiple segmentation configs\n";
  std::cout << "-- case-configs: \n";
  std::cout << "   -dc [decimation_percentage] (0: no decimation, 99: decimate 99\% polygons)\n";
  std::cout << "   -rs [resampling_percentage] (100: no reampling, 50: 50\% resampling)\n";
  std::cout << std::endl;
  std::cout << "Example: Following command will generate result by propagating seg02.nii.gz ";
  std::cout << "from time point 2 to time points 1,3,4,5,6, and seg09.nii.gz from timepoint 9 ";
  std::cout << "to time points 7,8,10,11,12,13,14,15,16. The image result will be resampled to 40% ";
  std::cout << "to the original resolution; The mesh result will be decimated by 50%. ";
  std::cout << "All the results will be stored at folder named output_dir.\n";
  std::cout << std::endl;
  std::cout << "CaseGen input/img4d.nii.gz output_dir \\\n";
  std::cout << "  -s seg02.nii.gz 2 1,3,4,5,6\n";
  std::cout << "  -s seg09.nii.gz 9 7,8,10,11,12,13,14,15,16\n";
  std::cout << "  -dc 50 -rs 40\n";
}

struct SegConfig
{
  std::string fnSeg;
  unsigned int tpr;
  std::vector<unsigned int> tpt; 
};

int main (int argc, char *argv[])
{
  std::cout << "======================================\n";
  std::cout << "-- AVRSPT Case Generator\n";
  std::cout << "======================================\n";

  // // -- PARSE COMMAND ----------
  // if (argc < 6)
  // {
  //   usage();
  //   return EXIT_FAILURE;
  // }

  // // parse image4d and the outdir first
  // std::string fnImage4D = argv[1];
  // std::string dirOutput;
  // char ** argBegin = argv;
  // char ** argEnd = argv + argc;

  std::string fnImage4D = "/Users/jileihao/data/avrspt/tav48/img4d_tav48_LAS.nii.gz";
  std::string dirOutput = "/Users/jileihao/data/avrspt/tav48/case_output";
  std::vector<SegConfig> segConfigs;

  SegConfig scSys, scDias;
  
  scSys.fnSeg = "/Users/jileihao/data/avrspt/tav48/seg02_tav48_NY.nii.gz";
  scSys.tpr = 2;
  scSys.tpt = std::vector<unsigned int>{1, 3, 4, 5};

  scDias.fnSeg = "/Users/jileihao/data/avrspt/tav48/seg09_tav48_NY.nii.gz";
  scDias.tpr = 9;
  scDias.tpt = std::vector<unsigned int>{6, 7, 8, 10, 11, 12, 13, 14};

  segConfigs.push_back(scSys);
  segConfigs.push_back(scDias);

  unsigned int dc = 50, rs = 40;

  // Running propagation
  using namespace propagation;
  typedef float TReal;
  typedef PropagationAPI<TReal> PropagationAPIType;
  typedef PropagationInput<TReal> PropagationInputType;
  typedef PropagationInputBuilder<TReal> PropaInputBuilderType;
  typedef PropagationTools<TReal> PropaTools;
  typedef typename PropagationAPIType::TImage4D TImage4D;
  typedef typename PropagationAPIType::TImage3D TImage3D;
  typedef typename PropagationAPIType::TLabelImage3D TLabelImage3D;


  // -- run propagation for each segmentation config in the list

  std::shared_ptr<PropagationAPIType> propaAPI;
  std::shared_ptr<PropaInputBuilderType> ibuilder = 
    std::make_shared<PropaInputBuilderType>();

  TImage4D::Pointer img4d = PropaTools::ReadImage<TImage4D>(fnImage4D);

  img4d->Print(std::cout);

  // ibuilder->SetImage4D();
  // ibuilder->SetReferenceSegmentationIn3D(PropaTools::ReadImag)
  


  
  


  return EXIT_SUCCESS;
}