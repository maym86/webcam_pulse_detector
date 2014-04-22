//------------------------------------------------------------------------------
// Estimate Pulse from camera using OpenCV.
// by Michael May (http://maym86.com/) maym86@gmail.com
//------------------------------------------------------------------------------


#include "stdafx.h"
#include "BandProcess.h"

BandProcess::BandProcess()
{}

BandProcess::BandProcess( std::vector<float> &freqsIn, Mat &fftIn, float lowerLimit, 
	float upperLimit, bool makeFiltered, char* operation)
{
	freqs_in = freqsIn;
	fft_in = fftIn;
	
	peak_hz=0;

	fft_out = cv::Mat(0, 0, CV_32F);
	fft = cv::Mat(0, 0, CV_32F);

	make_filtered = makeFiltered;
	upper_limit = upperLimit;
	lower_limit = lowerLimit;
	
	operation_ = operation;
}

//Create a vector containing a Hamming Window which filters the signal
std::vector<float> BandProcess::HammingWindow(int length)
{
	std::vector<float> hamming;
	double omega = 2.0 * M_PI / (length);

    for (int i = 0; i < length; i++)
	{
		hamming.push_back( (0.54 - 0.46 * cos(omega * (i)))); 
	}
	return hamming;
}

void BandProcess::Execute()
{
	fft.empty();
	fft_out.empty();
	filtered.empty();
	

	std::vector<int> idx;	
	 if (strcmp(operation_, "pass") == 0)
	 {		
		 for(int i = 0; i < freqs_in.size(); i++)
		 {
			//get all the fft values at the acceptable frequencies
			if(freqs_in[i] > lower_limit && freqs_in[i] < upper_limit)
			{
				freqs.push_back(freqs_in[i]);
				fft.push_back(fft_in.at<float>(0,i) * fft_in.at<float>(0,i));
				
				fft_out.push_back( fft_in.at<float>(0,i) );
				idx.push_back(i);
			}
			else
			{ 
				fft_out.push_back((float)0.0);
			}
		 }
	 }
	 else
	 {
		for(int i = 0; i < freqs_in.size(); i++)
		 {
			if(freqs_in[i] < lower_limit && freqs_in[i] > upper_limit)
			{	
				freqs.push_back(freqs_in[i]);
				fft.push_back(fft_in.at<float>(0,i));
				fft_out.push_back( fft_in.at<float>(0,i));
				idx.push_back(i);
			}
			else
			{ 
				fft_out.push_back((float)0.0);
			}
		}		 
	 }

	 //for display invert fft
	 if(make_filtered)
	 {
		if(fft_out.rows > 2)
		 {
			Mat f;
			dft(fft_out, f, DFT_INVERSE|DFT_REAL_OUTPUT);
			std::vector<float> hamming = HammingWindow(f.rows);

			for(int i = 0; i < f.rows; i++)
			{
				filtered.push_back(f.at<float>(i)); /// hamming[i]);
			}
			
		 }
	 }
	 

	 
	 //error if the vector is empty
	 if(fft.rows > 0)
	 {
		float maxVal = 0;
		float maxIdx = 0;
		//blur the fft

		GaussianBlur(fft,fft,Size(1,15),1,3);
		Mat planes_storage = Mat::zeros(fft.rows,1, CV_32FC1 );
		Mat planes[] = {Mat_<float>(planes_storage), Mat::zeros(fft.size(), CV_32F)};
		
		split(fft,planes);
		Mat magnitudes;

		cv::magnitude(planes[0], planes[1], magnitudes);
		
		
		for(int i = 0; i < magnitudes.size().height; i++)
		{	
									
			float mag = magnitudes.at<float>(i);
			if(mag > maxVal)
			{
				maxVal = mag;
				maxIdx = i;				
			}
			magnitude.push_back(mag);
			
		}


		peak_hz = freqs[maxIdx];
		
		Mat phaseArray;
		//temp using 2d phase calculation
		cv::phase(planes[0], planes[1], phaseArray);
		phase = phaseArray.at<float>(maxIdx,0);
	 }
	 
}
