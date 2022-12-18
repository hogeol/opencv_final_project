#include "cameraDistortion.hpp"

namespace CameraDistortion {
  inline void CameraDistortion::cameraDistortion::pixelNormalized(double& px, double& py)
  {
    double y_n_u{ (py - m_camera_params.cy) / m_camera_params.fy };
    double x_n_u{ (px - m_camera_params.cx / m_camera_params.fx - (m_camera_params.skew_c * y_n_u)) };
    distortionModel(x_n_u, y_n_u);
  }

  inline void CameraDistortion::cameraDistortion::distortionModel(double& px, double& py)
  {
    double r_u2{ px * px + py * py };
    double radial_distortion{ 1 + m_camera_params.k1 * r_u2 + m_camera_params.k2 * r_u2 * r_u2 + m_camera_params.k3 * r_u2 * r_u2 * r_u2 };

    double x_n_d{ radial_distortion * px + 2 * m_camera_params.p1 * px * py + m_camera_params.p2 * (radial_distortion * 2 * px * px) };
    double y_n_d{ radial_distortion * py + m_camera_params.p1 * (radial_distortion * 2 * py * py) + 2 * m_camera_params.p2 * px * py };
    normalizedPixel(x_n_d, y_n_d);
  }

  inline void CameraDistortion::cameraDistortion::normalizedPixel(double& px, double& py)
  {
    double x_p_d{ m_camera_params.fx * (px + m_camera_params.skew_c * py) + m_camera_params.cx };
    double y_p_d{ (m_camera_params.fy * py) + m_camera_params.cy };
    px = x_p_d;
    py = y_p_d;
  }

  void CameraDistortion::cameraDistortion::undistortedImage()
  {
    cv::VideoCapture video(CAMERA_NUMBER);
    if (!video.isOpened())
    {
      std::cerr << "Error while opening video\n";
    }
    cv::Mat current_image_frame;
    cv::Mat undistorted_image;
    while (video.grab()) {
      video.read(current_image_frame);
      cv::flip(current_image_frame, current_image_frame, 1);
      cv::Size2i image_size{ current_image_frame.cols, current_image_frame.rows };
      undistorted_image = cv::Mat::zeros(image_size, current_image_frame.type());
      for (int row_iter = 0; row_iter < image_size.height; row_iter++) {
        for (int col_iter = 0; col_iter < image_size.width; col_iter++) {
          double px{ (double)col_iter }, py{ (double)row_iter };
          pixelNormalized(px, py);
          if (px >= 0 && py >= 0 && px < image_size.width && py < image_size.height) {
            undistorted_image.at<cv::Vec3b>(row_iter, col_iter)[0] = current_image_frame.at<cv::Vec3b>(py, px)[0];
            undistorted_image.at<cv::Vec3b>(row_iter, col_iter)[1] = current_image_frame.at<cv::Vec3b>(py, px)[1];
            undistorted_image.at<cv::Vec3b>(row_iter, col_iter)[2] = current_image_frame.at<cv::Vec3b>(py, px)[2];
            //undistorted_image.at<uchar>(row_iter, col_iter) = current_image_frame.at<uchar>(py, px);
          }
        }
      }
      cv::imshow("image", current_image_frame);
      cv::imshow("undistorted_image", undistorted_image);
      int key{ cv::waitKey(15) };
      if (key == 113) {
        break;
      }
    }
  }
  void cameraDistortion::undistortedImage(const cv::Mat& distorted_image)
  {
    cv::Mat origin_image = distorted_image.clone();
    cv::resize(origin_image, origin_image, cv::Size(640, 480), 0.0, 0.0, CV_INTER_NN);
    cv::Size2i image_size{ origin_image.cols, origin_image.rows };
    cv::Mat undistorted_image = cv::Mat::zeros(image_size, origin_image.type());
    for (int row_iter = 0; row_iter < image_size.height; row_iter++) {
      for (int col_iter = 0; col_iter < image_size.width; col_iter++) {
        double px{ (double)col_iter }, py{ (double)row_iter };
        pixelNormalized(px, py);
        undistorted_image.at<cv::Vec3b>(row_iter, col_iter)[0] = origin_image.at<cv::Vec3b>(py, px)[0];
        undistorted_image.at<cv::Vec3b>(row_iter, col_iter)[1] = origin_image.at<cv::Vec3b>(py, px)[1];
        undistorted_image.at<cv::Vec3b>(row_iter, col_iter)[2] = origin_image.at<cv::Vec3b>(py, px)[2];
      }
    }
    cv::imshow("distorted image", origin_image);
    cv::imshow("undistorted image", undistorted_image);
    cv::waitKey(0);
  }
}
