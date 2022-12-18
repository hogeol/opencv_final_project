#ifndef _CAMERA_CALIBRATION_H
#define _CAMERA_CALIBRATION_H

#define CAMERA_NUMBER 1

//STL
#include <filesystem>
#include <string>
//OPENCV
#include <opencv2/opencv.hpp>
//local
#include "cameraParams.hpp"

namespace CameraCalibration {
  class cameraCalibration : public CameraParams::cameraParams {
  private:
    std::filesystem::path m_board_path;
    cv::Mat m_board_image;
    cv::Size2i m_board_size;
  protected:
    CameraParams::cameraExternParams m_camera_params;
  public:
    void init(const std::string& board_image_save_path);
    void saveImage();
    void calculateCameraParams(const cv::Size2i& board_size = cv::Size2i{ 4,7 }, const bool &show_image = true);
    virtual void setParam(const double& fx, const double& fy, const double& cx, const double& cy, const double& k1, const double& k2, const double& k3, const double& p1, const double& p2, const double& skew_c);
    cv::Mat getBoardImage() const;
    cameraCalibration(void);
  };
}


#endif