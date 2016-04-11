/******************************************************************************

  Robot Toolkit ++ (RTK++)

  Copyright (c) 2007-2013 Shuhui Bu <bushuhui@nwpu.edu.cn>
  http://www.adv-ci.com

  ----------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

#ifndef __RTK_DATASTREAM_H__
#define __RTK_DATASTREAM_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <string>
#include <vector>
#include <ostream>

#include "base/debug/debug_config.h"

namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline void RDataStream_memcpy(void *dst, const void *src, int len)
{
    uint8_t *pd = (uint8_t*) dst,
            *ps = (uint8_t*) src;
    
    for(int i=0; i<len; i++) pd[i] = ps[i];
}
    


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The DataStream class
///
class RDataStream
{
public:
    ///
    /// \brief RDataStream - default initialize
    ///
    RDataStream() {
        m_fromRawData = 0;
        init();
    }

    ///
    /// \brief RDataStream - set to given length
    /// \param length - stream length
    ///
    RDataStream(uint32_t length) {
        m_fromRawData = 0;
        init();
        resize(length);
    }

    ///
    /// \brief RDataStream - from raw data (read-only)
    /// \param d    - byte array
    /// \param l    - array size
    ///
    RDataStream(uint8_t *d, uint32_t l) {
        m_fromRawData = 1;
        m_arrData = NULL;
        fromRawData_noCopy(d, l);
    }

    ///
    /// \brief RDataStream - from string (read-only)
    /// \param dat  - raw data encapsuled to string
    ///
    RDataStream(std::string &dat) {
        m_fromRawData = 1;
        m_arrData = NULL;
        fromRawData_noCopy((uint8_t*) dat.c_str(), dat.size());
    }

    ///
    /// \brief ~RDataStream
    ///
    virtual ~RDataStream() {
        if( !m_fromRawData )
            release();
    }

    ///
    /// \brief clear all data
    ///
    void clear(void) {
        clear_noDel();
    }

	///
	/// \brief clear without delete buffer
	///
    void clear_noDel(void) {
        if( m_fromRawData ) {
            dbg_pe("Datastream from raw data! Please do not modify it\n");
            exit(1);
        }

        m_size = 2*sizeof(uint32_t);
        m_idx  = 2*sizeof(uint32_t);
    }


    ///
    /// \brief resize stream size
    /// \param n    - new size
    /// \param c    - default byte value
    /// \return
    ///
    int resize(int n, int32_t c=-1) {
        if( m_fromRawData ) {
            dbg_pe("Datastream from raw data! Please do not modify it\n");
            exit(1);
        }

        if( n == m_size ) {
            return 0;
        } else if ( n < m_size ) {
            if( n < 2*sizeof(uint32_t) ) n = 2*sizeof(uint32_t);
            if( m_idx > n ) m_idx = n;
        } else {
            if( n > m_sizeReserved ) {
                int nSizeRev = n*2;
                reserve(nSizeRev, c);
            }
        }
        
        m_size = n;
        updateHeader();

        return 0;
    }

    ///
    /// \brief reserve given length's buffer
    /// \param n    - buffer size
    /// \param c    - default byte value
    /// \return
    ///
    int reserve(int n, int32_t c=-1) {
        if( m_fromRawData ) {
            dbg_pe("Datastream from raw data! Please do not modify it\n");
            exit(1);
        }

        if( n <= m_sizeReserved ) {
            return -1;
        }

        // alloc new buffer
        uint8_t* arrN, cb;
        arrN = new uint8_t[n];

        if( m_idx > 0 ) {
            RDataStream_memcpy(arrN, m_arrData, m_idx);

            // set new alloc data to given c
            if( c >= 0 ) {
                cb = c;
                for(int i=m_idx; i<n; i++) arrN[i] = cb;
            }

            // release old data
            delete [] m_arrData;
        }

        // set data pointers
        m_arrData = arrN;
        m_sizeReserved = n;

        return 0;
    }

    ///
    /// \brief set RDataStream to given raw data
    ///
    /// \param d        - raw data byte array
    /// \param l        - array size
    /// \param doCopy   - use provide data or not
    ///
    /// \return         - this object
    ///
    /// \note
    ///     When you set doCopy=0, be careful the data need to be manually managed
    ///
    RDataStream& fromRawData(uint8_t *d, uint32_t l, int doCopy=1) {
        if( doCopy ) {
            if( l > m_sizeReserved ) reserve(l);

            RDataStream_memcpy(m_arrData, d, l);
            m_size = l;
            m_idx = 2*sizeof(uint32_t);
        } else {
            if( !m_fromRawData ) {
                if( m_arrData != NULL ) delete [] m_arrData;
            }

            m_fromRawData = 0;
            m_arrData = d;
            m_size = l;
            m_sizeReserved = l;

            m_idx = 2*sizeof(uint32_t);
        }

        return *this;
    }

    ///
    /// \brief set RDataStream to given raw data (without copy)
    /// \param d    - raw data byte array
    /// \param l    - array size
    /// \return     - this object
    ///
    RDataStream& fromRawData_noCopy(uint8_t *d, uint32_t l) {
        if( !m_fromRawData ) {
            if( m_arrData != NULL ) delete [] m_arrData;
        }

        m_fromRawData = 1;
        m_arrData = d;
        m_size = l;
        m_sizeReserved = l;

        m_idx = 2*sizeof(uint32_t);

        return *this;
    }


    ///
    /// \brief rewind to beginning position
    /// \return
    ///
    int rewind(void) {
        m_idx = 2*sizeof(uint32_t);
        return 0;
    }

    ///
    /// \brief seek pointer position
    /// \param offset       - offset
    /// \param whence       - baseline position
    ///                         SEEK_SET - beginning position
    ///                         SEEK_CUR - current position
    ///                         SEEK_END - end of stream
    /// \return
    ///     0   - success
    ///     -1  - given position outside the stream
    ///
    int seek(int32_t offset, int whence=SEEK_SET) {
        int32_t np, bp;

        if( whence == SEEK_SET ) bp = 0;
        if( whence == SEEK_CUR ) bp = m_idx;
        if( whence == SEEK_END ) bp = m_size;

        np = bp + offset;
        if( np > m_size ) return -1;
        if( np < 2*sizeof(uint32_t) ) np = 2*sizeof(uint32_t);

        m_idx = np;
        return 0;
    }

    uint32_t headerSize(void) {
        return 2*sizeof(uint32_t);
    }

    ///
    /// \brief get the size of stream
    /// \return stream length
    ///
    /// \see length()
    ///
    uint32_t size(void) {
        return m_size;
    }

    ///
    /// \brief get the length of stream
    /// \return stream length
    ///
    /// \see size()
    ///
    uint32_t length(void) {
        return m_size;
    }

    ///
    /// \brief return stream raw data
    /// \return - raw data pointer
    ///
    uint8_t* data(void) {
        return m_arrData;
    }

    ///
    /// \brief get current data pointer
    ///
    /// \return - raw data pointer (with the position of current position)
    ///
    uint8_t* currDataPtr(void) {
        return m_arrData + m_idx;
    }

    ///
    /// \brief set RDataStream header
    ///
    /// \param magic    - magic number
    /// \param ver      - version
    ///
    /// \return
    ///     0           - success
    ///
    int setHeader(uint32_t magic, uint32_t ver) {
        uint32_t    mv;

        mv = (ver << 16) | (magic & 0x0000FFFF);
        RDataStream_memcpy(m_arrData, &mv, sizeof(uint32_t));

        return 0;
    }

    ///
    /// \brief get RDataStream header
    ///
    /// \param magic    - magic number
    /// \param ver      - version
    ///
    /// \return
    ///     0           - success
    ///
    int getHeader(uint32_t& magic, uint32_t& ver) {
        uint32_t    mv;

        RDataStream_memcpy(&mv, m_arrData, sizeof(uint32_t));

        magic   = mv & 0x0000FFFF;
        ver     = mv >> 16;

        return 0;
    }

    ///
    /// \brief setVerNum
    /// \param ver - version number
    /// \return
    ///
    int setVerNum(uint32_t magic) {
        uint32_t mv;
        uint8_t* p;

        p = m_arrData;
        RDataStream_memcpy(&mv, p, sizeof(uint32_t));

        mv = mv & 0x0000FFFF;
        mv = mv | (magic << 16);
        RDataStream_memcpy(p, &mv, sizeof(uint32_t));

        return 0;
    }

    ///
    /// \brief getVerNum
    /// \param ver - version number
    /// \return
    ///
    int getVerNum(uint32_t& magic) {
        uint32_t mv;
        uint8_t* p;

        p = m_arrData;
        RDataStream_memcpy(&mv, p, sizeof(uint32_t));

        magic = mv >> 16;

        return 0;
    }

    ///
    /// \brief setMagicNum
    /// \param magic - magic number
    /// \return
    ///
    int setMagicNum(uint32_t ver) {
        uint32_t mv;
        uint8_t* p;

        p = m_arrData;
        RDataStream_memcpy(&mv, p, sizeof(uint32_t));

        mv = mv & 0xFFFF0000;
        mv = mv | (ver & 0x0000FFFF);
        RDataStream_memcpy(p, &mv, sizeof(uint32_t));

        return 0;
    }

    ///
    /// \brief getMagicNum
    /// \param magic - magic number
    /// \return
    ///
    int getMagicNum(uint32_t& ver) {
        uint32_t mv;
        uint8_t* p;

        p = m_arrData;
        RDataStream_memcpy(&mv, p, sizeof(uint32_t));

        ver = mv & 0x0000FFFF;

        return 0;
    }


    ///
    /// \brief update stream header
    /// \return
    ///
    int updateHeader(void) {
        uint8_t* p;

        p = m_arrData + sizeof(uint32_t);
        RDataStream_memcpy(p, &m_size, sizeof(uint32_t));

        return 0;
    }



public:

    #define write_(d) \
        uint32_t sn, dl; \
        dl = sizeof(d); \
        sn = m_idx + dl; \
        if( sn > m_sizeReserved ) reserve(sn*2); \
        RDataStream_memcpy((m_arrData+m_idx), &d, dl); \
        m_idx += dl; \
        if( m_idx > m_size ) m_size = m_idx; \
        updateHeader(); \
        return 0;

    ///
    /// \brief write int8_t data
    /// \param d - int8_t data
    /// \return
    ///
    int write(int8_t &d) {
        write_(d);
    }

    ///
    /// \brief write uint8_t data
    /// \param d - uint8_t data
    /// \return
    ///
    int write(uint8_t &d) {
        write_(d);
    }

    ///
    /// \brief write int16_t data
    /// \param d - int16_t data
    /// \return
    ///
    int write(int16_t &d) {
        write_(d);
    }


    ///
    /// \brief write uint16_t data
    /// \param d - uint16_t data
    /// \return
    ///
    int write(uint16_t &d) {
        write_(d);
    }

    ///
    /// \brief write int32_t data
    /// \param d
    /// \return
    ///
    int write(int32_t &d) {
        write_(d);
    }


    ///
    /// \brief write uint32_t data
    /// \param d
    /// \return
    ///
    int write(uint32_t &d) {
        write_(d);
    }

    ///
    /// \brief write int64_t data
    /// \param d
    /// \return
    ///
    int write(int64_t &d) {
        write_(d);
    }

    ///
    /// \brief write uint64_t data
    /// \param d
    /// \return
    ///
    int write(uint64_t &d) {
        write_(d);
    }

    ///
    /// \brief write float (float) data
    /// \param d
    /// \return
    ///
    int write(float &d) {
        write_(d);
    }

    ///
    /// \brief write double (double) data
    /// \param d
    /// \return
    ///
    int write(double &d) {
        write_(d);
    }

    ///
    /// \brief write binary data
    /// \param d   - binary data array
    /// \param len - length in byte
    /// \return
    ///
    int write(uint8_t *d, uint32_t len) {
        uint32_t sn, dl;

        dl = len;
        sn = m_idx + dl;
        if( sn > m_sizeReserved ) reserve(sn*2);

        RDataStream_memcpy((m_arrData+m_idx), d, dl);

        m_idx += dl;
        if( m_idx > m_size ) m_size = m_idx;
        updateHeader();

        return 0;
    }

    ///
    /// \brief write a std::string obj
    /// \param s - std::string obj
    ///
    /// \return
    ///     0           - success
    ///
    int write(std::string &s) {
        uint32_t    sl;
        uint32_t    sn, dl;

        // determine length
        sl = s.size();
        dl = sizeof(uint32_t) + sl + 1;
        sn = m_idx + dl;
        if( sn > m_sizeReserved ) reserve(sn*2);

        // copy data
        RDataStream_memcpy((m_arrData+m_idx), &sl, sizeof(uint32_t));
        RDataStream_memcpy((m_arrData+m_idx+sizeof(uint32_t)), (void*) s.c_str(), sl);
        m_arrData[m_idx + sizeof(uint32_t) + sl] = 0;

        // update index
        m_idx += dl;
        if( m_idx > m_size ) m_size = m_idx;
        updateHeader();

        return 0;
    }

    ///
    /// \brief write RDataStream data
    /// \param d - RDataStream data
    /// \return
    ///
    int write(RDataStream &d) {
        uint32_t sn, dl;

        dl = d.size();
        sn = m_idx + dl;
        if( sn > m_sizeReserved ) reserve(sn*2);

        RDataStream_memcpy((m_arrData+m_idx), d.data(), dl);

        m_idx += dl;
        if( m_idx > m_size ) m_size = m_idx;
        updateHeader();

        return 0;
    }



    #define read_(d) \
        uint32_t sn, dl; \
        dl = sizeof(d); \
        sn = m_idx + dl; \
        if( sn > m_size ) return -1; \
        RDataStream_memcpy(&d, (m_arrData+m_idx), dl); \
        m_idx += dl; \
        return 0; \


    ///
    /// \brief read int8_t data
    /// \param d
    /// \return
    ///
    int read(int8_t &d) {
        read_(d);
    }

    ///
    /// \brief read uint8_t data
    /// \param d
    /// \return
    ///
    int read(uint8_t &d) {
        read_(d);
    }

    ///
    /// \brief read int16_t data
    /// \param d
    /// \return
    ///
    int read(int16_t &d) {
        read_(d);
    }

    ///
    /// \brief read uint16_t data
    /// \param d
    /// \return
    ///
    int read(uint16_t &d) {
        read_(d);
    }

    ///
    /// \brief read int32_t data
    /// \param d
    /// \return
    ///
    int read(int32_t &d) {
        read_(d);
    }

    ///
    /// \brief read uint32_t data
    /// \param d
    /// \return
    ///
    int read(uint32_t &d) {
        read_(d);
    }

    ///
    /// \brief read int64_t data
    /// \param d
    /// \return
    ///
    int read(int64_t &d) {
        read_(d);
    }

    ///
    /// \brief read uint64_t data
    /// \param d
    /// \return
    ///
    int read(uint64_t &d) {
        read_(d);
    }

    ///
    /// \brief read float (float) data
    /// \param d
    /// \return
    ///
    int read(float &d) {
        read_(d);
    }

    ///
    /// \brief read double (double) data
    /// \param d
    /// \return
    ///
    int read(double &d) {
        read_(d);
    }

    ///
    /// \brief read binary data
    /// \param d   - binar data array
    /// \param len - length in byte
    /// \return
    ///
    int read(uint8_t *d, int len) {
        uint32_t sn, dl;

        dl = len;
        sn = m_idx + dl;
        if( sn > m_size ) return -1;

        RDataStream_memcpy(d, (m_arrData+m_idx), dl);

        m_idx += dl;

        return 0;
    }

    ///
    /// \brief read a std::string obj
    /// \param s - std::string obj
    ///
    /// \return
    ///     0       - success
    ///     -1      - out of index
    ///
    int read(std::string &s) {
        uint32_t    sl;
        uint32_t    sn, dl;
        char        *buf;

        // read string length
        dl = sizeof(uint32_t);
        sn = m_idx + dl;
        if( sn > m_size ) return -1;
        RDataStream_memcpy(&sl, (m_arrData+m_idx), dl);

        // determine total length
        sn = m_idx + sizeof(uint32_t) + sl;
        if( sn > m_size ) return -1;

        // read string
        buf = (char*) (m_arrData+m_idx+sizeof(uint32_t));
        s = buf;

        // update index
        m_idx += sizeof(uint32_t) + sl + 1;

        return 0;
    }

    ///
    /// \brief read RDataStream data
    /// \param d - RDataStream data
    /// \return
    ///
    int read(RDataStream &d) {
        uint32_t hl, sn, dl;

        // read header information
        hl = 2*sizeof(uint32_t);
        if( m_idx + hl > m_size ) return -1;
        RDataStream_memcpy(&dl, (m_arrData+m_idx+sizeof(uint32_t)), sizeof(uint32_t));

        // check length
        sn = m_idx + dl;
        if( sn > m_size ) return -1;

        // read data
        d.fromRawData(m_arrData+m_idx, dl);
        m_idx += dl;

        return 0;
    }

    ///
    /// \brief read RDataStream data (fast without memcpy)
    /// \param d - RDataStream data
    /// \return
    ///
    int readFast(RDataStream &d) {
        uint32_t hl, sn, dl;

        // read header information
        hl = 2*sizeof(uint32_t);
        if( m_idx + hl > m_size ) return -1;
        RDataStream_memcpy(&dl, (m_arrData+m_idx+sizeof(uint32_t)), sizeof(uint32_t));

        // check length
        sn = m_idx + dl;
        if( sn > m_size ) return -1;

        // read data
        d.fromRawData_noCopy(m_arrData+m_idx, dl);
        m_idx += dl;

        return 0;
    }


    RDataStream& operator << (int8_t &d) {
        write(d);
        return *this;
    }


    RDataStream& operator << (uint8_t &d) {
        write(d);
        return *this;
    }

    RDataStream& operator << (int16_t &d) {
        write(d);
        return *this;
    }


    RDataStream& operator << (uint16_t &d) {
        write(d);
        return *this;
    }

    RDataStream& operator << (int32_t &d) {
        write(d);
        return *this;
    }


    RDataStream& operator << (uint32_t &d) {
        write(d);
        return *this;
    }

    RDataStream& operator << (int64_t &d) {
        write(d);
        return *this;
    }


    RDataStream& operator << (uint64_t &d) {
        write(d);
        return *this;
    }

    RDataStream& operator << (float &d) {
        write(d);
        return *this;
    }


    RDataStream& operator << (double &d) {
        write(d);
        return *this;
    }

    RDataStream& operator << (std::string &s) {
        write(s);
        return *this;
    }

    RDataStream& operator << (RDataStream &d) {
        write(d);
        return *this;
    }


    RDataStream& operator >> (int8_t &d) {
        read(d);
        return *this;
    }


    RDataStream& operator >> (uint8_t &d) {
        read(d);
        return *this;
    }

    RDataStream& operator >> (int16_t &d) {
        read(d);
        return *this;
    }


    RDataStream& operator >> (uint16_t &d) {
        read(d);
        return *this;
    }

    RDataStream& operator >> (int32_t &d) {
        read(d);
        return *this;
    }


    RDataStream& operator >> (uint32_t &d) {
        read(d);
        return *this;
    }

    RDataStream& operator >> (int64_t &d) {
        read(d);
        return *this;
    }


    RDataStream& operator >> (uint64_t &d) {
        read(d);
        return *this;
    }

    RDataStream& operator >> (float &d) {
        read(d);
        return *this;
    }


    RDataStream& operator >> (double &d) {
        read(d);
        return *this;
    }

    RDataStream& operator >> (std::string &s) {
        read(s);
        return *this;
    }

    RDataStream& operator >> (RDataStream &d) {
        read(d);
        return *this;
    }


    void print(void) {
        uint32_t    magic, ver;

        getHeader(magic, ver);
        printf("DataStream: magic = 0x%04X, ver = %d, size = %d\n content = ", magic, ver, m_size);
        for(int i=2*sizeof(uint32_t); i<m_size; i++) {
            printf("0x%02X ", m_arrData[i]);
        }
        printf("\n");
    }

protected:

    ///
    /// \brief init RDataStream obj
    ///
    void init(void) {
        if( m_fromRawData ) {
            dbg_pe("Datastream from raw data! Please do not modify it\n");
            exit(1);
        }

        m_fromRawData = 0;
        m_sizeReserved = 2048;

        m_arrData = new uint8_t[m_sizeReserved];
        for(int i=0; i<m_sizeReserved; i++) m_arrData[i] = 0;

        m_size = 2*sizeof(uint32_t);
        m_idx  = 2*sizeof(uint32_t);
    }

    ///
    /// \brief release RDataStream obj
    ///
    void release(void) {
        if( m_fromRawData ) {
            dbg_pe("Datastream from raw data! Please do not modify it\n");
            exit(1);
        }

        if( m_arrData != NULL ) {
            delete [] m_arrData;
            m_arrData = NULL;
        }

        m_idx = 0;
        m_sizeReserved = 0;
        m_size = 0;
    }

protected:
    uint8_t*                m_arrData;                  ///< data array
    uint32_t                m_idx;                      ///< current position
    uint32_t                m_size;                     ///< current length
    uint32_t                m_sizeReserved;             ///< stream actual memory size (reserved)

    int                     m_fromRawData;              ///< datastream from raw data
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline int datastream_get_header(uint8_t *buf, uint32_t &magic, uint32_t &ver)
{
    uint32_t mv;

    memcpy(&mv, buf, sizeof(uint32_t));

    magic   = mv & 0x0000FFFF;
    ver     = mv >> 16;

    return 0;
}

inline uint32_t datastream_get_length(uint8_t *buf)
{
    uint32_t len;

    memcpy(&len, buf+4, sizeof(uint32_t));

    return len;
}

inline int datastream_set_header(uint8_t *buf, uint32_t magic, uint32_t ver, uint32_t len)
{
    uint32_t mv;

    mv = ver << 16 | (magic & 0x0000FFFF);

    memcpy(buf,   &mv,  sizeof(uint32_t));
    memcpy(buf+4, &len, sizeof(uint32_t));

    return 0;
}

} // end of namespace pi

#endif // __RTK_DATASTREAM_H__

