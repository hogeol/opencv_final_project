#ifndef _CAMERA_PARAMS_H
#define _CAMERA_PARAMS_H

namespace CameraParams {
  struct cameraExternParams {
    double fx;
    double fy;
    double cx;
    double cy;
    double k1;
    double k2;
    double k3;
    double p1;
    double p2;
    double skew_c;
  };

  class cameraParams {
  public:
    virtual void setParam(const double &fx, const double &fy, const double &cx, const double &cy, const double &k1, const double &k2, const double &k3, const double &p1, const double &p2, const double &skew_c) = 0;
  };
}

#endif
