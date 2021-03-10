#pragma once

#define RACAM_UDP_RECORD_SIZE 32768

#include <array>
#include <memory>
#include <stdint.h>

#pragma pack(push, 1)

struct UDPRecord_Header{
    /*
     * Application Layer Info
     */
    uint16_t versionInfo; /// version of this header (UDP_RECORD_VERSIONINFO)
    uint16_t sourceTxCnt; ///
    uint32_t sourceTxTime; /// time stamp (ms)
    uint8_t sourceInfo; /// [0:63] identifying sending application
    uint8_t reservedSrc1; /// 0
    uint8_t reservedSrc2; /// 0
    uint8_t reservedSrc3; /// 0

    /*
     * Process Layer Info
     */
    uint32_t streamRefIndex; /// Stream specific index used for data retrieval
    uint16_t streamDataLen; /// stream data payload size (bytes)
    uint8_t streamTxCnt; /// how many time this stream sent
    uint8_t streamNumber; /// stream number [0:31]
    uint8_t streamVersion; ///data sturcture format / version
    uint8_t streamChunks; ///0 if transmittion is not part of a series of chunk, set to M>=2 for a series of M chunks
    uint8_t streamChunkIdx; /// chunk id in a series of chunk
    uint8_t reservedStr3; /// 0

};

/// Structure for holding an entire UDP message.
struct UDPRecordBuffer_t
{
    /// Header of the UDP message
    UDPRecord_Header header;
    /// Payload of the UDP message
    std::array<unsigned char, RACAM_UDP_RECORD_SIZE> payload;
};                            

static const int UDP_RECORD_VERSION = 0xA1;
static const uint16_t UDP_RECORD_VERSIONINFO = ((UDP_RECORD_VERSION << 8) | sizeof(UDPRecord_Header));

#pragma pack(pop)

