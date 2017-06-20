#include "word_count.h"

#include "crypto.h"

#include "../../common/data_type.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

// a 4-byte specical delimeter for map serialization
#define DLMT 0x0

typedef std::unordered_map<std::string, int> CounterMap;

WordCount::WordCount(int id, const char *file)
{
    m_id = id;
    m_name = "<Word Count>";

    std::ifstream infile(file);
    if (infile.good()) {
        std::stringstream sstream;
        sstream << infile.rdbuf();
        infile.close();

        m_content = sstream.str();
        
        hash(RAW((m_name + m_content).data()), m_content.size(), m_tag);

        m_exp_size = m_content.size();
        m_output.reserve(m_exp_size);
    }
    else {
        abort();
    }
}

const char * WordCount::get_name()
{
    return m_name.c_str();
}

const int WordCount::get_id()
{
    return m_id;

}
const uint8_t * WordCount::get_tag()
{
    return m_tag;
}

uint8_t * WordCount::output()
{
    return RAW(m_output.data());
}

int WordCount::exp_output_size()
{
    return m_exp_size;
}

void WordCount::process()
{
    std::stringstream sstream(m_content);
    std::string word;
    CounterMap counter;

    // currently we do tokenization by whitespace
    while (sstream >> word) {
        ++counter[word];
    }

    m_output.clear();
    for (CounterMap::const_iterator it = counter.begin();
        it != counter.end(); ++it) {
        m_output += it->first;
        m_output += std::to_string(it->second); // C++11
        m_output += std::to_string(DLMT);
    }

    if (m_output.size() > m_exp_size)
        m_exp_size = m_output.size();
}