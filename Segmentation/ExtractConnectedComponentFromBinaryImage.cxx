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
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

int main( int argc, char *argv[])
{
  if( argc < 3 )
    {
    std::cout << "Usage:" << std::endl;
    std::cout << argv[0] << " InputFileName OutputFileName" << std::endl;
    }

  const unsigned int Dimension = 2;

  typedef unsigned char InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >  ImageType;

  typedef unsigned short ComponentPixelType;
  typedef itk::Image< ComponentPixelType, Dimension >  ComponentImageType;

  typedef itk::ImageFileReader< ImageType >   ReaderType;
  typedef itk::ImageFileWriter< ImageType >   WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[2] );

  typedef itk::ConnectedComponentImageFilter <ImageType, ComponentImageType >
    ConnectedComponentImageFilterType;

  ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New ();
  connected->SetInput(reader->GetOutput());
  connected->Update();

  std::cout << "Number of objects: " << connected->GetObjectCount() << std::endl;

  typedef itk::RelabelComponentImageFilter<
               ComponentImageType, ComponentImageType >  RelabelFilterType;
  RelabelFilterType::Pointer relabeler = RelabelFilterType::New();

  relabeler->SetInput( connected->GetOutput() );

  typedef itk::BinaryThresholdImageFilter<
               ComponentImageType, ImageType >  ThresholdFilterType;
  ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
  thresholder->SetLowerThreshold(1);
  thresholder->SetUpperThreshold(1);
  thresholder->SetOutsideValue(0);
  thresholder->SetInsideValue(255);

  thresholder->SetInput( relabeler->GetOutput() );
  writer->SetInput( thresholder->GetOutput() );
  writer->Update();

  std::cout << "Largest object of " << connected->GetObjectCount() << " objects";

  typedef std::vector< itk::SizeValueType > SizesInPixelsType;
  const SizesInPixelsType & sizesInPixels = relabeler->GetSizeOfObjectsInPixels();
  std::cout << "Number of pixels in largest component = " << sizesInPixels[0] << std::endl;
  std::cout << "Number of pixels in second  component = " << sizesInPixels[1] << std::endl;

  return EXIT_SUCCESS;
}
