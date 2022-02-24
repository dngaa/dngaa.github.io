#pragma once

#include <stdbool.h>

enum {
  SPRITE_REVERSE = 1 << 0,
  SPRITE_LOOPED = 1 << 1,
};

typedef struct Frame {
  float quad[4];
  float duration;
} Frame;

typedef struct Anim {
  char *name;
  int from;
  int to;
  float duration;
} Anim;

typedef struct Sprite {
  Frame *frames;
  Anim *anims;
  Anim *cur_anim;

  int width;
  int height;
  int last_frame;
  int frame;
  float frame_time;
  float playback_speed;
  unsigned flags;
  int offset_x;
  int offset_y;
  float rate;
} Sprite;

void sprite_clear(Sprite *sprite);
void sprite_get_quad(Sprite *sprite, float quad[4]);
void sprite_play(Sprite *sprite, const char *name, unsigned flags);
void sprite_update(Sprite *sprite, float dt);
void sprite_mirror(Sprite *from, Sprite *to, const char *name);
bool sprite_is_playing(Sprite *sprite, const char *name);
void sprite_add_frame(Sprite *sprite, float quad[4], float duration);
void sprite_add_anim(Sprite *sprite, const char *name, int from, int to);
int sprite_num_frames(Sprite *sprite);
bool sprite_entered_anim(Sprite *sprite, const char *name);

