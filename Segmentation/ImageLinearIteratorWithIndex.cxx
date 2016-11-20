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

#include "itkImage.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"

int main( int argc, char *argv[] )
{
  if ( argc < 2 )
    {
    std::cerr << "Missing parameters. " << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 2;

  typedef unsigned char PixelType;
  typedef itk::Image< PixelType, Dimension > ImageType;

  typedef itk::ImageLinearConstIteratorWithIndex< ImageType > ConstIteratorType;

  typedef itk::ImageFileReader< ImageType > ReaderType;

  ImageType::ConstPointer inputImage;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    inputImage = reader->GetOutput();
    }
  catch ( itk::ExceptionObject &err)
    {
    std::cerr << "ExceptionObject caught a !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

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
      PixelType value = inputIt.Get();
      if (value == 255 ) {
        ++count;
        }
      ++inputIt;
      }
    outputIt.Set(count);
    ++outputIt;
    counter.push_back(count);
    std::cout << line << " " << count << std::endl;
    ++line;
    }

  CountImageType::IndexType leftIndex;
  CountImageType::IndexType rightIndex;

  typedef itk::OtsuMultipleThresholdsImageFilter< 
    CountImageType, CountImageType >  OtsuFilterType;
  OtsuFilterType::Pointer otsuFilter = OtsuFilterType::New();
  otsuFilter->SetInput( countImage );
  // otsuFilter->SetNumberOfHistogramBins(100);
  otsuFilter->SetNumberOfThresholds(2);
  otsuFilter->Update();
  CountImageType::Pointer otsuOutput = otsuFilter->GetOutput();

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

  std::cout << "leftIndex " << leftIndex << std::endl;
  std::cout << "rightIndex " << rightIndex << std::endl;

  return EXIT_SUCCESS;
}
