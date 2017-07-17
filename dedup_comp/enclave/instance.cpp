#include "Enclave_t.h"

#include "compression.h"
#include "dedup_service.h"
#include "function.h"
#include "pattern_matching.h"
#include "sysutils.h"
#include "word_count.h"

// test cases go here
void ecall_entrance()
{
    hrtime start_time, end_time;

    int fid = 0;

    // word count
    char *textfile;
    int filesize;
    load_text_file("test_wc", &textfile, &filesize);

	get_time(&start_time);
	get_time(&end_time);
	eprintf("###Get time need %d us\n", time_elapsed_in_us(&start_time, &end_time));
	get_time(&start_time);
	eprintf("###Print and differ time need %d us\n\n", time_elapsed_in_us(&end_time, &start_time));
    
	Function *wc1 = new WordCount(fid++, textfile, filesize);
	dedup(wc1);
	delete wc1;

	Function *wc2 = new WordCount(fid++, textfile, filesize);
	dedup(wc2);
	delete wc2;
	eprintf("Use word count to init dedup cache.\n\n");


    // pattern matching
    const char regex[] = "(Twain)|([[:alpha:]]+ing)|(Huck[[:alpha:]]+)|(^[^ ]*?Twain)|(Tom|Sawyer|Huckleberry|Finn)|((Tom|Sawyer|Huckleberry|Finn).{0,30}river|river.{0,30}(Tom|Sawyer|Huckleberry|Finn))";
	char* fileName = "test_pm";
	Function *pm1 = 0,*pm2 = 0;

	fileName = "test_pm_1kb";
	load_text_file(fileName, &textfile, &filesize);
	eprintf("File %s size is %d\n", fileName, filesize);

    pm1 = new PatternMatching(fid++, regex, textfile, filesize);
    get_time(&start_time);
    dedup(pm1);
    get_time(&end_time);
    eprintf("PM cache miss total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));
    delete pm1;

    pm2 = new PatternMatching(fid++, regex, textfile, filesize);
    get_time(&start_time);
    dedup(pm2);
    get_time(&end_time);
    eprintf("PM cache hit total use[%d us]\n\n", time_elapsed_in_us(&start_time, &end_time));
    delete pm2;

	fileName = "test_pm_10kb";
	load_text_file(fileName, &textfile, &filesize);
	eprintf("File %s size is %d\n", fileName, filesize);

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

	fileName = "test_pm_100kb";
	load_text_file(fileName, &textfile, &filesize);
	eprintf("File %s size is %d\n", fileName, filesize);

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

	fileName = "test_pm_1024kb";
	load_text_file(fileName, &textfile, &filesize);
	eprintf("File %s size is %d\n", fileName, filesize);

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

	//compression
	//load_text_file("test_cp", &textfile, &filesize);
	//fileName = "test_cp_5kb";
	//load_text_file(fileName, &textfile, &filesize);
	//eprintf("File %s size is %d\n", fileName, filesize);
	//
	//Function *cp1 = new Compression(fid++, (byte *)textfile, filesize);
	//dedup(cp1);
	//eprintf("##CP cache miss total use [%d us]\n", time_elapsed_in_us(&start_time, &end_time));
	//delete cp1;
	//
	//Function *cp2 = new Compression(fid++, (byte *)textfile, filesize);
	//dedup(cp2);
	//eprintf("##CP cache hit total use[%d us]\n\n", time_elapsed_in_us(&start_time, &end_time));
	//delete cp2;

}