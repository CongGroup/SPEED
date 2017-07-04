#include "word_count.h"

#include "crypto.h"
#include "sysutils.h"

#include "../../common/data_type.h"

#include <map>

// a 4-byte specical delimeter for map serialization
#define DLMT 0x0

// TODO replace with unordered_map
typedef std::map<std::string, int> CounterMap;

// this needs to be large enough
#define WORD_COUNT_RLT_SIZE 1024*1024 // 1M

WordCount::WordCount(int id, const char *textfile, int filesize)
    : m_id(id),
      m_name("Word Count"),
      m_type(FUNC_WC),
      m_content (textfile, filesize)
{
    hash((byte *)((m_name + m_content).data()), m_content.size(), m_tag);

    m_expt_output_size = std::max((int)m_content.size(), WORD_COUNT_RLT_SIZE);

    // secure a reservation for making get request
    m_output.reserve(m_expt_output_size);
}

const char * WordCount::get_name()
{
    return m_name.c_str();
}

const int WordCount::get_id()
{
    return m_id;
}

const int WordCount::get_type()
{
    return m_type;
}

const byte * WordCount::get_tag()
{
    return m_tag;
}

byte * WordCount::input()
{
    return (byte *)m_content.data();
}

int WordCount::input_size()
{
    return m_content.size();
}

byte * WordCount::output()
{
    return (byte *)m_output.data();
}

int WordCount::expt_output_size()
{
    return m_expt_output_size;
}

void WordCount::process()
{
    std::string word;
    CounterMap counter;

    // currently we do tokenization by whitespace or common punctuations
    static std::string delim = " ,;.?!\t\n";
    size_t delim_pos = m_content.find_first_of(delim);
    size_t word_pos = 0;
    while (delim_pos !=std::string::npos) {
        // in case of continous punctuations
        if ((delim_pos - word_pos) > 1) {
            word = m_content.substr(word_pos, delim_pos - word_pos);
            ++counter[word];
        }

        word_pos = delim_pos + 1;

        delim_pos = m_content.find_first_of(delim, word_pos);
    }
    // last word
    if (word_pos < m_content.size()) {
        word = m_content.substr(word_pos, m_content.size() - word_pos);
        ++counter[word];
    }

    // customized serialization
    m_output.clear();
    for (CounterMap::const_iterator it = counter.begin();
                                    it != counter.end(); ++it) {
        m_output += it->first;
        //m_output += std::to_string(it->second); // c++11
        //m_output += std::to_string(DLMT);
        m_output += "1234"; // fake
        m_output += "0000"; // fake
        //eprintf("%s %d \n", it->first.c_str(), it->second);
    }

    //if (m_output.size() > m_exp_size)
    m_expt_output_size = m_output.size();
}