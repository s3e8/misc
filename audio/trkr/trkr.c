#include <stdbool.h>

typedef struct audio_buffer_t
{
    // char*   name;
    // original_format;
    // file_location;
    int sample_rate;
    int     length_in_frames;
    bool    is_playing;
    // ma_decoder buffer;
    int range_start_frame;
    int range_end_frame;
    int loop_start_frame;
    int loop_end_frame;
} audio_buffer_t;

// void audio_buffer_vtable =
// {
    
// }

audio_buffer_t* new_audio_buffer();
audio_buffer_t* audio_buffer_init()
{

}

// data_source api:

void audio_callback()
{
    
}


int main()
{

}