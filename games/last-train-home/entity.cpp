#include "game.h"

void entity_free(Entity *en) {
  if (en->texture) {
    gfx_delete_texture(en->texture);
    en->texture = 0;
  }

  if (en->sprite) {
    sprite_clear(en->sprite);
    free(en->sprite);
    en->sprite = 0;
  }
}
void entity_draw(Entity *en) {
  gfx_set_color(en->color.x,en->color.y,en->color.z,en->color.w);
  call_entity_proc(en, ACTION_DRAW);
  if (en->texture) {
    gfx_push();
    gfx_translate(floorf(en->x), floorf(en->y));

    Texture *t = en->texture;
    float q[4] = {0.f, 0.f, (float)t->width, (float)t->height};

    if (en->sprite) {
      sprite_get_quad(en->sprite, q);
    }

    gfx_translate(floorf(en->width/2 - q[2]/2), floorf(en->height - q[3]));


    if (en->facing == FACING_LEFT) {
      gfx_translate(floorf(q[2]/2), 0);
      gfx_scale(-1, 1);
      gfx_translate(-floorf(q[2]/2), 0);
    }

    gfx_draw_textureq(en->texture, q);

    gfx_pop();
  }
}

void entity_update(Entity *en) {
  if (en->suspended)
    return;
  call_entity_proc(en, ACTION_UPDATE);
  if (en->sprite) {
    sprite_update(en->sprite, app_delta());
  }
}

void entity_set_center_x(Entity *en, float x) {
  en->x = x - en->width/2;
}
