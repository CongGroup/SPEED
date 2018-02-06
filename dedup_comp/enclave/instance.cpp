#include "Enclave_t.h"


#include "compression.h"
#include "dedup_service.h"
#include "function.h"
#include "pattern_matching.h"
#include "sysutils.h"
#include "word_count.h"
#include "sift_function.h"
#include "string.h"

void dedupCase(int type, char* fileName)
{
	static int fid = 0;
	static hrtime start_time, end_time;

	// loadfile
	char *textfile;
	int filesize;
	load_text_file(fileName, &textfile, &filesize);
	eprintf("File %s size is %d\n", fileName, filesize);

	// pattern matching
	const char regex[] ="nil";
//	const char regex[] = "(Twain)|\
//(^[^ ]*?Twain)|\
//([[:alpha:]]+ing)|\
//(Huck[[:alpha:]]+)|\
//([a-z]shing)|\
//(\b\w+nn\b)|\
//([a-q][^u-z]{13}x)|\
//((?i)Tom|Sawyer|Huckleberry|Finn)|\
//(.{0,2}(Tom|Sawyer|Huckleberry|Finn))|\
//(.{2,4}(Tom|Sawyer|Huckleberry|Finn))|\
//([a-zA-Z]+ing)|\
//(\s[a-zA-Z]{0,12}ing\s)|\
//(([A-Za-z]awyer|[A-Za-z]inn)\s)|\
//([d-hx-z])|\
//((?:a|b)aa(?:aa|bb)cc(?:a|b))|\
//((?:a|b)aa(?:aa|bb)cc(?:a|b)abcabcabd)|\
//((2[0-4]\d|25[0-5]|[01]?\d\d?)\.){3}(2[0-4]\d|25[0-5]|[01]?\d\d?)";
//

	switch (type)
	{
	case 1:
	{
		Function *wc1 = new WordCount(fid++, textfile, filesize);
		dedup(wc1);
		delete wc1;

		Function *wc2 = new WordCount(fid++, textfile, filesize);
		dedup(wc2);
		delete wc2;
		eprintf("Use word count to init dedup cache.\n\n");
		break;
	}
	case 2:
	{
		Function *pm1 = 0, *pm2 = 0;

		pm1 = new PatternMatching(fid++, regex, textfile, filesize);
		get_time(&start_time);
		dedup(pm1);
		get_time(&end_time);
		eprintf("##PM cache miss total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));
		delete pm1;

		pm2 = new PatternMatching(fid++, regex, textfile, filesize);
		get_time(&start_time);
		dedup(pm2);
		get_time(&end_time);
		eprintf("##PM cache hit total use[%d us]\n\n", time_elapsed_in_us(&start_time, &end_time));
		delete pm2;

		break;
	}
	case 3:
	{
		//compression
		Function *cp1 = 0, *cp2 = 0;

		cp1 = new Compression(fid++, (byte *)textfile, filesize);
		get_time(&start_time);
		dedup(cp1);
		get_time(&end_time);
		eprintf("##CP cache miss total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));
		delete cp1;

		cp2 = new Compression(fid++, (byte *)textfile, filesize);
		get_time(&start_time);
		dedup(cp2);
		get_time(&end_time);
		eprintf("##CP cache hit total use[%d us]\n\n", time_elapsed_in_us(&start_time, &end_time));
		delete cp2;
		break;
	}
case 4:
	{
	Function *sf1 = 0, *sf2 = 0;

	int width;
	int height;

	byte* pic_Data = (byte *)textfile;

	memcpy(&width, pic_Data, sizeof(int));
	pic_Data += sizeof(int);
	memcpy(&height, pic_Data, sizeof(int));
	pic_Data += sizeof(int);

	sf1 = new sift_function(fid++, pic_Data, width, height);
	get_time(&start_time);
	dedup(sf1);
	get_time(&end_time);
	eprintf("##SF cache miss total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));
	delete sf1;
	

	sf2 = new sift_function(fid++, pic_Data, width, height);
	get_time(&start_time);
	dedup(sf2);
	get_time(&end_time);
	eprintf("##SF cache hit total use[%d us]\n\n", time_elapsed_in_us(&start_time, &end_time));
	delete sf2;

	break;
	}//end case 
	}//end switch

}//end function



// test cases go here
void ecall_entrance()
{
    hrtime start_time, end_time;

	//dedupCase(1, "test_wc");

	get_time(&start_time);
	get_time(&end_time);
	eprintf("###Get time need %d us\n", time_elapsed_in_us(&start_time, &end_time));
	get_time(&start_time);
	eprintf("###Print and differ time need %d us\n\n", time_elapsed_in_us(&end_time, &start_time));
    
	char* fileName;

	//fileName = "linux-3.12.tag";
	//dedupCase(3, fileName);
	//fileName = "1kb";
	//dedupCase(3, fileName);

	//PM
	fileName = "0.5K";
	dedupCase(2, fileName);
	fileName = "1K";
	dedupCase(2, fileName);
	fileName = "1.5K";
	dedupCase(2, fileName);
	//fileName = "10K";
	//dedupCase(2, fileName);
	//fileName = "32K";
	//dedupCase(2, fileName);
	//fileName = "64K";
	//dedupCase(2, fileName);
	//fileName = "100kb";
	//dedupCase(2, fileName);
	//fileName = "1024kb";
	//dedupCase(3, fileName);


	//CP
	//fileName = "1kb";
	//dedupCase(3, fileName);
	fileName = "1M";
	dedupCase(3, fileName);
	fileName = "2M";
	dedupCase(3, fileName);
	fileName = "3M";
	dedupCase(3, fileName);

	//SF
	//fileName = "500K.pgm";
	//dedupCase(4, fileName);
	//fileName = "1000K.pgm";
	//dedupCase(4, fileName);
	//fileName = "2000K.pgm";
	//dedupCase(4, fileName);
	
	int _sigma0;
	int _sigmaO;
}