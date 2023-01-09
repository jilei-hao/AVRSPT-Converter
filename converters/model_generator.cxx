#include <iostream>
#include <string>

int main (int argc, char *argv[])
{
  std::cout << "======================================\n";
  std::cout << "-- AVRSPT Model Generator\n";
  std::cout << "======================================\n";

  if (argc < 3)
  {
    std::cout << "usage: ModelGen input.nii.gz output vtp" << std::endl;
    return EXIT_FAILURE;
  }

  std::string fnIn = argv[1];
  std::string fnOut = argv[2];

  std::cout << "-- Input File Name: " << fnIn << std::endl;
  std::cout << "-- Output File Name: " << fnOut << std::endl;

  return EXIT_SUCCESS;
}