//------------------------------------------------------------------------------
// Estimate Pulse from camera using OpenCV.
// by Michael May (http://maym86.com/) maym86@gmail.com
//------------------------------------------------------------------------------


#ifndef _BAND_PROCESS
#define _BAND_PROCESS


#define _USE_MATH_DEFINES

#include <math.h>
#include <opencv2\opencv.hpp>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <cmath>
#include "GraphUtils.h"
using namespace cv;



class BandProcess
{
public:

	BandProcess();

	BandProcess(std::vector<float> &freqsIn, Mat &fftIn, 
		float lowerLimit = 0.0, float upperLimit = 3.0, bool makeFiltered = true, 
                 char* operation = "pass");

	void Execute();

	float hz;
    float peak_hz;
    float phase;
	std::vector<float> freqs;
	std::vector<float> filtered;
	std::vector<float> magnitude;

	Mat fft;
	Mat fft_out;
protected:
	std::vector<float> HammingWindow(int length);

	float upper_limit;
	float lower_limit;
    bool make_filtered;
    char* operation_;

	std::vector<float> freqs_in;
	Mat fft_in;

};


#endif