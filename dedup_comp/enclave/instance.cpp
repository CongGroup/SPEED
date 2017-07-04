#include "Enclave_t.h"

#include "dedup_service.h"
#include "function.h"
#include "sysutils.h"
#include "word_count.h"

// test cases go here
void ecall_entrance()
{
    char *textfile;
    int filesize;
    load_text_file("test_wc", &textfile, &filesize);

    Function *func = new WordCount(1, textfile, filesize);
    dedup(func);
    
    Function *func2 = new WordCount(2, textfile, filesize);
    dedup(func2);

    textfile[0] = '.';
    Function *func3 = new WordCount(3, textfile, filesize);
    dedup(func3);

    Function *func4 = new WordCount(4, textfile, filesize);
    dedup(func4);

    delete func;
    delete func2;
    delete func3;
    delete func4;
}