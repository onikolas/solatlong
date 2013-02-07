#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "exr.h"

using namespace std;

const float PI = 3.14159265358979323846;

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		cout << "  Create a latitude longitude HDR environemnt map from a fisheye HDR image \n";
		cout << "  Usage: envmaker [fisheye image] [diameter] [camera f] [center x] [center y] \n\n";
		cout << "  The image size of the lat-long image is dirived from the diameter\n";
		return -1;
	}

	int centerx, centery;
	int diameter = atoi(argv[2]);
	float camera_f = atof(argv[3]);

	//read the fisheye exr
	fRGB *fisheye, *latlong;
	int w,h;
	ReadEXR(argv[1], 1, fisheye, w, h);

	//TODO: change size depending on the diameter
	int latlong_w = w;
	int latlong_h = h;
	latlong = new fRGB[latlong_w*latlong_h]; //this is ofr testing only

	if(argc == 6)
	{
		centerx  = atoi(argv[4]);
		centery  = atoi(argv[5]);
	}
	else
	{
		centerx = w/2;
		centery = h/2;
	}

	//initialize to black
	for(int i=0; i<w*h; i++)
		latlong[i] = fRGB(0,0,0);

	//go over the pixels of the new image
	for(int i=0 ; i<latlong_h; i++)
	{
		for(int j=0; j<latlong_w; j++)
		{
			//find pixel on fisheye image corresponding to i,j (theta,phi)

			//To test i will asume a perfect semisphear, later i will use someting like the following:

			//Using mapping function R = 2*f*sin(theta/2) until i know better
			/*
			float R = 2 * f * sin(i*PI/(2*latlong_h));
			float maxR = 2 * f *sin(PI/4); //max theta = PI/2
			int pixelR = (R/maxR) * (diameter/2);
			*/

			float theta = (i*PI)/latlong_h;
			float phi = (j*2*PI)/latlong_w;

			int x = (diameter/2) * sin(theta) * cos(phi) + centerx;
			int y = (diameter/2) * sin(theta) * sin(phi) + centery;

			latlong[i*latlong_w+j] = fisheye[y*w+x];
		}
	}

	//add .latlong to the input filename
	string filename = argv[1];
	int dot = filename.find(".exr");
	filename = filename.substr(0, dot);
	filename += ".latlong.exr";

	WriteEXR(filename, latlong, w, h);

	return 0;
}
