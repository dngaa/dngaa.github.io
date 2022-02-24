#include "sprite.h"
#include "stb/stb_ds.h"

#include <assert.h>
#include <stdio.h>

void sprite_clear(Sprite *sprite) {
  for (int i = 0; i < arrlen(sprite->anims); i++)
    free(sprite->anims[i].name);
  arrfree(sprite->frames);
  arrfree(sprite->anims);
  *sprite = {};
}

static Anim *sprite_find_anim
  (Sprite *s, const char *name)
{
  for (int i = 0; i < arrlen(s->anims); i++)
    if (!strcmp(s->anims[i].name, name))
      return &s->anims[i];
  return NULL;
}

static Frame *get_frame(Sprite *s, int frame) {
  assert(frame < arrlen(s->frames));
  return &s->frames[frame];
}
void sprite_get_quad(Sprite *sprite, float quad[4]) {
  Frame *f = get_frame(sprite, sprite->frame);
  memcpy(quad, f->quad, sizeof(quad[0])*4);
}

void sprite_play(Sprite *sprite, const char *name, unsigned flags) {
  Anim *anim = sprite_find_anim(sprite, name);
  if (!anim) {
    printf("No animation '%s'.\n", name);
    return;
  }

  if (sprite->cur_anim == anim && sprite->flags == flags) {
    return;
  }

  sprite->flags = flags;
  if (sprite->cur_anim != anim) {
    sprite->rate = 1.f;
    sprite->cur_anim = anim;
    sprite->frame_time = 0;
    sprite->frame = anim->from;
    if (flags & SPRITE_REVERSE) {
      Frame *f = get_frame(sprite, anim->to);
      sprite->frame_time = f->duration;
      sprite->frame = anim->to;
    }
  }

  sprite->playback_speed = 1;
}

bool sprite_entered_anim(Sprite *sprite, const char *name) {
  Anim *anim = sprite_find_anim(sprite, name);
  if (anim) {
    return anim->from == sprite->frame && sprite->frame != sprite->last_frame;
  }
  return false;
}

void sprite_update(Sprite *sprite, float dt) {
  int cur_frame = sprite->frame;
  if (sprite->cur_anim) {
    if (!(sprite->flags & SPRITE_REVERSE)) {
      sprite->frame_time += dt * sprite->playback_speed * sprite->rate;
      Frame *f = get_frame(sprite, sprite->frame);
      if (sprite->frame_time >= f->duration) {
        sprite->frame_time = 0;
        if (sprite->frame >= sprite->cur_anim->to) {
          if (sprite->flags & SPRITE_LOOPED) {
            sprite->frame = sprite->cur_anim->from;
          }
          else {
            sprite->cur_anim = NULL;
          }
        }
        else {
          sprite->frame++;
        }
      }
    }
    // Reverse
    else {
      sprite->frame_time -= dt * sprite->playback_speed * sprite->rate;
      if (sprite->frame_time <= 0) {
        if (sprite->frame <= sprite->cur_anim->from) {
          if (sprite->flags & SPRITE_LOOPED) {
            sprite->frame = sprite->cur_anim->to;
            Frame *f = get_frame(sprite, sprite->frame);
            sprite->frame_time = f->duration;
          }
          else {
            sprite->cur_anim = NULL;
          }
        }
        else {
          sprite->frame--;
          Frame *f = get_frame(sprite, sprite->frame);
          sprite->frame_time = f->duration;
        }
      }
    }
  }
  sprite->last_frame = cur_frame;
}

void sprite_mirror(Sprite *from, Sprite *to, const char *name) {
  to->cur_anim = NULL;
  if (!from->cur_anim)
    return;

  int rel_frame = from->frame - from->cur_anim->from;
  Anim *anim = sprite_find_anim(to, name);
  if (!anim)
    return;

  int new_frame = anim->from + rel_frame;
  if (new_frame < arrlen(to->frames))
    to->frame = new_frame;
}

bool sprite_is_playing(Sprite *sprite, const char *name) {
  return sprite->cur_anim && !strcmp(sprite->cur_anim->name, name);
}

void sprite_add_frame(Sprite *sprite, float quad[4], float duration) {
  Frame f = {0};
  f.quad[0] = quad[0];
  f.quad[1] = quad[1];
  f.quad[2] = quad[2];
  f.quad[3] = quad[3];
  f.duration = duration;
  arrpush(sprite->frames, f);
}

void sprite_add_anim(Sprite *sprite, const char *name, int from, int to) {
  Anim anim = {0};
  anim.name = (char *)malloc(strlen(name)+1);
  memcpy(anim.name, name, strlen(name)+1);
  anim.from = from;
  anim.to = to;
  for (int i = from; i <= to && arrlen(sprite->frames)-1; i++)
    anim.duration += sprite->frames[i].duration;
  arrpush(sprite->anims, anim);
}

int sprite_num_frames(Sprite *sprite) {
  return arrlen(sprite->frames);
}

