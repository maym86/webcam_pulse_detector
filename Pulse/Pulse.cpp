//------------------------------------------------------------------------------
// Estimate Pulse from camera using OpenCV.
// by Michael May (http://maym86.com/) maym86@gmail.com
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "Pulse.h"

using namespace std;
using namespace cv;

std::vector<float> filtered;
std::vector<float> samples;
std::vector<float> mag;


DWORD WINAPI RunGraphs( LPVOID lpParam ) 
{
	while(true)
	{
		if(filtered.size() > 0)
			showFloatGraph("Pulse", &filtered[0], filtered.size(),1,0,0,0,425);
		if(samples.size() > 0)
			showFloatGraph("Samples", &samples[0], samples.size(),1,0,0,0,425);
		if(mag.size() > 0)
			showFloatGraph("FFT Magnitude", &mag[0], mag.size(),1,0,0,0,425);
	}
	return 0;

}


 Rect DetectCascade(CascadeClassifier &cascade, Mat &image, int minNeighbours)
{
	vector<Rect> faces;
	Rect faceOut = Rect(0,0,0,0);

	cascade.detectMultiScale( image, faces, 1.5, 3, CV_HAAR_SCALE_IMAGE, Size(64, 64) );

	for(int i = 0; i<faces.size(); i++)
	{
		if(faces.at(i).width > faceOut.width && faces.at(i).height > faceOut.height)
		{
			faceOut = faces.at(i);
		}
	}

	return faceOut;
}

//Draw the face rectangle and the text
void DrawFace(Mat &frame, Rect &faceRect)
{
	vector<Mat> channels;
	Mat roi = frame(faceRect);

	split(roi,channels);
    Mat B,G,R;

    equalizeHist( channels[0], B );
    equalizeHist( channels[1], G );
    equalizeHist( channels[2], R );
    vector<Mat> combined;
    combined.push_back(B);
    combined.push_back(G);
    combined.push_back(R);
    
    merge(combined,roi);
}

void EqualizeGreen(Mat &frame)
{
	vector<Mat> channels;
	

	split(frame,channels);
    Mat B,G,R;

 
    equalizeHist( channels[1], G );
    vector<Mat> combined;
    combined.push_back(channels[0]);
    combined.push_back(G);
    combined.push_back(channels[2]);
    
    merge(combined,frame);

}

DWORD WINAPI RunPulse( LPVOID lpParam ) 
{
	VideoCapture pulseCam;

	pulseCam.open(0);

	String face_cascade_name = "haarcascade_frontalface_alt.xml";

	Mat framePulseColour;
	CascadeClassifier cascade;
	
	if( !cascade.load(face_cascade_name) )
	{ 
		printf("Error loading cascade file.\n"); return -1; 
	}

	DetectPulse pulse_detector;
	Rect face;
	int threshFace = 5;

	std::vector<Rect> faceAvg;

	//Stop face pos from moving by pressing s
	bool faceLock = false;
	
	//if a face is found in the scene
	bool faceFound = false;

	while(true)
	{
		Mat framePulseColourTemp;
		pulseCam >> framePulseColourTemp;
		
		Rect crop(framePulseColour.rows * 0.25, framePulseColour.cols * 0.25 ,320,320);

		framePulseColour = framePulseColourTemp(crop);

		//EqualizeGreen(framePulseColour);

		//if detecting the face
		if(!faceLock)
		{
			face = DetectCascade(cascade, framePulseColour, 3);
			
			if(face.width > 30)
			{

				faceFound = true;
				faceAvg.push_back(face);
			
				if(faceAvg.size() > threshFace)
				{
					faceAvg = std::vector<Rect>(faceAvg.begin() +1, faceAvg.end());
				}
			}
			else
			{
				faceFound = false;
			}

			
			if(faceAvg.size() > 0)
			{
				Rect faceSum;
			


				for(int i = 0; i < faceAvg.size(); i++)
				{
					faceSum.x += faceAvg[i].x;
					faceSum.y += faceAvg[i].y;
					faceSum.width += faceAvg[i].width;
					faceSum.height += faceAvg[i].height;
				}

			
				face.x = faceSum.x /  faceAvg.size();
				face.y = faceSum.y /  faceAvg.size();
				face.width = faceSum.width /  faceAvg.size();
				face.height = faceSum.height / faceAvg.size();
			}
		}
		
		if(face.width > 30)
		{
			pulse_detector.Execute(framePulseColour, face, faceFound);

			filtered = pulse_detector.filtered;
			samples = pulse_detector.samples;
			mag = pulse_detector.mag;
			//DrawFace(framePulseColour,face);
		}

		imshow("face", framePulseColour);
		char k = waitKey(1);

		if(k == 27)
			break;

		if(k == 's')
			faceLock = !faceLock;
	}
	return 0;
	
}


int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE Thread_Handles[1];
	
	filtered = std::vector<float>(425);
	std::fill( filtered.begin(), filtered.end(), 0.0 );
	
	samples = std::vector<float>(425);
	std::fill( samples.begin(), samples.end(), 0.0 );
	
	mag = std::vector<float>(425);
	std::fill( mag.begin(), mag.end(), 0.0 );
	
	Thread_Handles[0] = CreateThread( NULL, 0, RunPulse, NULL, 0, NULL);
	Thread_Handles[1] = CreateThread( NULL, 0, RunGraphs, NULL, 0, NULL);
	
	//run threads	
	WaitForMultipleObjects(2, Thread_Handles, FALSE, INFINITE);

	// Close all thread handles upon completion. 
	CloseHandle(Thread_Handles[0]);	
	CloseHandle(Thread_Handles[1]);	
}