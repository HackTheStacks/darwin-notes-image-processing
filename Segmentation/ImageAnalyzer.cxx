#include "ImageAnalyzer.h"


ImageAnalyzer::ImageAnalyzer() {
  m_ImageReader = ReaderType::New();
  m_ImageWriter = WriterType::New();
  m_OtsuFilter = OtsuFilterType::New();
  m_ConnectedComponents = ConnectedComponentImageFilterType::New();
  m_RelabelComponents = RelabelComponentsFilterType::New();
  m_Thresholder = ThresholdFilterType::New();
  m_VerticalCountImage = VerticalCountImageType::New();
  m_OtsuMultipleFilter = OtsuMultipleFilterType::New();
  m_LinearScale = 1.0;
}


ImageAnalyzer::~ImageAnalyzer() {
}

void ImageAnalyzer::SetBaseDirectory(const std::string & directory) {
  m_BaseImageDir = directory;
  m_InputImageDir = m_BaseImageDir + "images/";
  m_SegmentationDir = m_BaseImageDir + "segmentations/";
  m_OutputCurvesDir = m_BaseImageDir + "image_csvs/";
}

void ImageAnalyzer::SetInputFilename(const std::string & filename) {
  m_BaseImageFilename = filename;
  m_InputFilename = m_InputImageDir + m_BaseImageFilename;
}

void ImageAnalyzer::ReadInputImage() {
  m_ImageReader->SetFileName(m_InputFilename);
  m_ImageReader->Update();
}

void ImageAnalyzer::ThresholdInputImage() {
  m_OtsuFilter->SetInput(m_ImageReader->GetOutput());
  m_OtsuFilter->SetOutsideValue(255);
  m_OtsuFilter->SetInsideValue(0);
  m_OtsuFilter->Update();
  int threshold = m_OtsuFilter->GetThreshold();
  std::cout << "Threshold = " << threshold << std::endl;
}

void ImageAnalyzer::WriteThresholdedImage() {
  std::string filename = m_SegmentationDir + m_BaseImageFilename;
  filename.erase(filename.end()-4, filename.end());
  filename += "_otsu.png";
  m_ImageWriter->SetInput(m_OtsuFilter->GetOutput());
  m_ImageWriter->SetFileName(filename);
  m_ImageWriter->Update();
}

void ImageAnalyzer::ExtractLargestConnectedComponentImage() {
  m_ConnectedComponents->SetInput(m_OtsuFilter->GetOutput());
  m_RelabelComponents->SetInput(m_ConnectedComponents->GetOutput());
  m_Thresholder->SetInput(m_RelabelComponents->GetOutput());
  m_Thresholder->SetLowerThreshold(1);
  m_Thresholder->SetUpperThreshold(1);
  m_Thresholder->SetOutsideValue(0);
  m_Thresholder->SetInsideValue(255);
  m_Thresholder->Update();
}

void ImageAnalyzer::ComputeLinearScaleFromRuler() {
  typedef std::vector< itk::SizeValueType > SizesInPixelsType;
  const SizesInPixelsType & sizesInPixels = m_RelabelComponents->GetSizeOfObjectsInPixels();

  if ( sizesInPixels.size() == 0 ) {
    std::cerr << "ERROR: No connected component was detected" << std::endl;
    return;
  }

  std::cout << "Number of pixels in largest component = " << sizesInPixels[0] << std::endl;

  if ( sizesInPixels.size() == 1 || sizesInPixels[1] == 0 ) {
    std::cerr << "ERROR: Ruler was not detected" << std::endl;
    return;
  }

  std::cout << "Number of pixels in second  component = " << sizesInPixels[1] << std::endl;
  m_LinearScale = std::sqrt(sizesInPixels[1]);
}

void ImageAnalyzer::WriteLargestConnectedComponentImage() {
  std::string filename = m_SegmentationDir + m_BaseImageFilename;
  filename.erase(filename.end()-4, filename.end());
  filename += "_largest_component.png";
  m_ImageWriter->SetInput(m_Thresholder->GetOutput());
  m_ImageWriter->SetFileName(filename);
  m_ImageWriter->Update();
}

void ImageAnalyzer::ComputeVerticalProjectionImage() {
  typedef itk::ImageLinearConstIteratorWithIndex< InputImageType > ConstIteratorType;

  InputImageType::ConstPointer componentImage = m_Thresholder->GetOutput();

  VerticalCountImageType::IndexType start;
  start[0] = 0;

  VerticalCountImageType::SizeType  size;
  size[0]  = componentImage->GetRequestedRegion().GetSize()[0];

  VerticalCountImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );

  m_VerticalCountImage = VerticalCountImageType::New();
  m_VerticalCountImage->SetRegions( region );
  m_VerticalCountImage->Allocate();

  CountIteratorType outputIt( m_VerticalCountImage, m_VerticalCountImage->GetLargestPossibleRegion() );
  outputIt.GoToBegin();

  ConstIteratorType inputIt( componentImage, componentImage->GetRequestedRegion() );

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
}

void ImageAnalyzer::ComputeMarginsWithOtsuThresholds(unsigned int numberOfThresholds) {
  m_OtsuMultipleFilter->SetInput( m_VerticalCountImage );
  m_OtsuMultipleFilter->SetNumberOfThresholds(numberOfThresholds);
  m_OtsuMultipleFilter->Update();

  WriteOtsuMultipleThresholds();

  VerticalCountImageType::Pointer otsuOutput = m_OtsuMultipleFilter->GetOutput();

  CountIteratorType bandsIt( otsuOutput, otsuOutput->GetLargestPossibleRegion() );
  bandsIt.GoToBegin();
  CountPixelType currentValue = bandsIt.Get();
  while (!bandsIt.IsAtEnd()) {
    if (bandsIt.Get() != currentValue) {
      m_LeftMarginIndex = bandsIt.GetIndex();
      currentValue = bandsIt.Get();
      break;
      }
    ++bandsIt;
    }
  while (!bandsIt.IsAtEnd()) {
    if (bandsIt.Get() != currentValue) {
      m_RightMarginIndex = bandsIt.GetIndex();
      break;
      }
    ++bandsIt;
    }

  // Add a safety band of 100 pixels
  m_LeftMarginIndex[0] += 100;
  m_RightMarginIndex[0] -= 100;

  std::cout << "Left  Margin = " << m_LeftMarginIndex[0] << std::endl;
  std::cout << "Right Margin = " << m_RightMarginIndex[0] << std::endl;
}

void ImageAnalyzer::WriteOtsuMultipleThresholds() {
  OtsuMultipleFilterType::ThresholdVectorType thresholds = m_OtsuMultipleFilter->GetThresholds();
  std::cout << "Otsu Multiple Thresholds = ";
  OtsuMultipleFilterType::ThresholdVectorType::const_iterator trit = thresholds.begin();
  while(trit != thresholds.end()) {
    std::cout << *trit << " ";
    ++trit;
    }
  std::cout << std::endl;
}

void ImageAnalyzer::FindImageMargins() {
  ComputeVerticalProjectionImage();
  ComputeMarginsWithOtsuThresholds(2);
  if (m_LeftMarginIndex[0] > m_RightMarginIndex[0]) {
    ComputeMarginsWithOtsuThresholds(1);
  }
}

void ImageAnalyzer::ExtractNorthSouthContours() {
  m_NorthContour.clear();
  m_SouthContour.clear();
  ContourCoordinateType xpos = 0;

  typedef itk::ImageLinearConstIteratorWithIndex< InputImageType > ConstIteratorType;
  InputImageType::ConstPointer inputImage = m_Thresholder->GetOutput();
  ConstIteratorType rangeIt( inputImage, inputImage->GetRequestedRegion() );
  rangeIt.SetDirection(1); // walk faster along the vertical direction.

  for (rangeIt.GoToBegin(); ! rangeIt.IsAtEnd(); rangeIt.NextLine(), ++xpos )
    {
    ContourCoordinateType bottom = 0;
    ContourCoordinateType top = 0;
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

    if (xpos >= m_LeftMarginIndex[0] && xpos <= m_RightMarginIndex[0]) {
      m_NorthContour.push_back(top);
      m_SouthContour.push_back(bottom);
      }
    }
}

void ImageAnalyzer::WriteNorthSouthContours() {
  std::string northFilename = m_OutputCurvesDir + m_BaseImageFilename;
  northFilename.erase(northFilename.end()-4, northFilename.end());
  northFilename += "_north.csv";

  std::string southFilename = m_OutputCurvesDir + m_BaseImageFilename;
  southFilename.erase(southFilename.end()-4, southFilename.end());
  southFilename += "_south.csv";

  std::cout << "northFilename " << northFilename << std::endl;
  std::cout << "southFilename " << southFilename << std::endl;

  std::ofstream northFile(northFilename.c_str());
  std::vector< ContourCoordinateType >::const_iterator curveIt = m_NorthContour.begin();
  ContourCoordinateType xpos = m_LeftMarginIndex[0];
  while (curveIt != m_NorthContour.end()) {
    northFile << xpos/m_LinearScale << ", " << *curveIt/m_LinearScale << std::endl;
    ++xpos;
    ++curveIt;
  }
  northFile.close();

  std::ofstream southFile(southFilename.c_str());
  curveIt = m_SouthContour.begin();
  xpos = m_LeftMarginIndex[0];
  while (curveIt != m_SouthContour.end()) {
    southFile << xpos/m_LinearScale << ", " << *curveIt/m_LinearScale << std::endl;
    ++xpos;
    ++curveIt;
  }
  southFile.close();
}
