
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "stm32f4xx_conf.h"
extern "C" {
#include "stm32f4_discovery_audio_codec.h"
}

#include "ymPlayer.h"
#include <stdio.h>
#include <string.h>

#define SWAPLWORD(lw) ((lw&0xFF)<<24|(lw&0xFF00)<<8|(lw&0xFF0000)>>8|(lw&0xFF000000)>>24)

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

int16_t ymVolumeTable[16] = { 0, 1, 1, 1, 2, 3, 4, 6, 8, 12, 17, 24, 36, 82, 127 };

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

ymPlayer::ymPlayer()
{
    m_music = NULL;
    m_nbFrames = 0x0;
    m_songAttribute = 0x0;
    m_frameRegister = NULL;
    m_currentFrame = 0;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

ymPlayer::~ymPlayer()
{
    m_music = NULL;
    m_nbFrames = 0x0;
    m_songAttribute = 0x0;
    delete [] m_frameRegister;
    m_currentFrame = 0;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

uint8_t ymPlayer::readByteFromMemory( uint32_t & offset )
{
    uint8_t tmp = m_music[offset];
    offset++;
    return tmp;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

uint16_t ymPlayer::readWordFromMemory( uint32_t & offset )
{
    uint16_t tmp = (m_music[offset+1]) + (m_music[offset]<<8);
    offset += 2;
    return tmp;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

uint32_t ymPlayer::readLWordFromMemory( uint32_t & offset )
{
    uint32_t tmp = (m_music[offset+3]) + (m_music[offset+2]<<8) + (m_music[offset+1]<<16) + (m_music[offset]<<24);
    offset += 4;
    return tmp;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

char * ymPlayer::readString( uint32_t & offset )
{
    uint32_t string_size = 0;
    uint32_t idx = offset;
    uint8_t tmp = m_music[idx];
    while ( (tmp = m_music[idx++]) != 0x00)
        string_size++;
    string_size++;
    char * new_string = new char[string_size+1];
    for (uint32_t i=0;i<string_size;++i)
        new_string[i] = m_music[offset+i];
    new_string[string_size] = '\0';

    offset += string_size;

    return new_string;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void ymPlayer::openFileFromMemory( const uint8_t * music )
{
    m_music = music;

    uint32_t    currentIndex =0;
    memcpy( m_header.fileId, &m_music[0], 4);
    m_header.fileId[4] = '\0';
    currentIndex += 4;
    memcpy( m_header.signature, &m_music[4], 8);
    m_header.signature[8] = '\0';
    currentIndex += 8;

    m_header.nbFrames = readLWordFromMemory(currentIndex);
    m_header.songAttributes = readLWordFromMemory(currentIndex);
    m_header.nbDigidrumSamples = readWordFromMemory(currentIndex);
    m_header.masterClock = readLWordFromMemory(currentIndex) >> 4;
    m_header.originalClock = readWordFromMemory(currentIndex);
    m_header.loopFrame = readLWordFromMemory(currentIndex);
    m_header.paddingSize = readWordFromMemory(currentIndex);

    // Add padding to Current Index
    currentIndex += m_header.paddingSize;

    // Reading song name
    m_header.songName = readString(currentIndex);
    // Reading song author
    m_header.songAuthor = readString(currentIndex);
    // Reading song comment
    m_header.songComment = readString(currentIndex);

    m_frameRegister = new uint8_t[m_header.nbFrames*16];

    // Reading frame register
    if ( m_header.songAttributes & (0x01<<ymInterleavedData) )
    {
        for ( int r=0; r<16; ++r)
            for ( uint32_t f=0; f<m_header.nbFrames; ++f )
                m_frameRegister[f*16+r] = m_music[currentIndex++];
    }
    else
    {
        for ( uint32_t f=0; f<m_header.nbFrames; ++f )
            for ( int r=0; r<16; ++r)
                m_frameRegister[f*16+r] = m_music[currentIndex++];
    }

    // Reading End Marker
    memcpy(m_header.endMarker, &m_music[currentIndex], 4);
    currentIndex += 4;
    m_header.endMarker[4] = '\0';
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void ymPlayer::openFileFromSD()
{

}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void ymPlayer::close()
{

}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void ymPlayer::printInfo()
{
    printf("File Id      : %s\n", m_header.fileId);
    printf("Signature    : %s\n", m_header.signature);
    printf("Nb Frames    : %lu\n", m_header.nbFrames);
    printf("Song Attrs   : %lx\n", m_header.songAttributes);
    printf("Nb Samples   : %d\n", m_header.nbDigidrumSamples);
    printf("Master Clock : %lu\n", m_header.masterClock*16);
    printf("Orig Clock   : %d\n", m_header.originalClock);
    printf("Loop Frame   : %lu\n", m_header.loopFrame);
    printf("Padding Size : %d\n", m_header.paddingSize);
    printf("Song Name    : %s\n", m_header.songName);
    printf("Author Name  : %s\n", m_header.songAuthor);
    printf("Song Comment : %s\n", m_header.songComment);
    printf("End Marker   : %s\n", m_header.endMarker);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void ymPlayer::printFrames()
{
    for (uint32_t f=0; f<m_header.nbFrames; ++f)
    {
        uint32_t index = f*16;
        printf("Frame %lu : A=%d B=%d C=%d N=%d M=%d VA=%d VB=%d VC=%d E=%d EShape=%d\n",
               f,
               m_frameRegister[index+0] + ((m_frameRegister[index+1]&0x0F)<<8),     // Period A
               m_frameRegister[index+2] + ((m_frameRegister[index+3]&0x0F)<<8),     // Period B
               m_frameRegister[index+4] + ((m_frameRegister[index+5]&0x0F)<<8),     // Period C
               m_frameRegister[index+6]&0x1F,                                       // Noise Period
               m_frameRegister[index+7]&0x3F,                                       // Mixer Control
               m_frameRegister[index+8]&0x1F,                                       // Volume A
               m_frameRegister[index+9]&0x1F,                                       // Volume B
               m_frameRegister[index+10]&0x1F,                                      // Volume C
               m_frameRegister[index+11] + ((m_frameRegister[index+12]&0x0F)<<8),   // Envelope Period
               m_frameRegister[index+13]&0x0F                                       // Envelope Shape
               );
    }
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

