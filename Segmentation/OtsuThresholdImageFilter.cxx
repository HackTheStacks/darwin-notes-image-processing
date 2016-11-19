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

#include "itkOtsuThresholdImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int main( int argc, char * argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImageFile outputImageFile ";
    std::cerr << " insideValue    outsideValue   "  << std::endl;
    return EXIT_FAILURE;
    }

  typedef  unsigned char  InputPixelType;
  typedef  unsigned char  OutputPixelType;
  const unsigned int      Dimension = 2;

  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;

  typedef itk::OtsuThresholdImageFilter<
               InputImageType, OutputImageType >  FilterType;

  typedef itk::ImageFileReader< InputImageType >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  FilterType::Pointer filter = FilterType::New();

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  reader->SetFileName( argv[1] );

  filter->SetInput( reader->GetOutput() );

  const OutputPixelType outsideValue = atoi( argv[3] );
  const OutputPixelType insideValue  = atoi( argv[4] );

  filter->SetOutsideValue( outsideValue );
  filter->SetInsideValue(  insideValue  );

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << excp << std::endl;
    }

  int threshold = filter->GetThreshold();
  std::cout << "Threshold = " << threshold << std::endl;

  writer->SetFileName( argv[2] );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << excp << std::endl;
    }

  return EXIT_SUCCESS;
}
