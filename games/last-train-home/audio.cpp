#include "audio.h"
#include "core.h"
#define STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"

#define AL_ALEXT_PROTOTYPES
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/efx.h"
#include "AL/alext.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define PAGE_CAPACITY (4096*2)
#define AUDIO_NEW(T) ((T*)calloc(sizeof(T), 1))

ALCdevice *device_;
ALCcontext *context_;

typedef struct {
  unsigned ChunkID          : 32;
  unsigned ChunkSize        : 32;
  unsigned Format           : 32;
} WAVE_RIFF;

typedef struct {
  unsigned Subchunk1ID      : 32;
  unsigned Subchunk1Size    : 32;
  unsigned AudioFormat      : 16;
  unsigned NumChannels      : 16;
  unsigned SampleRate       : 32;
  unsigned ByteRate         : 32;
  unsigned BlockAlign       : 16;
  unsigned BitsPerSample    : 16;
  // unsigned ExtraParamSize   : 16;
  // char     ExtraParams[0];
} WAVE_FORMAT;

typedef struct {
  unsigned Subchunk2ID      : 32;
  unsigned Subchunk2Size    : 32;
} WAVE_DATA;

static
int TestMagic(unsigned magic, const char str[4]) {
  char c[4];
  memcpy(c, &magic, 4);
  return
    c[0] == str[0] &&
    c[1] == str[1] &&
    c[2] == str[2] &&
    c[3] == str[3];
}

typedef struct {
  WAVE_RIFF   riff;
  WAVE_FORMAT format;
  WAVE_DATA   data;
  const char *pcm_data;
  unsigned pcm_data_size;
} WAVE_;

typedef struct {
  unsigned ckID : 32;
  unsigned ckSize : 32;
} broadcast_audio_extension;

static
int ReadWav_(const void *buffer, int size, WAVE_ *wav) {
  const char *cbuffer = (const char *)buffer;
  memcpy(&wav->riff, cbuffer, sizeof(wav->riff));
  cbuffer += sizeof(WAVE_RIFF);
  if (!TestMagic(wav->riff.ChunkID, "RIFF")) {
    return 0;
  }

  memcpy(&wav->format, cbuffer, sizeof(wav->format));
  if (!TestMagic(wav->format.Subchunk1ID, "fmt ")) {
    return 0;
  }
  cbuffer += sizeof(WAVE_FORMAT);
  if (wav->format.AudioFormat != 1) {
    return 0;
  }

  while (cbuffer + sizeof(WAVE_DATA) <= (char*)buffer + size) {
    WAVE_DATA data_or_subchunk = { 0 };
    memcpy(&data_or_subchunk, cbuffer, sizeof(data_or_subchunk));
    if (TestMagic(data_or_subchunk.Subchunk2ID, "data")) {
      wav->data = data_or_subchunk;
      wav->pcm_data = cbuffer + sizeof(data_or_subchunk);
      wav->pcm_data_size = data_or_subchunk.Subchunk2Size;
      return 1;
    }
    else if (TestMagic(data_or_subchunk.Subchunk2ID, "bext")) {
      cbuffer += data_or_subchunk.Subchunk2Size + sizeof(data_or_subchunk);
    }
    else {
      return 0;
    }
  }

  return 1;
}

static void al_log_errors() {
  ALenum err = alGetError();
  if (err == AL_NO_ERROR) return;

  switch (err) {
    case AL_INVALID_OPERATION:
      printf("AL_INVALID_OPERATION\n");
      break;
    case AL_INVALID_ENUM:
      printf("AL_INVALID_ENUM\n");
      break;
    case AL_INVALID_VALUE:
      printf("AL_INVALID_VALUE\n");
      break;
    case AL_INVALID_NAME:
      printf("AL_INVALID_NAME\n");
      break;
  }
}

static ALenum get_al_format(int channels, int bits_per_sample) {
  if (bits_per_sample == 16) {
    if (channels > 1)
      return AL_FORMAT_STEREO16;
    else if (channels == 1)
      return AL_FORMAT_MONO16;
  }
  else if (bits_per_sample == 8) {
    if (channels > 1)
      return AL_FORMAT_STEREO8;
    else if (channels == 1)
      return AL_FORMAT_MONO8;
  }
  return AL_INVALID;
}

static Audio_Buffer *make_buffer_from_vorbis(const void *data, int size) {
  int error = 0;
  stb_vorbis *decoder = stb_vorbis_open_memory((unsigned char *)data, size, &error, NULL);
  if (!decoder)
    return 0;

	stb_vorbis_info decoder_info = stb_vorbis_get_info(decoder);

  int channels = decoder_info.channels;
  int sample_rate = decoder_info.sample_rate;
  int total_samples = stb_vorbis_stream_length_in_samples(decoder);
  int bits_per_sample = sizeof(int16_t) * 8;

  ALenum al_format = get_al_format(channels, bits_per_sample);
  if (al_format == AL_INVALID)
    return 0;

  int16_t *samples = (int16_t *)malloc(total_samples * channels * sizeof(int16_t));

  const int page_capacity = PAGE_CAPACITY / channels;

  int num_samples = 0;

  int total_pages = total_samples / page_capacity + ((total_samples % page_capacity) ? 1 : 0);
  for (int i = 0; i < total_pages; i++) {
    int16_t page[PAGE_CAPACITY];
    stb_vorbis_seek(decoder, i * page_capacity);

    int page_size = 0;
    while (page_size < page_capacity) {

      const int samples_to_read = page_capacity - page_size;

      int n = stb_vorbis_get_samples_short_interleaved(
          decoder, channels,
          page + page_size*channels,
          samples_to_read*channels);

      if (n == 0)
        break;

      page_size += n;
    }

    {
      memcpy(
        samples + num_samples*channels,
        page,
        page_size * channels * sizeof(int16_t));

      num_samples += page_size;
    }
  }

  ALuint handle = 0;
  alGenBuffers(1, &handle);
  alBufferData(handle, al_format, samples, channels * num_samples * (bits_per_sample / 8), sample_rate);

  free(samples);
  stb_vorbis_close(decoder);

  Audio_Buffer *buffer = AUDIO_NEW(Audio_Buffer);
  buffer->handle = (size_t)handle;
  buffer->ref_count++;
  return buffer;
}

Audio_Buffer *aud_make_buffer(const void *data, int size) {
  WAVE_ wav = {0};
  if (!ReadWav_(data, size, &wav))
    return make_buffer_from_vorbis(data, size);

  int channels = wav.format.NumChannels;
  int bits_per_sample = wav.format.BitsPerSample;
  int sample_rate = wav.format.SampleRate;
  int data_size_in_bytes = wav.data.Subchunk2Size;
  const void *samples = wav.pcm_data;

  ALenum al_format = get_al_format(channels, bits_per_sample);
  if (al_format == AL_INVALID)
    return 0;

  ALuint handle = 0;
  alGenBuffers(1, &handle);
  alBufferData(handle, al_format, samples, data_size_in_bytes, sample_rate);

  Audio_Buffer *buffer = AUDIO_NEW(Audio_Buffer);
  buffer->handle = handle;
  buffer->ref_count++;
  return buffer;
}

void aud_load(void) {
  device_ = alcOpenDevice(NULL);
  context_ = alcCreateContext(device_, NULL);
  alcMakeContextCurrent(context_);
  al_log_errors();
}

void aud_set_source_looped(Audio_Source *src, bool looped) {
  alSourcei((ALuint)src->handle, AL_LOOPING, looped ? 1 : 0);
}

void aud_play(Audio_Source *source) {
  ALint state = 0;
  alGetSourcei((ALuint)source->handle, AL_SOURCE_STATE, &state);
  if (state != AL_PLAYING) {
    alSourcePlay((ALuint)source->handle);
  }
}

void set_master_volume(float volume) {
  if (volume > 1) volume = 1;
  if (volume < 0) volume = 0;
  alListenerf(AL_GAIN, volume);
}

float get_master_volume(void) {
  float volume = 0;
  alGetListenerf(AL_GAIN, &volume);
  return volume;
}

void aud_delete_source(Audio_Source *source) {
  for (int i = 0; i < _countof(source->buffers); i++) {
    Audio_Buffer *buf = &source->buffers[i];
    if (buf->handle) {
      ALuint buf_handle = (ALuint)buf->handle;
      alDeleteBuffers(1, &buf_handle);
    }
  }

  if (source->handle) {
    ALuint handle = (ALuint)source->handle;
    alDeleteSources(1, &handle);
  }

  if (source->buffer) {
    aud_release_buffer(source->buffer);
  }

  free(source);
}


void aud_release_buffer(Audio_Buffer *buffer) {
  assert(buffer->ref_count && "Buffer already destroyed.");
  buffer->ref_count--;
  if (buffer->ref_count <= 0) {
    ALuint handle = (ALuint)buffer->handle;
    alDeleteBuffers(1, &handle);
    free(buffer);
  }
}

Audio_Source *aud_make_source(const void *data, size_t size) {
  Audio_Buffer *buf = aud_make_buffer(data, (int)size);
  if (!buf) return 0;

  Audio_Source *src = aud_make_source_from_buffer(buf);
  aud_release_buffer(buf);
  return src;
}

Audio_Source *aud_make_source_from_buffer(Audio_Buffer *buffer) {
  ALuint handle = 0;
  alGenSources(1, &handle);
  alSourcef(handle, AL_PITCH, 1);
  alSourcef(handle, AL_GAIN, 1);
  assert(buffer && buffer->handle && "Buffer needs to be real");
  alSourcei(handle, AL_BUFFER, (ALuint)buffer->handle);
  Audio_Source *source = AUDIO_NEW(Audio_Source);
  source->buffer = buffer;
  source->fully_loaded = true;
  source->handle = (size_t)handle;
  buffer->ref_count++;
  return source;
}

bool aud_is_playing(Audio_Source *src) {
  ALint state = 0;
  alGetSourcei((ALuint)src->handle, AL_SOURCE_STATE, &state);
  return state == AL_PLAYING;
}

void aud_set_volume(Audio_Source *src, float volume) {
  alSourcef((ALuint)src->handle, AL_GAIN, volume);
}

#undef STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"


