//------------------------------------------------------------------------------
// Estimate Pulse from camera using OpenCV.
// by Michael May (http://maym86.com/) maym86@gmail.com
//------------------------------------------------------------------------------

#ifndef _DETECT_PULSE
#define _DETECT_PULSE

#define _USE_MATH_DEFINES

#include <math.h>
#include <opencv2\opencv.hpp>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <cmath>

#include "BandProcess.h"
#include "BufferFFT.h"
#include  "Cardiac.h"
#include  "PhaseController.h"
#include "GraphUtils.h"

using namespace cv;


class DetectPulse
{
public:

	std::vector<float> filtered;
	std::vector<float> samples;
	std::vector<float> mag;

	DetectPulse();

	void Execute(Mat &Image, Rect &face, bool faceFound);

protected:

	void DrawForehead(Mat& frame, Mat &forehead_img, Rect &forehead);
	float GetSkinPixelsMean(Mat &face, float upperBound, float lowerBound);
	
	Cardiac cardiac_;
	PhaseController phase_controller;
	BufferFFT buffer_fft;

	std::vector<float>freqs;
	std::vector<float>fft;
	float forehead_mean;
};


#endif