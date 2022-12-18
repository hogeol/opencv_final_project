#include "cameraCalibration.hpp"

namespace CameraCalibration {
  void cameraCalibration::init(const std::string& board_image_save_path)
  {
    m_board_path = std::filesystem::path{ board_image_save_path };
    if (!std::filesystem::exists(m_board_path)) {
      std::filesystem::create_directory(m_board_path);
    }
    //std::cout << "\nboard path: " << std::filesystem::absolute(m_board_path);
  }

  void cameraCalibration::saveImage()
  {
    static int cnt = 0;
    std::string save_file_path{ m_board_path.string() };
    cv::VideoCapture video(CAMERA_NUMBER);
    if (!video.isOpened()) {
      std::cerr << "\nERROR! plz check your camera\n";
    }
    cv::Mat current_frame_image;
    while (video.grab()) {
      video.read(current_frame_image);
      cv::imshow("image", current_frame_image);
      int key{ cv::waitKey(15) };
      if (key == 113) {
        break;
      }
      else if (key == 99) {
        /*save file name*/
        std::string save_file_name = save_file_path + "calibration_board_" + std::to_string(cnt++) + ".jpg";
        /*if same name is exist, increase count*/
        if (std::filesystem::exists(save_file_name)) {
          cnt++;
        }
        std::cout << "\nsave_file_path: " << save_file_name;
        cv::imwrite(save_file_name, current_frame_image);
      }
    }
  }

  void cameraCalibration::calculateCameraParams(const cv::Size2i& board_size, const bool& show_image)
  {
    m_board_size = board_size;
    setParam(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    std::vector<std::vector<cv::Point3f>> object_points;									// 3D 실세계 좌표
    std::vector<std::vector<cv::Point2f>> image_points;										// 2D 이미지 좌표

    std::vector<cv::Point3f> temp_object_points;
    for (int board_height_iter = 0; board_height_iter < m_board_size.height; board_height_iter++) {
      for (int board_width_iter = 0; board_width_iter < m_board_size.width; board_width_iter++) {
        temp_object_points.push_back(cv::Point3f{ (float)board_width_iter, (float)board_height_iter, 0 });
      }
    }
    std::vector<cv::String> images;
    std::string board_path{ m_board_path.string() };
    board_path += "calibration_board_*.png";
    cv::glob(board_path, images);
    cv::Mat frame, gray_frame;
    std::vector<cv::Point2f> corner_points;
    bool success;
    for (int i = 0; i < images.size(); i++){
      frame = cv::imread(images[i]);
      cv::cvtColor(frame, gray_frame, CV_RGB2GRAY);

      // findChessboardCorners : 입력 image가 chessboard pattern인지 확인하고 체스판 모서리를 찾는다.
      // 모서리의 배치가 특정 순서로 배치된 경우 true 반환, 그렇지 않으면 false
      // 보다 정확한 좌표를 결정하기 위해 cornerSubPix 함수와 사용

      // CV_CALIB_CB_ADAPTIVE_THRESH = 1	CV_CALIB_CB_NORMALIZE_IMAGE = 2	 CV_CALIB_CB_FAST_CHECK = 8
      // CV_CALIB_CB_ADAPTIVE_THRESH : image 흑백 변환
      // CV_CALIB_CB_NORMALIZE_IMAGE : equalizeHist를 사용하여 이미지 감마를 정규화
      // CV_CALIB_CB_FAST_CHECK : 체스판 모서리를 찾는 이미지에 대한 빠른 검사
      success = cv::findChessboardCorners(gray_frame, cv::Size(m_board_size.width, m_board_size.height), corner_points, CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE + CV_CALIB_CB_FAST_CHECK);

      if (success)
      {
        // TermCriteria
        // {
        //		int type;			CV_TERMCRIT_ITER 혹은 CV_TERMCRIT_EPS 의 OR 연산	
        //		int max_iter;		최대 반복 횟수
        //		double epsilon;		정확도
        // }
        // CV_TERMCRIT_ITER : 알고리즘 수행 최대 반복 회수, max_iter 변수
        // CV_TERMCRIT_EPS  : 얼마나 정확한 정도까지 수행할 것인지 나타내는 정확도, epsilon 변수
        cv::TermCriteria criteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 50, 0.001);

        cv::cornerSubPix(gray_frame, corner_points, cv::Size(11, 11), cv::Size(-1, -1), criteria);

        cv::drawChessboardCorners(frame, cv::Size(m_board_size.width, m_board_size.height), corner_points, success);

        object_points.push_back(temp_object_points);
        image_points.push_back(corner_points);
      }
      if (show_image == true) {
        cv::imshow("calib_img", frame);
        cv::waitKey(0);
      }
    }
    if (show_image == true) {
      cv::destroyAllWindows();
    }

    cv::Mat camera_matrix, distortion_coefficients, R, T;

    cv::calibrateCamera(object_points, image_points, cv::Size(gray_frame.rows, gray_frame.cols), camera_matrix, distortion_coefficients, R, T);
    // camera_matrix = { fx,   0,   cx
    //					  0,   fy,  cy
    //					  0    0    0}

    //distortion_coefficients = (k1, k2, p1, p2[, k3, [, k4, k5, k6[, s1, s2, s3, s4[, taux, tauy]]]])
    if (show_image == true) {
      std::cout << "\n\ncameraMatrix\n\n" << camera_matrix << std::endl;
      std::cout << "\n\ndistCoeffs\n\n" << distortion_coefficients << std::endl;
      std::cout << "\n\nRotation vector\n\n" << R << std::endl;
      std::cout << "\n\nTranslation vector\n\n" << T << std::endl;
    }
    setParam(camera_matrix.at<double>(0, 0), camera_matrix.at<double>(1, 1), camera_matrix.at<double>(0, 2), camera_matrix.at<double>(1, 2), distortion_coefficients.at<double>(0, 0), distortion_coefficients.at<double>(0, 1), distortion_coefficients.at<double>(0, 4), distortion_coefficients.at<double>(0, 2), distortion_coefficients.at<double>(0, 3), camera_matrix.at<double>(0, 1));
  }

  void cameraCalibration::setParam(const double& fx, const double& fy, const double& cx, const double& cy, const double& k1, const double& k2, const double& k3, const double& p1, const double& p2, const double& skew_c)
  {
    m_camera_params.fx = fx;
    m_camera_params.fy = fy;
    m_camera_params.cx = cx;
    m_camera_params.cy = cy;
    m_camera_params.k1 = k1;
    m_camera_params.k2 = k2;
    m_camera_params.k3 = k3;
    m_camera_params.p1 = p1;
    m_camera_params.p2 = p2;
    m_camera_params.skew_c = skew_c;
  }

  cv::Mat cameraCalibration::getBoardImage() const
  {
    return m_board_image;
  }

  cameraCalibration::cameraCalibration(void)
  {
  }
}