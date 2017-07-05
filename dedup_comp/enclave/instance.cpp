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

    Function *wc1 = new WordCount(fid++, textfile, filesize);
    dedup(wc1);
    delete wc1;

    Function *wc2 = new WordCount(fid++, textfile, filesize);
    dedup(wc2);
    delete wc2;

    textfile[0] = '.';
    Function *wc3 = new WordCount(fid++, textfile, filesize);
    dedup(wc3);
    delete wc3;

    Function *wc4 = new WordCount(fid++, textfile, filesize);
    dedup(wc4);
    delete wc4;

    // pattern matching
    const char regex[] = "(Twain)|([[:alpha:]]+ing)|(Huck[[:alpha:]]+)|(^[^ ]*?Twain)|(Tom|Sawyer|Huckleberry|Finn)|((Tom|Sawyer|Huckleberry|Finn).{0,30}river|river.{0,30}(Tom|Sawyer|Huckleberry|Finn))";
    load_text_file("test_pm", &textfile, &filesize);
    Function *pm1 = new PatternMatching(fid++, regex, textfile, filesize);
    get_time(&start_time);
    dedup(pm1);
    get_time(&end_time);
    eprintf("[%d us]\n", time_elapsed_in_us(&start_time, &end_time));
    delete pm1;

    Function *pm2 = new PatternMatching(fid++, regex, textfile, filesize);
    get_time(&start_time);
    dedup(pm2);
    get_time(&end_time);
    eprintf("[%d us]\n", time_elapsed_in_us(&start_time, &end_time));
    delete pm2;

    textfile[0] = '.';
    Function *pm3 = new PatternMatching(fid++, regex, textfile, filesize);
    dedup(pm3);
    delete pm3;

    Function *pm4 = new PatternMatching(fid++, regex, textfile, filesize);
    dedup(pm4);
    delete pm4;

    // compression
    load_text_file("test_cp", &textfile, &filesize);
    Function *cp1 = new Compression(fid++, (byte *)textfile, filesize);
    dedup(cp1);
    delete cp1;

    Function *cp2 = new Compression(fid++, (byte *)textfile, filesize);
    dedup(cp2);
    delete cp2;

    textfile[0] = '.';
    Function *cp3 = new Compression(fid++, (byte *)textfile, filesize);
    dedup(cp3);
    delete cp3;

    Function *cp4 = new Compression(fid++, (byte *)textfile, filesize);
    dedup(cp4);
    delete cp4;
}