#include "compression.h"

#include "crypto.h"
#include "sysutils.h"

#include <cstring>
#include <zlib.h>

Compression::Compression(int id, const byte *file, int file_size)
    : Function(id, FUNC_CP, "Compression"),
      m_input((char *)file, file_size),
      m_output(file_size*1.1+12, 0) // TBC
{
	m_data.reserve(m_name.size() + RAND_SIZE + m_input.size());
	m_data.assign(m_name);
	m_data.append(m_input.data(),m_input.data()+ m_input.size());
}

const byte *Compression::get_tag()
{
    // slightly deviate from the paper specification for ease of implmentation
	// TODO When CP 1kb put after 1024kb CP data and data+size is empty
	hash((byte *)(m_data.data()), m_name.size() + m_input.size(), m_tag);
	return m_tag;
}

byte *Compression::input()
{
    return (byte *)m_data.data();
}

int Compression::input_size()
{
    return m_data.size();
}

byte *Compression::output()
{
    return (byte *)m_output.data();
}

#define CHUNK 16384
#define CP_LEVEL 9 // highest compression level (hence slowest for our sake)
void Compression::process()
{
    int processed = 0, compressed = 0;
    int ret, flush;
    unsigned have;
    z_stream strm;
    int in_size = m_input.size();
    byte *in = (byte *)m_input.data();
    byte *out = (byte *)m_output.data();

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, CP_LEVEL);
    if (ret != Z_OK) {
        eprintf("[*] Compresison initializaion fails with error code %d!", ret);
        abort();
    }

    /* compress until end of file */
    do {
        if ((processed + CHUNK) < in_size) {
            strm.avail_in = CHUNK;
            flush = Z_NO_FLUSH;
        }
        else {
            strm.avail_in = in_size - processed;
            flush = Z_FINISH;
        }
        strm.next_in = &in[processed];

        /* run deflate() on input until output buffer not full, finish
        compression if all of source has been read in */ 
        do {
            strm.avail_out = CHUNK;
            strm.next_out = &out[compressed];

            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

            have = CHUNK - strm.avail_out;
            //if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
            //    (void)deflateEnd(&strm);
            //    //return Z_ERRNO;
            //}
            compressed += have;
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */
        processed += CHUNK;
    }
    /* done when last data in file processed */
    while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

     /* clean up and return */
    (void)deflateEnd(&strm);
    //return Z_OK;
    m_expt_output_size = compressed;
    eprintf(" [%d] \n", compressed);
}

void Compression::setR(const byte * r)
{
	m_data.append((const char*)r, (const char*)r + RAND_SIZE);
}
