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

  ConstIteratorType inputIt( inputImage, inputImage->GetRequestedRegion() );

  inputIt.SetDirection(1); // walk faster along the vertical direction.

  std::vector<unsigned short> counter;

  for ( inputIt.GoToBegin(); ! inputIt.IsAtEnd(); inputIt.NextLine() )
    {
    unsigned short count = 0;
    unsigned short line = 0;
    inputIt.GoToBeginOfLine();
    while ( ! inputIt.IsAtEndOfLine() )
      {
      PixelType value = inputIt.Get();
      if (value == 255 ) {
        ++count;
        }
      ++inputIt;
      ++line;
      }
    counter.push_back(count);
    std::cout << line << " " << count << std::endl;
    }

  return EXIT_SUCCESS;
}
