#ifndef SIFT_DRIVER_H
#define SIFT_DRIVER_H

#include "sift.hpp"
#include <string.h>


std::string computeSift(const float* _im_pt, int _width, int _height,
	float _sigman,
	float _sigma0,
	int _O, int _S,
	int _omin, int _smin, int _smax);

#endif