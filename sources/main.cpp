//STL
#include <iostream>
#include <string>
#include <filesystem>
#include <windows.h>
//OPENCV
#include <opencv2/opencv.hpp>
//LOCAL
#include "cameraCalibration.hpp"
#include "cameraDistortion.hpp"

#define SAVE_PATH "../boards/"

int main(int argc, char** argv)
{
  CameraCalibration::cameraCalibration camera_calibration_class;
  CameraDistortion::cameraDistortion camera_distortion_class;
  short select_mode = 0;
  HANDLE h_console;
  h_console = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(h_console, 15);
  std::cout << "\nSelect what you want to do\n\n";
  std::cout << "If you select rest of numbers except for the below numbers, Exit the program\n\n";
  SetConsoleTextAttribute(h_console, 79);
  std::cout << "1. Save Calibration board\n\n";
  SetConsoleTextAttribute(h_console, 47);
  std::cout << "2. Camera Calibration\n\n";
  SetConsoleTextAttribute(h_console, 31);
  std::cout << "3. Undistorted Camera\n\n";
  SetConsoleTextAttribute(h_console, 7);
  std::cout << ">> ";
  std::cin >> select_mode;
  int cnt{ 0 };
re_action:
  switch ((int)select_mode) {
    //save board image for camera calibration
  case 1: {
    std::cout << "\n\nEnter 'c' to save calibration board, or enter 'q' to exit";
    camera_calibration_class.init(SAVE_PATH);
    camera_calibration_class.saveImage();
    std::cout << "\n\nIf you want to continue with camera calibration, Enter ";
    SetConsoleTextAttribute(h_console, 9);
    std::cout << "'2'\n\n";
    SetConsoleTextAttribute(h_console, 7);
    std::cout << "Else, enter any key to exit\n\n";
    std::cout << ">>";
    std::cin.clear();
    std::cin >> select_mode;
    std::cout << "\n";
    if (select_mode == 2) {
      goto re_action;
    }
    break;
  }
  case 2: {
    cv::Size2i board_size{};
    std::cout << "\n\nEnter a board size\n\nheight >>";
    std::cin.clear();
    std::cin >> board_size.height;
    std::cout << "\nwidth >> ";
    std::cin.clear();
    std::cin >> board_size.width;
    camera_calibration_class.init(SAVE_PATH);
    camera_calibration_class.calculateCameraParams(board_size);
    cnt++;
    std::cout << "\n\nIf you want to continue with undistorted camera, Enter ";
    SetConsoleTextAttribute(h_console, 9);
    std::cout << "'3'\n\n";
    SetConsoleTextAttribute(h_console, 7);
    std::cout << "or re-save board image, Enter";
    SetConsoleTextAttribute(h_console, 9);
    std::cout << "'1'\n\n";
    SetConsoleTextAttribute(h_console, 7);
    std::cout << ">>";
    std::cin.clear();
    std::cin >> select_mode;
    std::cout << "\n";
    if (select_mode == 1 || select_mode == 3) {
      goto re_action;
    }
    break;
  }
  case 3: {
    if (cnt == 0) {
      camera_calibration_class.init(SAVE_PATH);
      camera_calibration_class.calculateCameraParams(cv::Size2i{ 7,6 }, false);
    }
    std::filesystem::path image_path{ SAVE_PATH };
    std::string image_name = image_path.string() + "calibration_board_5.jpg";
    cv::Mat image = cv::imread(image_name);
    camera_distortion_class.undistortedImage(image);
    break;
  }
  default: {
    break;
  }
  }
  return 0;
}