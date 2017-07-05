#include "function.h"

Function::Function(int id, int type, const std::string& name)
    : m_id   (id),
      m_type (type),
      m_name (name)
{
    // leave the calculation of "tag" to each specific function
}

int Function::get_id()
{
    return m_id;
}

const char *Function::get_name()
{
    return m_name.c_str();
}

int Function::get_type()
{
    return m_type;
}

int Function::expt_output_size()
{
    return m_expt_output_size;
}