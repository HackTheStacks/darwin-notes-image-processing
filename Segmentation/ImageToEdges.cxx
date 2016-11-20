/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/


#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

int main( int argc, char * argv[] )
{
  if( argc < 2 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImageFile";
    return EXIT_FAILURE;
    }

  std::string inputImageDir = "/data/amnh/darwin/image";
  std::string outputCurvesDir = "/data/amnh/darwin/image_csvs";

  typedef  unsigned char  InputPixelType;
  typedef  unsigned char  OutputPixelType;
  const unsigned int      Dimension = 2;

  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;

  typedef itk::OtsuThresholdImageFilter<
               InputImageType, OutputImageType >  OtsuFilterType;

  typedef itk::ImageFileReader< InputImageType >  ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  OtsuFilterType::Pointer otsuFilter = OtsuFilterType::New();

  std::string inputFilename = inputImageDir + argv[1];
  reader->SetFileName(inputFilename);

  otsuFilter->SetInput( reader->GetOutput() );

  const OutputPixelType outsideValue = 255;
  const OutputPixelType insideValue  = 0;

  otsuFilter->SetOutsideValue( outsideValue );
  otsuFilter->SetInsideValue(  insideValue  );

  try
    {
    otsuFilter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << excp << std::endl;
    }

  int threshold = otsuFilter->GetThreshold();

  typedef unsigned short ComponentPixelType;
  typedef itk::Image< ComponentPixelType, Dimension >  ComponentImageType;

  typedef itk::ConnectedComponentImageFilter <InputImageType, ComponentImageType >
    ConnectedComponentImageFilterType;

  ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New ();
  connected->SetInput(otsuFilter->GetOutput());
  connected->Update();

  typedef itk::RelabelComponentImageFilter<
               ComponentImageType, ComponentImageType >  RelabelFilterType;
  RelabelFilterType::Pointer relabeler = RelabelFilterType::New();

  relabeler->SetInput( connected->GetOutput() );

  typedef itk::BinaryThresholdImageFilter<
               ComponentImageType, OutputImageType >  ThresholdFilterType;
  ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
  thresholder->SetLowerThreshold(1);
  thresholder->SetUpperThreshold(1);
  thresholder->SetOutsideValue(0);
  thresholder->SetInsideValue(255);

  thresholder->SetInput( relabeler->GetOutput() );

  typedef std::vector< itk::SizeValueType > SizesInPixelsType;
  const SizesInPixelsType & sizesInPixels = relabeler->GetSizeOfObjectsInPixels();
  std::cout << "Number of pixels in largest component = " << sizesInPixels[0] << std::endl;
  std::cout << "Number of pixels in second  component = " << sizesInPixels[1] << std::endl;

  if ( sizesInPixels[1] == 0 ) {
    std::cerr << "Ruler was not detected" << std::endl;
    return EXIT_FAILURE;
  }

  const double linearScale = std::sqrt(sizesInPixels[1]);

  //
  // Compute integrated projection and use Otsu to find borders.
  // 
  typedef itk::ImageLinearConstIteratorWithIndex< InputImageType > ConstIteratorType;

  InputImageType::ConstPointer inputImage = thresholder->GetOutput();

  const unsigned int CountDimension = 1;
  typedef unsigned short CountPixelType;
  typedef itk::Image< CountPixelType, CountDimension > CountImageType;
  CountImageType::IndexType start;
  start[0] = 0;
  CountImageType::SizeType  size;
  size[0]  = inputImage->GetRequestedRegion().GetSize()[0];

  CountImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );

  CountImageType::Pointer countImage = CountImageType::New();
  countImage->SetRegions( region );
  countImage->Allocate();

  typedef itk::ImageRegionIterator< CountImageType > CountIteratorType;
  CountIteratorType outputIt( countImage, countImage->GetLargestPossibleRegion() );
  outputIt.GoToBegin();

  ConstIteratorType inputIt( inputImage, inputImage->GetRequestedRegion() );

  inputIt.SetDirection(1); // walk faster along the vertical direction.

  std::vector<unsigned short> counter;

  unsigned short line = 0;
  for ( inputIt.GoToBegin(); ! inputIt.IsAtEnd(); inputIt.NextLine() )
    {
    unsigned short count = 0;
    inputIt.GoToBeginOfLine();
    while ( ! inputIt.IsAtEndOfLine() )
      {
      InputPixelType value = inputIt.Get();
      if (value == 255 ) {
        ++count;
        }
      ++inputIt;
      }
    outputIt.Set(count);
    ++outputIt;
    counter.push_back(count);
    ++line;
    }

  CountImageType::IndexType leftIndex;
  CountImageType::IndexType rightIndex;

  typedef itk::OtsuMultipleThresholdsImageFilter< 
    CountImageType, CountImageType >  OtsuMultipleFilterType;
  OtsuMultipleFilterType::Pointer otsuMultipleFilter = OtsuMultipleFilterType::New();
  otsuMultipleFilter->SetInput( countImage );
  otsuMultipleFilter->SetNumberOfThresholds(2);
  otsuMultipleFilter->Update();
  CountImageType::Pointer otsuOutput = otsuMultipleFilter->GetOutput();

  CountIteratorType bandsIt( otsuOutput, otsuOutput->GetLargestPossibleRegion() );
  bandsIt.GoToBegin();
  CountPixelType currentValue = bandsIt.Get();
  while (!bandsIt.IsAtEnd()) {
    if (bandsIt.Get() != currentValue) {
      leftIndex = bandsIt.GetIndex();
      currentValue = bandsIt.Get();
      break;
      }
    ++bandsIt;
    }
  while (!bandsIt.IsAtEnd()) {
    if (bandsIt.Get() != currentValue) {
      rightIndex = bandsIt.GetIndex();
      break;
      }
    ++bandsIt;
    }

  // Add a safety band
  leftIndex[0] += 100;
  rightIndex[0] -= 100;

  std::vector< unsigned short > north;
  std::vector< unsigned short > south;

  unsigned short xpos = 0;
  ConstIteratorType rangeIt( inputImage, inputImage->GetRequestedRegion() );
  rangeIt.SetDirection(1); // walk faster along the vertical direction.

  for ( rangeIt.GoToBegin(); ! rangeIt.IsAtEnd(); rangeIt.NextLine(), ++xpos )
    {
    unsigned short bottom = 0;
    unsigned short top = 0;
    rangeIt.GoToBeginOfLine();
    while ( ! rangeIt.IsAtEndOfLine() )
      {
      InputPixelType value = rangeIt.Get();
      if (value == 255 ) {
        if ( bottom == 0 ) {
          bottom = rangeIt.GetIndex()[1];  
          }
        top = rangeIt.GetIndex()[1];  
        }
      ++rangeIt;
      }

    if (xpos >= leftIndex[0] && xpos <= rightIndex[0]) {
      north.push_back(top);
      south.push_back(bottom);
      }
    }

  std::string northFilename = outputCurvesDir + argv[1];
  northFilename.erase(northFilename.end()-4, northFilename.end());
  northFilename += "_north.csv";

  std::string southFilename = outputCurvesDir + argv[1];
  southFilename.erase(southFilename.end()-4, southFilename.end());
  southFilename += "_south.csv";

  std::ofstream northFile(northFilename.c_str());
  std::vector<unsigned short>::const_iterator curveIt = north.begin();
  xpos = leftIndex[0];
  while (curveIt != north.end()) {
    northFile << xpos/linearScale << ", " << *curveIt/linearScale << std::endl;
    ++xpos;
    ++curveIt;
  }
  northFile.close();
 
  std::ofstream southFile(southFilename.c_str());
  curveIt = south.begin();
  xpos = leftIndex[0];
  while (curveIt != south.end()) {
    southFile << xpos/linearScale << ", " << *curveIt/linearScale << std::endl;
    ++xpos;
    ++curveIt;
  }
  southFile.close();

  return EXIT_SUCCESS;
}
