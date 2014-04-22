//------------------------------------------------------------------------------
// Estimate Pulse from camera using OpenCV.
// by Michael May (http://maym86.com/) maym86@gmail.com
//------------------------------------------------------------------------------


#ifndef _PHASE_CONTROLLER
#define _PHASE_CONTROLLER


#include <math.h>

#include <opencv2\opencv.hpp>

class PhaseController
{
public:
	
	float alpha;
	float beta;
	
	PhaseController();

	PhaseController(float default_alpha, float default_beta, bool start_state);
	bool Toggle();
	void On();
	void Off();
	void Execute(float phase);

private:
    bool state;
    float default_b;
	float default_a;

};

#endif