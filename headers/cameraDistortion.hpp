#ifndef _CAMERA_DISTORTION_H
#define _CAMERA_DISTORTION_H

#include "cameraCalibration.hpp"

namespace CameraDistortion{
  class cameraDistortion : public CameraCalibration::cameraCalibration {
  private:

  public:
    inline void pixelNormalized(double &px, double &py);
    inline void distortionModel(double& px, double& py);
    inline void normalizedPixel(double& px, double& py);
    void undistortedImage();
    void undistortedImage(const cv::Mat& distorted_image);
  };
}
#endif
