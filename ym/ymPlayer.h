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

    typedef struct
    {
        int32_t     frequency;
        int32_t     phase;
        uint8_t     waveform;
        uint8_t     volume;
    } ymOscillator;

public:
    ymPlayer();
    ~ymPlayer();

    void createBuffer( int32_t bufferSize );

    void openFileFromMemory( const uint8_t * music );
    void openFileFromSD();
    void close();

    void printInfo();
    void printFrames();

    void getSampleData( int16_t * buffer, uint32_t buffer_size, uint32_t playFreq, int half );

    void play();
    void stop();

private:
    int16_t *           m_soundBuffer;
    uint32_t            m_soundBufferSize;

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

    ymOscillator        m_oscillator[3];

    uint32_t            m_bufferIndex;
};

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

