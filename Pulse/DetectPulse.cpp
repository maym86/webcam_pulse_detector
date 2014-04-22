//------------------------------------------------------------------------------
// Estimate Pulse from camera using OpenCV.
// by Michael May (http://maym86.com/) maym86@gmail.com
//------------------------------------------------------------------------------


#include "stdafx.h"
#include "DetectPulse.h"


DetectPulse::DetectPulse()
{ 
	buffer_fft = BufferFFT(425, 13.0);
	phase_controller = PhaseController(1.0, 0.0, true);

	cardiac_ = Cardiac();
	forehead_mean = 0;
}


//Draw the face rectangle and the text
void DetectPulse::DrawForehead(Mat& frame, Mat &forehead_img, Rect &forehead)
{
	Mat roi = frame(forehead);
	vector<Mat> channels;

	split(roi,channels);
    Mat B,G,R;
	
	G = forehead_img;

	vector<Mat> combined;
    combined.push_back(Mat(forehead_img.rows, forehead_img.cols, forehead_img.type(), Scalar(0)));
    combined.push_back(G);
	combined.push_back(Mat(forehead_img.rows, forehead_img.cols, forehead_img.type(), Scalar(0)));
	//merge the channels back into the original image
	merge(combined, roi);

}

float DetectPulse::GetSkinPixelsMean(Mat &face, float upperBound, float lowerBound)
{

	float cumulative = 0;
	int count = 0;
	
	for(int x= 0; x<face.rows; x++)
	{	
		for(int y=face.cols  * 0.2 ; y<face.cols * 0.8; y++)
		{	
			float val = face.at<Vec3b>(x,y)[1]; 
			if(val > lowerBound && val < upperBound)
			{
				cumulative += val;
				count++;
				face.at<Vec3b>(x,y)[0] = 0;
				face.at<Vec3b>(x,y)[2] = 0;
			}			
		}
	}

	return cumulative/count;
}

void DetectPulse::Execute(Mat &image, Rect &face, bool faceFound)
{

	//Get forehead
	float x = face.x + face.width * 0.5;
    float y = face.y + face.height * 0.2;
    
    x -= face.width / 5.0;
    y -= face.height / 5.0;

	Rect forehead(x, y, face.width * 0.4, face.height * 0.2);

	Mat forehead_image = image(forehead);

	vector<Mat> channels(3);
	// split img:
	split(forehead_image, channels);
	// get the green channel
	Mat forehead_green = channels[1];
	
	
	if(faceFound)
	{	
		cv::Scalar tempVal = mean( forehead_green );
		forehead_mean = tempVal.val[0];
	}

	int pixelRange = 10;

	float skinMean = GetSkinPixelsMean(image(face), forehead_mean + pixelRange, forehead_mean - pixelRange);

	buffer_fft.Execute(skinMean);

	//Measure heart
	cardiac_.Execute(buffer_fft.freqs,buffer_fft.fft);

	//time until the fft buffer is full
	float gap = (buffer_fft.n_ - buffer_fft.size) / buffer_fft.fps;
	
	char text[50];
	char countdown[50];
	double size =1;


	if(gap > 0)
		sprintf(countdown, "%0.0f sec", gap);
	else
		sprintf(countdown, "Ready");
	
	sprintf(text, "%0.0f bpm", cardiac_.bpm);
	size = 2;
	

	if ( buffer_fft.ready)
	{
		phase_controller.On();
	}

	filtered = cardiac_.filtered;
	samples = buffer_fft.interpolated;
	mag = cardiac_.magnitude;

	putText(image,text, Point(face.x,face.y - 20), FONT_HERSHEY_PLAIN,size,Scalar(0,255,0));
	putText(image,text, Point(face.x - 1,face.y - 21), FONT_HERSHEY_PLAIN,size,Scalar(255,0,0));

	putText(image,countdown, Point(20,20), FONT_HERSHEY_PLAIN,1,Scalar(0,255,0));
	putText(image,countdown, Point(21,21), FONT_HERSHEY_PLAIN,1,Scalar(255,0,0));
	
	equalizeHist( forehead_green, forehead_green );

	DrawForehead(image, forehead_green, forehead);

	
}

