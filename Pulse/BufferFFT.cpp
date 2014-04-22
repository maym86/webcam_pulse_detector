//------------------------------------------------------------------------------
// Estimate Pulse from camera using OpenCV.
// by Michael May (http://maym86.com/) maym86@gmail.com
//------------------------------------------------------------------------------



#include "stdafx.h"
#include "BufferFFT.h"

BufferFFT::BufferFFT(int n, float spikeLimit)
{
	n_ = n;
	fps = 1.0;
	spike_limit = spikeLimit;
	
	ready = false;
}

//interpolate between vector points 
std::vector<float> BufferFFT::Interpolate(std::vector<time_t> &newTimes, std::vector<time_t> &original, std::vector<float> &samples )
{
	std::vector<float> results;

	for(int i = 0; i < newTimes.size(); i++)
	{
		time_t current = newTimes[i];
		
		for(int j = 0; j < original.size()-1; j++)
		{		
			if( current >= original[j] && current <= original[j+1])
			{
				
				//if no interpolation required
				if(original[j+1] == current)
				{	results.push_back(samples[j+1]);
					break;
				}
				if(original[j] == current)
				{	results.push_back(samples[j]);
					break;
				}

				current = current - original[j];
				//y = mx + c 
				//get linear interpolated value
				float m = ((float)samples[j+1] - (float)samples[j]) / ((float)original[j+1] - (float)original[j]);
				
				float newVal = (m * current) + samples[j]; 
				results.push_back(newVal); 
				break;
			}
		}
	}
	return results;
}

std::vector<float> BufferFFT::SubtractMean(std::vector<float> &samples)
{
	cv::Scalar tempVal = mean( samples );
	float meanVal = tempVal.val[0];

	for(int j = 0; j < samples.size(); j++)
	{
		samples[j] = samples[j] - meanVal;
	}

	return samples;
}
//Create a vector containing a Hamming Window which filters the signal
std::vector<float> BufferFFT::HammingWindow(int length)
{
	std::vector<float> hamming;
	double omega = 2.0 * M_PI / (length);

    for (int i = 0; i < length; i++)
	{
		hamming.push_back(0.54 - 0.46 * cos(omega * (double)(i))); 
	}
	return hamming;
}

//laplace
std::vector<float> BufferFFT::HighPass(std::vector<float> &samples)
{	
	cv::Scalar tempVal = mean( samples );
	float meanVal = tempVal.val[0];

	std::vector<float> temp;

	temp.push_back(0.0);

	if (samples.size() > 4)
	{
		for(int j = 1; j < samples.size() -1; j++)
		{
			float val = (samples[j+1] + (samples[j] * -2)  +  samples[j+1]);
		
			//attenuate large spikes
			/*if (val > 0.5)
				val = 0.5;
			if (val < -0.5)
				val = -0.5;
			*/
			temp.push_back(val);
		}
	}

	temp.push_back(0.0);
	
	return temp;
}


void BufferFFT::GetFFT()
{	
	int N = (int)times.size();
	fps = N / ((difftime(times[times.size() -1], times[0]) / 1000));
		
	
	even_times.clear();

	
	//get the evenly spaced times
	time_t spacing = (difftime(times[times.size() -1],times[0]) / N);

	///std::cout<< spacing << " ";

	for(time_t i=0; i < N * spacing; i+=spacing)
	{	
		even_times.push_back(times[0] + i);
	}
	
	interpolated = Interpolate(even_times, times, samples);	

//	interpolated= SubtractMean(interpolated);

	
	//get kernal size depending on input frequency
	int kernal = (int)((fps * 60) / 160);

	if(kernal % 2 == 0)
		kernal++;	

	//low pass to remove high freq
	GaussianBlur(interpolated,interpolated,Size(kernal,1),3,1);
	interpolated = HighPass(interpolated);
	
	std::vector<float> hamming = HammingWindow(interpolated.size());
	
	for(int i = 0; i < interpolated.size(); i++)
	{
		interpolated[i] = interpolated[i] * hamming[i];
	}
	
	//zero padding
	for(int i = N; i< n_; i++)
	{
		interpolated.push_back(0);
	}
	


	//get fft
	dft(interpolated, fft);	

	freqs.clear();
	
	for(int i = 0; i < interpolated.size(); i++)
	{
		freqs.push_back((i * fps / interpolated.size()) / 2); 
	}
}
	
	
//find min and max and see if there is a spike in the signal	
float BufferFFT::FindOffset()
{
	float N = samples.size();

	for(int i=2;i<N;i++)
	{
		std::vector<float> samplesTemp;

		for(int j = i; j < samples.size(); j++)
		{
			samplesTemp.push_back(samples[j]);
		}
		
		//samples = samplesTemp;

		float max = samplesTemp[0];  
		float min = samplesTemp[0];  
		
		//get the min and max values
		for(int i = 0; i< samplesTemp.size(); i++)
		{
			float val = samplesTemp[i];

			if(val > max)
				max = val;

			if(val < min)
				val = min;
		}
		
		if ((max-min) > spike_limit)
		{
			return N-1;
		}
	
	}
	return 0;
}
	
	
	
void BufferFFT::Reset()
{
	int N = FindOffset();
	ready = false;
	

	std::vector<float> tempSamples;
	std::vector<time_t> tempTimes;
	//keep all data after spike
	for(int i = N; i < samples.size(); i++)
	{
		tempTimes.push_back(times[i]);
		tempSamples.push_back(samples[i]);
	}

	times = tempTimes;
	samples = tempSamples;

}

//Execute the command
void BufferFFT::Execute(float data_in)
{
	
	SYSTEMTIME time;
	GetSystemTime(&time);
	time_t millis = (time.wHour * 3600000) + (time.wMinute * 60000) + (time.wSecond * 1000) + time.wMilliseconds;
	


	samples.push_back(data_in);
	times.push_back(millis);
	size = samples.size();
		
    if(size > n_)
	{
        ready = true;
		std::vector<float> samplesTemp;  
		std::vector<time_t> timesTemp;  
		
		//get the rest of the vector
		for(int i = size - n_; i< samples.size(); i++)
		{
			samplesTemp.push_back(samples[i]);
			timesTemp.push_back(times[i]);
		}
		
		
		samples = samplesTemp;
        times = timesTemp;
	}
	if(size>4)
	{
        GetFFT();

        if (spike_limit != 0)
		{
			auto max = max_element(std::begin(samples), std::end(samples)); 
			auto min = min_element(std::begin(samples), std::end(samples)); 
				
            if ((*max-*min) > spike_limit)
			{
				Reset();
			}
		}
	}
}