#include <stdbool.h>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// setup device capture to grab computer audio like soundflower or whatever
typedef struct metronome {
    float bval;
    float bpm;
    int samples_per_beat;
    bool play_metro;
};

typedef struct audio_buffer {
    // paused
    // is_looping
    // cursor_pos
};

int convert_beat_to_sample(int beat, float bpm, int samplerate);

void audio_callback(ma_device *pDevice, void *pOutput, const void* pInput, ma_uint32 framecount);

// channel
// - vol
// pan
// delay

// sample_buffer

int main() 
{
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;
    config.playback.channels = 2;
    config.sampleRate = 48000;
    // config.dataCallback = data_callback;
    // config.pUserData = pMyCustomData;

    ma_result result;
    ma_engine engine;
    result = ma_engine_init(NULL, &engine);

    ma_device device;
    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) 
    {
        return -1;
    }


    while (1) 
    {
        if (play_metronome)
        {
            printf("click...\n");
        }
    }
    

    ma_device_start(&device);

    // program's main loop?

    ma_device_uninit(&device);
    return 0;
}

int convert_beat_to_sample(int beat, float bpm, int samplerate) {
    return ((float)beat * ((60.0f / bpm) * samplerate));
}

void audio_callback(ma_device *device, void *outbuf, const void* inbuf, ma_uint32 framecount) 
{
    if (device->pUserData == NULL) 
    {
        ma_silence_pcm_frames(outbuf, framecount, device->playback.format, device->playback.channels);
        return;
    }


    ma_uint32 frames_read = 0;
    while (1) 
    {
        if (frames_read >= framecount) break;
    }
}