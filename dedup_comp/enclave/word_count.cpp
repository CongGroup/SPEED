#include "word_count.h"

#include "crypto.h"
#include "sysutils.h"

#include <map>

// a 4-byte specical delimeter for map serialization
#define DLMT 0x0

// TODO replace with unordered_map
typedef std::map<std::string, int> CounterMap;

// the result is most very compact, but we keep this large enough
#define WC_RLT_SIZE 1024*1024 // 1M

WordCount::WordCount(int id, const char *textfile, int filesize)
    : Function(id, FUNC_WC, "Word Count"),
      m_input (textfile, filesize)
{
    m_expt_output_size = std::max((int)m_input.size(), WC_RLT_SIZE);

    // secure a reservation for making get request
    m_output.reserve(m_expt_output_size);
}

const byte *WordCount::get_tag()
{
    // slightly deviate from the paper specification for ease of implmentation
    hash((byte *)((m_input).data()), m_input.size(), m_tag);
    return m_tag;
}

byte * WordCount::input()
{
    return (byte *)m_input.data();
}

int WordCount::input_size()
{
    return m_input.size();
}

byte * WordCount::output()
{
    return (byte *)m_output.data();
}

void WordCount::process()
{
    std::string word;
    CounterMap counter;

    // currently we do tokenization by whitespace or common punctuations
    static std::string delim = " ,;.?!\t\n";
    size_t delim_pos = m_input.find_first_of(delim);
    size_t word_pos = 0;
    while (delim_pos !=std::string::npos) {
        // in case of continous punctuations
        if ((delim_pos - word_pos) > 1) {
            word = m_input.substr(word_pos, delim_pos - word_pos);
            ++counter[word];
        }

        word_pos = delim_pos + 1;

        delim_pos = m_input.find_first_of(delim, word_pos);
    }
    // last word
    if (word_pos < m_input.size()) {
        word = m_input.substr(word_pos, m_input.size() - word_pos);
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