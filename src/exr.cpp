#include <ImfInputFile.h>
#include <ImfOutputFile.h>
#include <ImfRgbaFile.h>
#include <ImfChannelList.h>
#include <ImfFrameBuffer.h>
#include <half.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include "exr.h"

using namespace Imf;
using namespace Imath;
using namespace std;

int ToneMapEXR(float i, float max)
{
	int r = (i/max)*255;
	if(r<0)   r=0;
	if(r>255) r=255;
	return r;
}

//TODO We could sample or average the area that is reduced to one pixel
//     instead of just getting one pixel (the top left)
fRGB *ScaleEXR(fRGB *HDRimg, int ow, int oh, int nw, int nh)
{
	if((ow < nw) || (oh < nh))
		return 0;

	float multw = ow/(float)nw;
	float multh = oh/(float)nh;

	fRGB * newimg = new fRGB[nw*nh];

	for(int a=0 ; a<(nw*nh); a++)
	{
		int i = a / nw;
		int j = a % nw;

		int io = i * multh;
		int jo = j * multw;

		newimg[a] = HDRimg[io*ow + jo];
	}

	return newimg;
}

float ReadEXR(string filename, float adjust, fRGB * &HDRimg, int &HDRw, int &HDRh)
{
	InputFile file( filename.c_str() );
	Box2i dw = file.header().dataWindow();
	int width  = dw.max.x - dw.min.x + 1;
	int height = dw.max.y - dw.min.y + 1;

	half *rgb = new half[3 * width * height];

	FrameBuffer frameBuffer;
	frameBuffer.insert("R", Slice(HALF, (char *)rgb,
		3*sizeof(half), width * 3 * sizeof(half), 1, 1, 0.0));
	frameBuffer.insert("G", Slice(HALF, (char *)rgb+sizeof(half),
		3*sizeof(half), width * 3 * sizeof(half), 1, 1, 0.0));
	frameBuffer.insert("B", Slice(HALF, (char *)rgb+2*sizeof(half),
		3*sizeof(half), width * 3 * sizeof(half), 1, 1, 0.0));

	file.setFrameBuffer(frameBuffer);
	file.readPixels(dw.min.y, dw.max.y);

	float max=0;

	HDRimg = new fRGB[width * height];
	for (int i = 0; i < width * height; ++i)
	{
		float frgb[3] = { rgb[3*i], rgb[3*i+1], rgb[3*i+2] };
		HDRimg[i].r = frgb[0]/adjust; if(HDRimg[i].r > max) max = HDRimg[i].r;
		HDRimg[i].g = frgb[1]/adjust; if(HDRimg[i].g > max) max = HDRimg[i].g;
		HDRimg[i].b = frgb[2]/adjust; if(HDRimg[i].b > max) max = HDRimg[i].b;
	}

	HDRw = width; HDRh = height;

	delete[] rgb;
	return max;
}

bool WriteEXR(std::string filename, fRGB *img, int w, int h)
{
	Header header(w, h);
	header.channels().insert("R", Channel(HALF));
	header.channels().insert("G", Channel(HALF));
	header.channels().insert("B", Channel(HALF));

	OutputFile file(filename.c_str(), header);

	FrameBuffer frameBuffer;

	//split img int 3 buffers
	half *r = new half[w*h];
	half *g = new half[w*h];
	half *b = new half[w*h];

	for(int i=0; i<w*h; i++)
	{
		r[i] = img[i].r;
		g[i] = img[i].g;
		b[i] = img[i].b;
	}

	frameBuffer.insert("R", Slice(HALF, (char *)r, sizeof(*r), sizeof(*r)*w));
	frameBuffer.insert("G", Slice(HALF, (char *)g, sizeof(*g), sizeof(*g)*w));
	frameBuffer.insert("B", Slice(HALF, (char *)b, sizeof(*b), sizeof(*b)*w));

	file.setFrameBuffer(frameBuffer);
	file.writePixels(h);

	delete [] r;
	delete [] g;
	delete [] b;

	return true;
}
