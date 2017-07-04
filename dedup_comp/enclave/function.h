#ifndef FUNCTION_H
#define FUNCTION_H

#include "../../common/data_type.h"

class Function {
public:
    // unique id
    virtual const int get_id() = 0;

    virtual const char *get_name() = 0;

    // type id, which is used for computing tag
    virtual const int get_type() = 0;

    virtual const byte *get_tag() = 0;

    virtual byte *input() = 0;

    virtual int input_size() = 0;

    virtual byte *output() = 0;

    virtual int expt_output_size() = 0;

    // All main logic go here, including customized serialization (Do NOT use boost).
    // We minimize overhead in other function (benefit for our design against baseline D-:).
    virtual void process() = 0;
};

#endif