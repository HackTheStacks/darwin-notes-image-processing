#include "ImageAnalyzer.h"

int main( int argc, char * argv[] )
{
  if( argc < 2 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImageFile";
    return EXIT_FAILURE;
    }

  // baseImageDir in cloud server: "/data/amnh/darwin"
  std::string baseImageDir = "/home/ibanez/data/amnh/darwin_notes/";

  ImageAnalyzer analyzer;

  analyzer.SetBaseDirectory(baseImageDir);
  analyzer.SetInputFilename(argv[1]);

  analyzer.ReadInputImage();

  analyzer.ThresholdInputImage();
  analyzer.WriteThresholdedImage();

  analyzer.ExtractLargestConnectedComponentImage();
  analyzer.WriteLargestConnectedComponentImage();

  return EXIT_SUCCESS;
}
