#include <stdbool.h>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#define TSF_IMPLEMENTATION
// #include "tinysoundfont.h"

/*
    a   node            is...
    a   device          is...
    an  engine          is...
    a   channel         is...
    a   voice           is...
    an  audio buffer    is...
    the flow is as follows: voices->mix_channel->mix_out (sum_channel->sum_out?)
*/ 


// start with single voice, single channel

typedef struct debug_state_t {
    ma_atomic_uint32 last_frame_count;
    ma_atomic_uint32 callback_count;
    ma_atomic_uint32 underrun_count;
} debug_state_t;


// setup device capture to grab computer audio like soundflower or whatever
typedef struct metronome {
    float bval;
    float bpm;
    int samples_per_beat;
    bool play_metro;
};

typedef struct transport {
    bool playing;
} transport;

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
    config.playback.format      = ma_format_f32;
    config.playback.channels    = 2;
    config.sampleRate           = 48000;
    config.dataCallback         = audio_callback;
    config.pUserData            = NULL;

    ma_device device;
    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) 
    {
        return -1;
    }

    ma_result result;
    ma_engine engine;
    result = ma_engine_init(NULL, &engine);




    while (1) // while (!quit)?
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