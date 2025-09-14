#include <stddef.h>
#include <stdbool.h>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
// commands
// play_sample
// channelnum

#define AUDIO_EVENT_SIZE 256

#define STARTING_BPM    160
#define SAMPLE_RATE     48000
#define FRAME_COUNT     600     // PCM Frame = 2 samples in stereo audio

typedef struct trkr_cfg {
    ma_device_config ma_config;
    ma_uint32 bpm;
    ma_uint32 frame_count;
} trkr_cfg;

trkr_cfg* trkr_init() {
    
}

typedef struct trkr_node {

} trkr_node;


typedef struct audio_file_t {
    char* name;
    int idx_start;
    int idx_end;
    ma_decoder* data;
} audio_file_t;

/// audio files ///
audio_file_t* new_audio_file();
bool load_audio_file(const char* fname, audio_file_t* afile);
/// ringbuf ///
void rb_init();
void rb_uninit();
void write_to_rb();
/// devices /// 
void device_init();
void device_uninit();
/// idk ///
void listen_and_dispatch(); // listen for events and write to ringbuffer
// event functions that write to voices array
void mix_and_generate_out();
void audio_callback();


// ops?
void set_bpm();

typedef struct audio_event_t {
    char* log;
};
typedef struct channel_params { // channel_data?
    // mostly state for ui stuff I guess
    int muted;
    float volume;
    int pan;
    char* name;
};

void main() {
    device_init();
    return;
}

audio_file_t* new_audio_file() {
    audio_file_t f;
    return &f;
}

bool load_audio_file(const char* fname, audio_file_t* afile) {
    ma_result result = ma_decoder_init_file(fname, NULL, afile->data);
    if (result != MA_SUCCESS) return false;
    return true;
}

void write_to_rb() {
    printf("rb updated\n");

    // #IFDEF AUDIO_ENGINE_RAYLIB
    // #IFDEF AUDIO_ENGINE_MINIAUDIO
    // #IFDEF AUDIO_ENGINE_CSOUND
}

void audio_callback() { // data callback?
    listen_and_dispatch();
}

void listen_and_dispatch() {
    // switch case:
    // hardcoded opcodes
    // forth opcodes
    int total_frames_read = 0;
    int frame_count;

    while (total_frames_read < frame_count) {

    }
}

