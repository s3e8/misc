#include <stdbool.h>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#define TSF_IMPLEMENTATION
// #include "tinysoundfont.h"

#define FORMAT                  ma_format_f32
#define CHANNELS                2
#define SAMPLE_RATE             48000
#define BUFFER_SIZE             1024
#define BUFFER_SIZE_IN_FRAMES   512

#define DEFAULT_BPM 160;

/*
    a   node            is...
    a   device          is...
    an  engine          is...
    a   channel         is...
    a   voice           is...
    an  audio buffer    is...
    the flow is as follows: voices->mix_channel->mix_out (sum_channel->sum_out?)
*/ 

typedef struct trkr_state_t {
    ma_pcm_rb   rb; // should this just be a pointer and declared elsewhere for speed?
    //
    float       bpm;
    int         channels;
    int         sample_rate;
    // int         total_frames_written; // this seems like an extra step in the loop
    int         total_frames_per_beat;
    int         total_frames_until_next_beat;
} trkr_state_t;

void audio_callback(ma_device* p_device, void* p_output, const void* p_input, ma_uint32 frame_count)
{
    (void)p_device; // do we need this?
    trkr_state_t* p_trkr = (trkr_state_t*)p_device->pUserData;

    void* p_read_buffer;
    ma_uint32 frames_read = frame_count; // apparently this is modified by acquire read
    

    // Init the output buffer to 0
    memset(p_output, 0, frame_count * p_device->playback.channels * ma_get_bytes_per_sample(p_device->playback.format));

    ma_silence_pcm_frames(p_output, frame_count, p_device->playback.format, p_device->playback.channels);

    // void* p_read_buffer; // why is this needed?
    // ma_uint32 frames_read       = 0;
    // ma_uint32 frames_to_read    = frame_count;
    // float tmp_audiobuf[BUFFER_SIZE_IN_FRAMES * CHANNELS] = {0}; // todo: read this stuff from trkr_state_t

    // ma_pcm_rb_acquire_read(&p_trkr->rb, &frames_to_read, &p_read_buffer); // not p_output?
    // // while (frames_read < frame_count)
    // while (frames_to_read > 0)
    // {
    //     // frames_to_read = frame_count - frames_read;

    //     ma_copy_pcm_frames(p_output, p_read_buffer, frames_to_read, p_device->playback.format, p_device->playback.channels);
    //     ma_pcm_rb_commit_read(&p_trkr->rb, frames_to_read);
    // }
}

int main() 
{
    trkr_state_t trkr = {0};
    trkr.bpm                            = DEFAULT_BPM; // todo: init with random bpm value...
    // trkr.frames_written                 = 0;
    trkr.total_frames_per_beat          = 512;
    trkr.total_frames_until_next_beat   = 512;

    ma_device_config cfg = ma_device_config_init(ma_device_type_playback);
    cfg.playback.format     = FORMAT;
    cfg.playback.channels   = CHANNELS;
    cfg.sampleRate          = SAMPLE_RATE;
    cfg.dataCallback        = NULL;
    cfg.pUserData           = &trkr;

    ma_device device;
    if (ma_device_init(NULL, &cfg, &device) != MA_SUCCESS)
    {
        // error:
        printf("Failed to initialize device\n");
        // ma_pcm_rb_uninit(&state.ring_buffer);
        return -1;
    }

    ma_result result;

    ma_pcm_rb rb;
    result = ma_pcm_rb_init(cfg.playback.format, cfg.playback.channels, BUFFER_SIZE_IN_FRAMES, NULL, NULL, &trkr.rb);
    if (result != MA_SUCCESS)
    {
        // error:
        printf("Failed to initialize ring buffer\n");
        return -1;
    }

    //
    // prep main loop:
    void* p_write_buffer; // ?
    const ma_uint32 frames_per_write    = BUFFER_SIZE_IN_FRAMES; // can this be a regular int?
    ma_uint32       frames_to_write     = frames_per_write;
    ma_uint32       frames_available    = frames_per_write;
    float tmp_audiobuf[frames_per_write * CHANNELS]; // do we need this?

    // other init stuff:
    float   seconds_per_beat        = 60.0f / trkr.bpm;
    float   frames_per_beat         = seconds_per_beat * cfg.sampleRate;
    int     frames_until_next_beat  = frames_per_beat;
    int     beat_count              = 0;
    // int beats_per_measure;
    // int which_beat // downbeat;

    // start the device and go into main loop:
    ma_device_start(&device);
    printf("Device started. Press Ctrl+C to quit.\n");

    // main loop:
    while (1)
    {
        frames_available = ma_pcm_rb_available_write(&rb);
        if (frames_available >= frames_per_write)
        {
            ma_pcm_rb_acquire_write(&rb, &frames_to_write, &p_write_buffer);

            if (p_write_buffer != NULL && frames_to_write > 0)
            {
                frames_until_next_beat--;
                if (frames_until_next_beat <= 0)
                {
                    beat_count++;
                    printf("TICK: %d\n", beat_count);
                    // play metronome:
                    // todo
                }
            }

            ma_sleep(10);
        }
    }

    // Cleanup (this won't be reached in this example)
    ma_device_uninit(&device);
    ma_pcm_rb_uninit(&rb);
    return 0;
}