#ifndef STM32F4_AUDIO_H_INCLUDED
#define STM32F4_AUDIO_H_INCLUDED

class stm32f4_audio
{
public:
    stm32f4_audio();
    ~stm32f4_audio();

    void init( uint8_t volume, uint32_t audioFreq );

    void playBuffer( uint16_t * buffer, uint32_t buffer_size );

    void stop();
};

#endif /* STM32F4_AUDIO_H_INCLUDED */
