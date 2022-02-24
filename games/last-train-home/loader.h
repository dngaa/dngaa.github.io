#pragma once

void init_loader(void);
char *load_file(const char *path, int *size);
struct Texture *load_texture(const char *path);
struct Sprite *load_sprite(const char *path);
struct Font *load_image_font(const char *path, const char *characters, int spacing);
struct Audio_Source *load_audio_source(const char *path);
struct Audio_Buffer *load_audio_buffer(const char *path);

