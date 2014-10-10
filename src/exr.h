#ifndef EXR_H_
#define EXR_H_

#include <string>

struct fRGB
{
	fRGB() {r=0;g=0;b=0;}
	fRGB(float i,float j,float k) {r=i;g=j;b=k;}

	fRGB operator/(const float & v) const
	{
		return fRGB(r/v, g/v, b/v);
	}

	fRGB operator+=(const float &a)
	{
		r+=a; g+=a; b+=a; return *this;
	}

	fRGB operator+=(const fRGB &a)
	{
		r+=a.r; g+=a.g; b+=a.b; return *this;
	}

	float r,g,b;
};

int ToneMapEXR(float i, float max=1);
float ReadEXR(std::string filename, float adjust, fRGB *&HDRimg, int&, int&);
bool WriteEXR(std::string filename, fRGB *HDRimg, int w, int h);
fRGB * ScaleEXR(fRGB *HDRimg, int ow, int oh, int nw, int nh);

#endif /* EXR_H_ */
