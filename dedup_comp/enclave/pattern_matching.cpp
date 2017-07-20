#include "pattern_matching.h"

#include "crypto.h"

#include "sysutils.h"

PatternMatching::PatternMatching(int id, const char *regex, const char *str, int str_len)
    : Function(id, FUNC_PM, "Pattern Matching"),
      m_str(str, str_len)
{
    // #int -> #byte
    m_expt_output_size = PCRE_OVECTOR_SIZE*sizeof(int);
    
    // prepare pcre engine for later processing
    m_pcre = pcre_compile(regex, // pattern
                          0,     // default options
                          &m_error, &m_erroffset,  // errors
                          NULL); // default character table

    if (!m_pcre) {
        eprintf("PCRE compilation failed at offset %d: %s\n", m_error, m_erroffset);
        abort();
    }

	m_data.reserve(m_name.size() + RAND_SIZE + m_str.size());
	m_data.assign(m_name);
	m_data.append(m_str.data(),m_str.data()+m_str.size());
}

const byte *PatternMatching::get_tag()
{
    // slightly deviate from the paper specification for ease of implmentation
    hash((byte *)((m_data).data()),m_name.size()+ m_str.size(), m_tag);
    return m_tag;
}

byte *PatternMatching::input()
{
    return (byte *)m_data.c_str();
}

int PatternMatching::input_size()
{
    return m_data.length();
}

byte *PatternMatching::output()
{
    return (byte*)m_ovector;
}

void PatternMatching::process()
{
    int rc = pcre_exec(m_pcre, 0, // pcre engine
                       m_str.c_str(), m_str.length(), // query string
                       0, // starting offset
                       0, // options
                       m_ovector, PCRE_OVECTOR_SIZE); // output vector of matching positions
    if (rc < 0) {
        switch (rc){
            case PCRE_ERROR_NOMATCH:
                m_expt_output_size = 4;
                m_ovector[0] = PCRE_ERROR_NOMATCH;
                break;
            default:
                assert(false);
        }
    }
    else {
        // no room for all matches, so we return the first 1000
        if (rc == 0)
            rc = PCRE_OVECTOR_SIZE / 3;
        m_expt_output_size = rc * 2 * sizeof(int);
    }

    /*eprintf("[pcre %d %d %d %d %d %d %d]", rc, m_ovector[0],
        m_ovector[1], m_ovector[2], m_ovector[3], m_ovector[4], m_ovector[5]);*/
}

void PatternMatching::setR(const byte *r)
{
	m_data.append((const char*)r, (const char*)r+RAND_SIZE);
}
