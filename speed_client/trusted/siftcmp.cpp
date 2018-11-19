#include "sift.h"

#include<string>
#include<algorithm>
#include<string.h>
#include<getopt.h>
#include<assert.h>
#include<memory>

#include "stdlib.h"
#include "Enclave_t.h"
#include "sysutils.h"

#include "siftcmp.h"
#include "Deduplicable.h"

#include "FunctionDB.h"
#include "dedupTool.h"
using namespace std;
using namespace VL;

//string computeDefaultSift(VL::PgmBuffer& buffer)
static std::string sift_wrapper(const string& pic, int _width, int _height)
{
	const float*const  _im_pt = (const float*)pic.data();

	float _sigman = 0.5f;
	float _sigma0 = 2.0f;
	int _O = -1;
	int _S = 3;
	int _omin = -1;
	int _smin = -1;
	int _smax = 4;

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

static void readfile(const char* path, PgmBuffer& data)
{
	char *textfile;
	int filesize;
	load_text_file(path, &textfile, &filesize);

	byte* pic_Data = (byte *)textfile;

	memcpy(&data.width, pic_Data, sizeof(int));
	pic_Data += sizeof(int);

	memcpy(&data.height, pic_Data, sizeof(int));
	pic_Data += sizeof(int);

	data.data = (pixel_t*)pic_Data;
}

extern bool dedup_switch;

bool cmpSift(const string& sift1, const string& sift2)
{
	return sift1.size()== sift2.size();
}


void siftcmp_run(const char * path)
{
	const int each_file_path_buffer_len = 200;
	char(*buffer_out)[each_file_path_buffer_len];
	int file_count = 0;
	ocall_read_dir(&file_count, path, (char**)&buffer_out, each_file_path_buffer_len);

	char(*buffer)[each_file_path_buffer_len] = (char(*)[each_file_path_buffer_len])new char[file_count*each_file_path_buffer_len + 1];
	memcpy(buffer, buffer_out, file_count*each_file_path_buffer_len);

	eprintf("Find %d files in path %s\n", file_count, path);
	if (file_count > 0)
	{
		PgmBuffer* files = new PgmBuffer[file_count];
		string* ress = new string[file_count];
		for (int i = 0; i < file_count; i++)
		{
			readfile(buffer[i], files[i]);
			//eprintf("run %p\n", (char*)files[i].data - sizeof(int) * 2);
			int pic_size = sizeof(float) * files[i].width * files[i].height;
			string strPic = string((char*)files[i].data, pic_size);
			if (dedup_switch)
			{
				Deduplicable<string, const string&, int, int> dedup_func("libsiftpp", "0.8.1", "sift_wrapper", sift_wrapper);
				ress[i] = dedup_func(strPic, files[i].width, files[i].height);
			}
			else
			{
				ress[i] = sift_wrapper(strPic, files[i].width, files[i].height);
			}
			eprintf("%s, w:%d, h:%d, siftsize:%d\n", buffer[i], files[i].width, files[i].height, ress[i].size());
		
			static int fileno = 0;
			fileno++;
			
			bool doOutput = false;
			if (doOutput)
			{
				char outputpath[64] = { 0 };
				snprintf(outputpath, 64, "out%d.sft", fileno);
				write_text_file(outputpath, ress[i].c_str(), ress[i].size());
			}
		}

		//for (int i = 0; i < file_count; i++)
		//{
		//	char simFiles[512] = { 0 };
		//	char* writePos = simFiles;
		//	for (int j = 0; j < file_count; j++)
		//	{
		//		if (i == j)
		//			continue;
		//		if (cmpSift(ress[i], ress[j]))
		//		{
		//			snprintf(writePos, 64, "%s ", buffer[j]);
		//			writePos += strlen(buffer[j]);
		//		}
		//	}
		//	if (strlen(simFiles) == 0)
		//	{
		//		snprintf(writePos, 64, "%s ", " none");
		//	}
		//	eprintf("%s is similar with %s\n", buffer[i], simFiles);
		//}
	}
}
