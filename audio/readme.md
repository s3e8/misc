The third parameter of ma_sound_init_from_file() is a set of flags that control how the sound be loaded and a few options on which features should be enabled for that sound. By default, the sound is synchronously loaded fully into memory straight from the file system without any kind of decoding. If you want to decode the sound before storing it in memory, you need to specify the MA_SOUND_FLAG_DECODE flag. This is useful if you want to incur the cost of decoding at an earlier stage, such as a loading stage. Without this option, decoding will happen dynamically at mixing time which might be too expensive on the audio thread.

A sound can have its volume changed with ma_sound_set_volume(). If you prefer decibel volume control you can use ma_volume_db_to_linear() to convert from decibel representation to linear.

Panning and pitching is supported with ma_sound_set_pan() and ma_sound_set_pitch(). If you know a sound will never have its pitch changed with ma_sound_set_pitch() or via the doppler effect, you can specify the MA_SOUND_FLAG_NO_PITCH flag when initializing the sound for an optimization.

Sounds can be faded in and out with ma_sound_set_fade_in_pcm_frames().

To check if a sound is currently playing, you can use ma_sound_is_playing(). To check if a sound is at the end, use ma_sound_at_end(). Looping of a sound can be controlled with ma_sound_set_looping(). Use ma_sound_is_looping() to check whether or not the sound is looping.