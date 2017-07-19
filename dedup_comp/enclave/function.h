#ifndef FUNCTION_H
#define FUNCTION_H

#include "../../common/data_type.h"

#include <string>

class Function {
public:
    Function(int id, int type, const std::string& name);

    // unique id
    int get_id();

    // type id, which is used for computing tag
    int get_type();

    const char *get_name();

    virtual const byte *get_tag() = 0;

    virtual byte *input() = 0;

    virtual int input_size() = 0;

    virtual byte *output() = 0;

    int expt_output_size();

    // All main logic go here, including customized serialization (Do NOT use boost).
    // We minimize overhead in other function (benefit for our design against baseline D-:).
    virtual void process() = 0;

protected:
    int             m_id;
    int             m_type;
    std::string     m_name;

    byte            m_tag[TAG_SIZE];

    // for OCALL memory allocation
    int             m_expt_output_size;
};

#endif