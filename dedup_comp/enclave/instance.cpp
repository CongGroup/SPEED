#include "Enclave_t.h"

#include "dedup_service_t.h"
#include "function.h"
#include "sysutils.h"
#include "word_count.h"

// test cases go here
void ecall_entrance()
{
    int f_id = 1;
    
    char *textfile;
    int filesize;
    load_text_file("test_wc", &textfile, &filesize);
    
    Function *func = new WordCount(f_id, textfile, filesize);

    dedup(func);
}
