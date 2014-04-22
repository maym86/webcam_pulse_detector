//------------------------------------------------------------------------------
// Estimate Pulse from camera using OpenCV.
// by Michael May (http://maym86.com/) maym86@gmail.com
//------------------------------------------------------------------------------


#include "stdafx.h"
#include "Cardiac.h"


Cardiac::Cardiac()
{	
	bufferThresh = 50;
	filtered = std::vector<float>(0);
}

void Cardiac::Execute(std::vector<float> &freqsIn, Mat &fftIn, 
		float lowerLimit, float upperLimit, bool makeFiltered, char* operation)
{

	bandProcess = BandProcess(freqsIn, fftIn, lowerLimit/60.0, upperLimit/60.0, makeFiltered, operation);

	
	bandProcess.Execute();

	freqs = bandProcess.freqs;

	for(int i = 0; i < freqs.size(); i++)
		freqs[i] =  freqs[i] * 60;

	
	pulseBuffer.push_back(bandProcess.peak_hz * 60);

	if(pulseBuffer.size() > bufferThresh)
	{
		pulseBuffer = std::vector<float>(pulseBuffer.begin() +1, pulseBuffer.end());
	}

	float bpmSum = 0;
	for(int i = 0; i < pulseBuffer.size(); i++)
	{
		bpmSum += pulseBuffer[i];
	}

	bpm = (bpmSum / pulseBuffer.size());
	
	magnitude = bandProcess.magnitude;
	filtered =  bandProcess.filtered;

	phase = bandProcess.phase;
}
