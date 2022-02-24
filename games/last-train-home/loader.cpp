#include "core.h"
#include "sprite.h"
#include "font.h"
#include "audio.h"

#include "stb/stb_image.h"
#include "stb/stb_ds.h"
#include "json/json.h"
#include "miniz/miniz.h"

#include <stdio.h>

typedef struct Zipped_File {
  char *path;
  int size;
  int index;
} Zipped_File;

static mz_zip_archive zip_;
static bool use_zip_;
static Zipped_File *zip_files_;

void init_loader(void) {
  mz_zip_archive *zip = &zip_;
  if (!mz_zip_reader_init_file(zip, "data.bin", 0)) {
    printf("No data archive. Using files directly.\n");
    return;
  }

  printf("Data archive found.\n");
  use_zip_ = true;
  int num_files_and_dirs = mz_zip_reader_get_num_files(zip);

  int num_files = 0;
  for (int i = 0; i < num_files_and_dirs; i++) {
    if (mz_zip_reader_is_file_a_directory(zip, i))
      continue;

    int path_length = mz_zip_reader_get_filename(zip, i, NULL, 0);
    char *buf = (char *)malloc(path_length + 1);
    mz_zip_reader_get_filename(zip, i, buf, path_length+1);
    for (char *c = buf; *c; c++)
      if (*c == '\\')
        *c = '/';

    mz_zip_archive_file_stat stats = {0};
    mz_zip_reader_file_stat(zip, i, &stats);

    Zipped_File file = {0};
    file.path = buf;
    file.index = i;
    file.size = stats.m_uncomp_size;
    arrpush(zip_files_, file);
  }
}

static char *load_zipped_file(const char *path, int *size) {
  for (int i = 0; i < arrsize(zip_files_); i++) {
    Zipped_File *file = &zip_files_[i];
    if (str_eq(path, file->path)) {

      char *buffer = (char *)malloc(file->size + 1);
      mz_zip_reader_extract_to_mem(&zip_, file->index, buffer, file->size, 0);
      buffer[file->size] = 0;

      *size = file->size;
      return buffer;
    }
  }

  return NULL;
}

char *load_file(const char *path, int *size) {
  if (use_zip_) {
    return load_zipped_file(path, size);
  }

  FILE *f = fopen(path, "rb");
  if (!f) {
    return 0;
  }

  fseek(f, 0, SEEK_END);
  size_t tell_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buffer = (char *)malloc(tell_size+1);
  fread(buffer, 1, tell_size, f);
  buffer[tell_size] = 0;

  if (size)
    *size = (int)tell_size;

  fclose(f);
  return buffer;
}

struct Texture *load_texture(const char *path) {
  if (!str_ends_with(path, ".png"))
    return NULL;

  int size;
  char *data = load_file(path, &size);
  if (!data)
    return NULL;

  Texture *texture = NULL;
  int w, h, comps;
  void *pixels = stbi_load_from_memory((unsigned char *)data, size, &w, &h, &comps, 4);
  if (pixels) {
    texture = gfx_make_texture(pixels, w, h);
    free(pixels);
  }

  free(data);
  return texture;
}

Sprite *load_sprite(const char *path) {
  if (!str_ends_with(path, ".json"))
    return NULL;

  Sprite *sprite = NULL;
  int size = 0;

  char *data = load_file(path, &size);
  if (!data) {
    printf("Cannot load '%s'.\n", path);
    return NULL;
  }

  struct json_value_s *json = json_parse(data, size);
  if (json) {
    sprite = (Sprite *)calloc(1, sizeof(Sprite));

    struct json_object_s *json_obj = json_as_object(json);
    struct json_object_s *meta = json_as_object(json_get(json_obj, "meta"));
    struct json_array_s *tags = json_as_array(json_get(meta, "frameTags"));
    struct json_array_s *frames = json_as_array(json_get(json_obj, "frames"));

    for (struct json_array_element_s *it = frames->start;
        it; it = it->next)
    {
      struct json_object_s *f = json_as_object(it->value);
      struct json_object_s *frame = json_as_object(json_get(f, "frame"));
      float x = (float)json_as_number(json_get(frame, "x"));
      float y = (float)json_as_number(json_get(frame, "y"));
      int w = (int)json_as_number(json_get(frame, "w"));
      int h = (int)json_as_number(json_get(frame, "h"));
      float duration = (float)json_as_number(json_get(f, "duration")) * 0.001f;

      sprite->width = w;
      sprite->height = h;

      float quad[4] = {x,y, (float)w, (float)h};
      sprite_add_frame(sprite, quad, duration);
    }

    for (struct json_array_element_s *it = tags->start;
        it; it = it->next)
    {
      struct json_object_s *tag = json_as_object(it->value);

      const char *name = json_as_string(json_get(tag, "name"));
      int from = (int)json_as_number(json_get(tag, "from"));
      int to = (int)json_as_number(json_get(tag, "to"));

      sprite_add_anim(sprite, name, from, to);
    }

    sprite_add_anim(sprite, "<all-frames>", 0, sprite_num_frames(sprite)-1);
  }
  else {
    printf("Failed to parse json in '%s'.\n", path);
  }

  free(json);
  free(data);

  return sprite;
}

struct Font *load_image_font(const char *path, const char *characters, int spacing) {
  if (!str_ends_with(path, ".png")) {
    printf("Cannot load image font '%s'. File must be .png\n", path);
    return NULL;
  }

  Font *font = NULL;
  int size;
  char *data = load_file(path, &size);
  if (data) {
    unsigned char *pixels = NULL;
    int w, h, comp;

    pixels = stbi_load_from_memory((unsigned char *)data, size, &w, &h, &comp, 4);
    if (!pixels) {
      printf("Cannot read image '%s'.\n", path);
      return NULL;
    }
    free(data);
    font = (Font *)calloc(1, sizeof(Font));
    font_load_image(font, pixels, w, h, characters, spacing);
    free(pixels);
  }
  else {
    printf("Could not load file '%s'\n", path);
    return NULL;
  }

  return font;
}

struct Audio_Source *load_audio_source(const char *path) {
  if (!str_ends_with(path, ".ogg") && !str_ends_with(path, ".wav")) {
    printf("Cannot load audio '%s'. File must be .ogg\n", path);
    return 0;
  }

  int size = 0;
  char *buf = load_file(path, &size);
  if (!buf)
    return 0;

  Audio_Source *src = aud_make_source(buf, size);

  free(buf);
  return src;
}

struct Audio_Buffer *load_audio_buffer(const char *path) {
  if (!str_ends_with(path, ".ogg") && !str_ends_with(path, ".wav")) {
    printf("Cannot load audio '%s'. File must be .ogg or .wav\n", path);
    return 0;
  }

  int size = 0;
  char *buf = load_file(path, &size);
  if (!buf)
    return 0;

  Audio_Buffer *b = aud_make_buffer(buf, size);

  free(buf);
  return b;
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "json/json.c"

#include "miniz/miniz.c"

