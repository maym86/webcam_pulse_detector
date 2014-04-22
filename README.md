Pulse
=====

C++ computer vision pulse detection using OpenCV based on https://github.com/thearn/webcam-pulse-detector. 

This application uses OpenCV to find the location of the user's face, then isolate the forehead region. Data is collected from this location over time to estimate the user's heart rate. This is done by measuring average optical intensity in the forehead location, in the subimage's green channel alone (a better color mixing ratio may exist, but the blue channel tends to be very noisy). Physiological data can be estimated this way via a combination of photoplethysmology and the optical absorption characteristics of (oxy-) haemoglobin (see http://www.opticsinfobase.org/oe/abstract.cfm?uri=oe-16-26-21434).

With good lighting and minimal noise due to motion, a stable heartbeat should be isolated in about 15 seconds. Other physiological waveforms (such as Mayer waves) should also be visible in the raw data stream.

When run, a window will open showing a stream from your computer's webcam
When a forehead location has been correctly detected and isolated, the user should press "S" on their keyboard to lock this location, and remain as still as possible (the camera stream window must have focus for the click to register). This freezes the acquisition location in place. This lock can be released by pressing "S" again.
