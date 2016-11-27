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

  typedef unsigned short ComponentPixelType;
  typedef itk::Image< ComponentPixelType, 2 >  ComponentImageType;

  typedef itk::ConnectedComponentImageFilter <InputImageType, ComponentImageType >
    ConnectedComponentImageFilterType;

  typedef itk::RelabelComponentImageFilter<
               ComponentImageType, ComponentImageType >  RelabelComponentsFilterType;

  typedef itk::BinaryThresholdImageFilter<
               ComponentImageType, OutputImageType >  ThresholdFilterType;

  typedef unsigned short CountPixelType;
  typedef itk::Image< CountPixelType, 1 > VerticalCountImageType;
  typedef itk::ImageRegionIterator< VerticalCountImageType > CountIteratorType;

  typedef itk::OtsuMultipleThresholdsImageFilter<
    VerticalCountImageType, VerticalCountImageType >  OtsuMultipleFilterType;

  typedef unsigned short ContourCoordinateType;

private:

  ReaderType::Pointer m_ImageReader;
  WriterType::Pointer m_ImageWriter;
  OtsuFilterType::Pointer m_OtsuFilter;
  ConnectedComponentImageFilterType::Pointer m_ConnectedComponents;
  RelabelComponentsFilterType::Pointer m_RelabelComponents;
  ThresholdFilterType::Pointer m_Thresholder;
  VerticalCountImageType::Pointer m_VerticalCountImage;
  OtsuMultipleFilterType::Pointer m_OtsuMultipleFilter;

  std::string m_BaseImageDir;
  std::string m_BaseImageFilename;
  std::string m_InputFilename;
  std::string m_InputImageDir;
  std::string m_OutputCurvesDir;
  std::string m_SegmentationDir;

  double m_LinearScale;

  VerticalCountImageType::IndexType m_LeftMarginIndex;
  VerticalCountImageType::IndexType m_RightMarginIndex;

  std::vector< ContourCoordinateType > m_NorthContour;
  std::vector< ContourCoordinateType > m_SouthContour;

public:

  ImageAnalyzer();

  ~ImageAnalyzer();

  void SetBaseDirectory(const std::string & directory);

  void SetInputFilename(const std::string & filename);

  void ReadInputImage();

  void ThresholdInputImage();

  void WriteThresholdedImage();

  void ExtractLargestConnectedComponentImage();

  void WriteLargestConnectedComponentImage();

  void ComputeLinearScaleFromRuler();

  void ComputeVerticalProjectionImage();

  void ComputeMarginsWithOtsuThresholds(unsigned int);

  void WriteOtsuMultipleThresholds();

  void WriteVerticalCountValues();

  void FindImageMargins();

  void ExtractNorthSouthContours();

  void WriteNorthSouthContours();

};

#endif
