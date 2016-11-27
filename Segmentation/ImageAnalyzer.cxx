#include "ImageAnalyzer.h"


ImageAnalyzer::ImageAnalyzer() {
  m_ImageReader = ReaderType::New();
  m_ImageWriter = WriterType::New();
  m_OtsuFilter = OtsuFilterType::New();

  m_OtsuFilter->SetInput(m_ImageReader->GetOutput());
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

