#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdint.h>

class Function {
public:
    virtual const char *get_name() = 0;

    virtual const int get_id() = 0;

    virtual const uint8_t *get_tag() = 0;

    virtual uint8_t *output() = 0;

    virtual int expt_output_size() = 0;

    // All main logic go here, including customized serialization (Do NOT use boost).
    // We minimize overhead in other function (benefit for our design against baseline D-:).
    virtual void process() = 0;
};

#endif