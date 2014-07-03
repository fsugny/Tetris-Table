//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#ifndef YM_PLAYER_H
#define YM_PLAYER_H

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "stm32f4xx.h"
#include "stm32f4_audio.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

class ymPlayer
{
public:
    typedef struct
    {
        uint8_t     fileId[5];
        uint8_t     signature[9];
        uint32_t    nbFrames;
        uint32_t    songAttributes;
        uint16_t    nbDigidrumSamples;
        uint32_t    masterClock;
        uint16_t    originalClock;
        uint32_t    loopFrame;
        uint16_t    paddingSize;
        char *      songName;
        char *      songAuthor;
        char *      songComment;
        uint8_t     endMarker[5];
    } ymHeader;

    enum
    {
        ymInterleavedData = 0,
        ymSignedDigiDrumData = 1,
        ymDigiDrum4Bits = 2
    } ymSongAttributes;

public:
    ymPlayer();
    ~ymPlayer();

    void openFileFromMemory( const uint8_t * music );
    void openFileFromSD();
    void close();

    void printInfo();
    void printFrames();

public:
    const uint8_t *     m_music;
    uint32_t            m_nbFrames;
    uint32_t            m_songAttribute;

    ymHeader            m_header;
    uint8_t *           m_frameRegister;

    uint8_t             readByteFromMemory( uint32_t & offset );
    uint16_t            readWordFromMemory( uint32_t & offset );
    uint32_t            readLWordFromMemory( uint32_t & offset );
    char *              readString( uint32_t & offset );

    uint32_t            m_currentFrame;
};

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

