#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "exr.h"

using namespace std;

const float PI = 3.14159265358979323846;

int main(int argc, char** argv)
{
	if(argc < 4)
	{
		cout << "\n  Create a latitude longitude HDR environemnt map from a fisheye HDR image \n";
		cout << "  Usage: envmaker [fisheye image] [diameter] [focal length] [center x] [center y] [width] [latlong image]\n\n";
		cout << "  [fisheye image]    Image to process\n";
		cout << "  [diameter]         Diameter of the image circle in pixels\n";
		cout << "  [focal length]     The focal length. Enter -1 if not known\n";
		cout << "  [center x]         Center of the image circle\n";
		cout << "  [center y]         \n";
		cout << "  [width]            Width of the latlong to be created. Height is set to width/2)\n";
		cout << "  [latlong image]    Filename of the latlong image\n\n";
		return -1;
	}

	int centerx, centery;
	int latlong_w, latlong_h;
	int diameter = atoi(argv[2]);
	float camera_f = atof(argv[3]);

	if(camera_f == -1)
	{
		cout << "Finding the focal length from the diameter not yet implemented\n";
		return -1;
	}

	//read the fisheye exr
	fRGB *fisheye, *latlong;
	int w,h;
	ReadEXR(argv[1], 1, fisheye, w, h);

	if(argc >= 6)
	{
		centerx  = atoi(argv[4]);
		centery  = atoi(argv[5]);
	}
	else
	{
		centerx = w/2;
		centery = h/2;
	}
	if(argc >=7)
	{
		latlong_w = atoi(argv[6]);
		latlong_h = latlong_w/2;
	}
	else
	{
		latlong_w = w;
		latlong_h = latlong_w/2;
	}

	latlong = new fRGB[latlong_w*latlong_h];

	//initialize to black
	for(int i=0; i<latlong_w*latlong_h; i++)
		latlong[i] = fRGB(0,0,0);

	//go over the pixels of the new image
	for(int i=0 ; i<=latlong_h/2; i++)
	{
		for(int j=0; j<latlong_w; j++)
		{
			float theta = (i*PI)/latlong_h;
			float phi = (j*2*PI)/latlong_w;

			//find pixel on fisheye image corresponding to i,j (theta,phi)

			//Using mapping function R = 2*f*sin(theta/2) (equisolid) for Sigma 4.5mm F2.8 EX DC HSM Circular Fisheye
			//Other lenses can be added later if needed

			float R = 2 * camera_f * sin(theta/2); //this R is in mm because f is in mm
			float maxR = 2 * camera_f * sin(PI/4); //max theta = 90
			int pixelR = (R/maxR) * diameter/2;    //R in pixels

			int x = pixelR * cos(phi) + centerx;
			int y = pixelR * sin(phi) + centery;

			//asuming that the image is a perfect semisphere (equisolid is ?)
			//int x = (diameter/2) * sin(theta) * cos(phi) + centerx;
			//int y = (diameter/2) * sin(theta) * sin(phi) + centery;

			latlong[i*latlong_w+j] = fisheye[y*w+x];
		}
	}
	string filename;
	if(argc == 8)
		filename = argv[7];
	else
	{
		//add .latlong to the input filename
		filename= argv[1];
		int dot = filename.find(".exr");
		filename = filename.substr(0, dot);
		filename += ".latlong.exr";
	}
	WriteEXR(filename, latlong, latlong_w, latlong_h);

	return 0;
}
