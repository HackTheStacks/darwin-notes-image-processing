#include "ImageAnalyzer.h"


ImageAnalyzer::ImageAnalyzer() {
  m_ImageReader = ReaderType::New();
  m_ImageWriter = WriterType::New();
  m_OtsuFilter = OtsuFilterType::New();
  m_ConnectedComponents = ConnectedComponentImageFilterType::New();
  m_RelabelComponents = RelabelComponentsFilterType::New();
  m_Thresholder = ThresholdFilterType::New();
  m_VerticalCountImage = VerticalCountImageType::New();
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

  typedef itk::OtsuMultipleThresholdsImageFilter<
    VerticalCountImageType, VerticalCountImageType >  OtsuMultipleFilterType;

  OtsuMultipleFilterType::Pointer otsuMultipleFilter = OtsuMultipleFilterType::New();
  otsuMultipleFilter->SetInput( m_VerticalCountImage );
  otsuMultipleFilter->SetNumberOfThresholds(numberOfThresholds);
  otsuMultipleFilter->Update();

  VerticalCountImageType::Pointer otsuOutput = otsuMultipleFilter->GetOutput();

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
}

void ImageAnalyzer::FindImageMargins() {
  ComputeVerticalProjectionImage();
  ComputeMarginsWithOtsuThresholds(2);
  if (m_LeftMarginIndex[0] > m_RightMarginIndex[0]) {
    ComputeMarginsWithOtsuThresholds(1);
  }
}
