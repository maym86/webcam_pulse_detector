//------------------------------------------------------------------------------
// Estimate Pulse from camera using OpenCV.
// by Michael May (http://maym86.com/) maym86@gmail.com
//------------------------------------------------------------------------------


#ifndef _BUFFER_FFT
#define _BUFFER_FFT


#define _USE_MATH_DEFINES

#include <math.h>
#include <opencv2\opencv.hpp>
#include "BandProcess.h"
#include <time.h> 

#include "windows.h"


class BufferFFT
{
public:

	BufferFFT(int n = 322, float spikeLimit = 5.0);

	void GetFFT();
    float FindOffset();
	void Reset();
    void Execute(float data_in);
	
	
	float ready;
	int size;
    int n_;
	float fps;
	Mat fft;
	std::vector<float> freqs;
	std::vector<time_t> times;
	std::vector<float> samples;
	
	std::vector<float> interpolated;
private:
	std::vector<float> Interpolate(std::vector<time_t> &newTimes, std::vector<time_t> &original, std::vector<float> &samples );
	std::vector<float> HammingWindow(int length);
	std::vector<float> HighPass(std::vector<float> &samples);
	std::vector<float> SubtractMean(std::vector<float> &samples);
	float spike_limit;
	
    
	
	std::vector<time_t> even_times;
	
};

#endif