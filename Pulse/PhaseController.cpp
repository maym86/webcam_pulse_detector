//------------------------------------------------------------------------------
// Estimate Pulse from camera using OpenCV.
// by Michael May (http://maym86.com/) maym86@gmail.com
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "PhaseController.h"

PhaseController::PhaseController()
{
}

PhaseController::PhaseController(float default_alpha, float default_beta, bool start_state)
{
	state = start_state;
	default_a = default_alpha;
	default_b = default_beta;

}

bool PhaseController::Toggle()
{
	 if(state)
        state = false;
     else
         state = true;
	 
	 return state;
}


void PhaseController::On()
{
	if(!state)
        Toggle();
}


void PhaseController::Off()
{
	 if(state)
        Toggle();

}


void PhaseController::Execute(float phase)
{
	if (state)
	{
		float t = (sin(phase) + 1.0f) / 2.0f;
		t = 0.9*t + 0.1;
		alpha = t;
		beta = 1-t;
	
	}
	else
	{
		beta = default_b;
		alpha = default_a;
	
	
	}

}