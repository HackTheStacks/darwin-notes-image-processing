#ifndef ImageAnalyzer_h
#define ImageAnalyzer_h

#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

class ImageAnalyzer {

public:

  typedef  unsigned char  InputPixelType;
  typedef  unsigned char  OutputPixelType;

  typedef itk::Image< InputPixelType,  2 >   InputImageType;
  typedef itk::Image< OutputPixelType, 2 >   OutputImageType;

  typedef itk::OtsuThresholdImageFilter<
               InputImageType, OutputImageType >  OtsuFilterType;

  typedef itk::ImageFileReader< InputImageType >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;


private:

  ReaderType::Pointer m_ImageReader;
  WriterType::Pointer m_ImageWriter;
  OtsuFilterType::Pointer m_OtsuFilter;

  std::string m_BaseImageDir;
  std::string m_BaseImageFilename;
  std::string m_InputFilename;
  std::string m_InputImageDir;
  std::string m_OutputCurvesDir;
  std::string m_SegmentationDir;

public:

  ImageAnalyzer();

  ~ImageAnalyzer();

  void SetBaseDirectory(const std::string & directory);

  void SetInputFilename(const std::string & filename);

  void ReadInputImage();

  void ThresholdInputImage();

  void WriteThresholdedImage();

};

#endif
