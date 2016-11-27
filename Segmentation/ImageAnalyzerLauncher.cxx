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
  std::string inputImageDir = baseImageDir + "images/";
  std::string inputFilename = inputImageDir + argv[1];

  ImageAnalyzer analyzer;

  analyzer.SetInputFilename(inputFilename);
  analyzer.ReadInputImage();
  analyzer.ThresholdInputImage();

  return EXIT_SUCCESS;
}
