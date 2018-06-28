#include "sift.hpp"

#include<string>
#include<algorithm>

#include <string.h>

extern "C" {
#include<getopt.h>
#if defined (VL_MAC)
#include<libgen.h>
#else
#include<string.h>
#endif
#include<assert.h>
}
#include<memory>

#include "sift-driver.h"

using namespace std;

std::string __inner_sift_test(int a, int b, double c)
{
	char buf[64] = { 0 };
	snprintf(buf, BUFSIZ, "%lf %d %d", c,b,a);
	return string().append(buf);
}

//string computeDefaultSift(VL::PgmBuffer& buffer)
std::string computeSift(const float* _im_pt, int _width, int _height,
	float _sigman,
	float _sigma0,
	int _O, int _S,
	int _omin, int _smin, int _smax)
{
	int    first = -1;
	int    octaves = -1;
	int    levels = 3;
	float  threshold = 0.04f / levels / 2.0f;
	float  edgeThreshold = 10.0f;
	float  magnif = 3.0;
	int    nodescr = 0;
	int    noorient = 0;
	int    stableorder = 0;
	int    savegss = 0;
	int    verbose = 0;
	int    binary = 0;
	int    haveKeypoints = 0;
	int    unnormalized = 0;
	int    fp = 0;

	int         O = octaves;
	int const   S = levels;
	int const   omin = first;
	float const sigman = .5;
	float const sigma0 = 1.6 * powf(2.0f, 1.0f / S);


	if (O < 1) {
		O = std::max
		(int
		(std::floor
		(log2
		(std::min(_width, _height))) - omin - 3), 1);
	}


	VL::Sift sift(_im_pt, _width, _height,
		sigman, sigma0,
		O, S,
		omin, -1, S + 1);

	sift.detectKeypoints(threshold, edgeThreshold);

	//eprintf("pic_size %d X %d  \n", buffer.width, buffer.height);
	//eprintf("gaussian %d %d %d \n", O, omin, S);
	//eprintf("threshold %f %f   \n", threshold, edgeThreshold);
	//eprintf("detector completed with %d keypoints\n", sift.keypointsEnd() - sift.keypointsBegin());

	sift.setNormalizeDescriptor(!unnormalized);
	sift.setMagnification(magnif);


	string out,line;
	int count = 0;
	char buf[BUFSIZ] = { '\0' };

	for (VL::Sift::KeypointsConstIter iter = sift.keypointsBegin();
		iter != sift.keypointsEnd(); ++iter)
	{

		// detect orientations
		VL::float_t angles[4];
		int nangles;
		if (!noorient) {
			nangles = sift.computeKeypointOrientations(angles, *iter);
		}
		else {
			nangles = 1;
			angles[0] = VL::float_t(0);
		}

		// compute descriptors
		for (int a = 0; a < nangles; ++a)
		{
			snprintf(buf, BUFSIZ, "%.2f %.2f %.2f %.3f", iter->x, iter->y, iter->sigma, angles[a]);
			line.append(buf);

			/* compute descriptor */
			VL::float_t descr_pt[128];
			sift.computeKeypointDescriptor(descr_pt, *iter, angles[a]);

			for (int i = 0; i < 128; ++i)
			{
				snprintf(buf, BUFSIZ, " %d", int32_t(float_t(512) * descr_pt[i]));
				line.append(buf);
			}
			snprintf(buf, BUFSIZ, "\n");
			line.append(buf);

			out.append(line);
			++count;
			line = "";
		} // next angle
	} // next key point

	return out;
}
