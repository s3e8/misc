#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// setup device capture to grab computer audio like soundflower or whateever

void audio_callback(ma_device *pDevice, void *pOutput, const void* pInput, ma_uint32 framecount);

// int main() 
// {
//     ma_device_config config = ma_device_config_init(ma_device_type_playback);
//     config.playback.format = ma_format_f32;
//     config.playback.channels = 2;
//     config.sampleRate = 48000;
//     // config.dataCallback = data_callback;
//     // config.pUserData = pMyCustomData;

//     ma_device device;
//     if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
//         return -1;
//     }

    

//     ma_device_start(&device);

//     // program's main loop?

//     ma_device_uninit(&device);
//     return 0;
// }

int main() 
{
    ma_result result;
    ma_engine engine;

    result = ma_engine_init(NULL, &engine);

    return 0;
}

void audio_callback(ma_device *device, void *outbuf, const void* inbuf, ma_uint32 framecount) {

}