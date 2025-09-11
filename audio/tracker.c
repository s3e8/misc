

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
// commands
// play_sample
// channelnum


#define AUDIO_EVENT_SIZE 256

void rb_init();
void device_init();
void rb_uninit();
void device_uninit();
void listen_and_dispatch(); // listen for events and write to ringbuffer
void write_to_rb();
// event functions that write to voices array
void mix_and_generate_out();
void audio_callback();

void load_file();

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
}

bool load_file(const char* f) {
    ma_decoder decoder;
    ma_result result = ma_decoder_init_file(f, null, &decoder);
    if (result != MA_SUCCESS) return false;
}