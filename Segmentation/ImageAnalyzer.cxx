#include "ImageAnalyzer.h"


ImageAnalyzer::ImageAnalyzer() {
  m_ImageReader = ReaderType::New();
  m_ImageWriter = WriterType::New();
  m_OtsuFilter = OtsuFilterType::New();

  m_OtsuFilter->SetInput(m_ImageReader->GetOutput());
}


ImageAnalyzer::~ImageAnalyzer() {
}


void ImageAnalyzer::SetInputFilename(const std::string & filename) {
  m_InputFilename = filename;
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


