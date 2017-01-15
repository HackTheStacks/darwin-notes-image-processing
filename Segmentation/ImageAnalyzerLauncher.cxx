#include "ImageAnalyzer.h"

int main( int argc, char * argv[] )
{
  if( argc < 2 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImageFile";
    return EXIT_FAILURE;
    }

  std::string baseImageDir = "/data/amnh/darwin/";

  ImageAnalyzer analyzer;

  analyzer.SetBaseDirectory(baseImageDir);
  analyzer.SetInputFilename(argv[1]);

  analyzer.ReadInputImage();

  analyzer.ThresholdInputImage();
  analyzer.WriteThresholdedImage();

  analyzer.ExtractLargestConnectedComponentImage();
  analyzer.WriteLargestConnectedComponentImage();

  analyzer.ComputeLinearScaleFromRuler();

  analyzer.FindImageMargins();

  analyzer.ExtractNorthSouthContours();
  analyzer.WriteNorthSouthContours();

  return EXIT_SUCCESS;
}
