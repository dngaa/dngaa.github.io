#include "game.h" 

void player_update(Entity *en) {
  float speed = 60;

  const char *walk_anim = "walk";
  const char *idle_anim = "idle";
  if (flashlight_on) {
     walk_anim = "walk_lighter";
     idle_anim = "idle_lighter";
  }

  if (!global_state && app_keydown(KEY_LEFT)){
    en->x -= speed * app_delta();
    en->facing = FACING_LEFT;
    sprite_play(en->sprite, walk_anim, SPRITE_LOOPED);
  }
  else if (!global_state && app_keydown(KEY_RIGHT)) {
    en->x += speed * app_delta();
    en->facing = FACING_RIGHT;
    sprite_play(en->sprite, walk_anim, SPRITE_LOOPED);
  }
  else {
    sprite_play(en->sprite, idle_anim, SPRITE_LOOPED);
  }
}

void player_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->width = 26;
    en->height = 40;

    en->sprite = load_sprite("data/player.json");
    en->texture = load_texture("data/player.png");

    en->y = FLOOR_Y - en->height;
  }

  else if (action == ACTION_UPDATE) {
    en->y = FLOOR_Y - en->height;
    player_update(en);
  }

  else if (action == ACTION_DRAW) {
    if (en->sprite) {
      if (sprite_entered_anim(en->sprite, "step:0")) {
        play_footstep();
      }
      else if (sprite_entered_anim(en->sprite, "step:1")) {
        play_footstep();
      }
      else if (sprite_entered_anim(en->sprite, "step:2")) {
        play_footstep();
      }
      else if (sprite_entered_anim(en->sprite, "step:3")) {
        play_footstep();
      }
    }
  }
}

void train_bg_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->texture = load_texture("data/train.png");
    en->width = en->texture->width;
    en->height = en->texture->height;
  }
}


void newspaper_guy_talk(Entity *en) {
  set_state(IN_NEWSPAPER_GUY_SAY);
}
void newspaper_guy_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->width = 30;
    en->height = 60;
    en->texture = load_texture("data/newspaper_guy.png");
    en->sprite = load_sprite("data/newspaper_guy.json");
    en->height = 40;

    en->on_use = newspaper_guy_talk;
  }

  if (action == ACTION_UPDATE) {
    en->y = FLOOR_Y - en->height;
  }
}

void vomit_girl_talk(Entity *en) {
  set_state(IN_VOMIT_GIRL_TALK);
}

void vomit_girl_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->texture = load_texture("data/vomit_girl.png");
    en->sprite = load_sprite("data/vomit_girl.json");
    en->width = 30;
    en->height = 40;
    en->facing = FACING_LEFT;

    en->on_use = vomit_girl_talk;
  }
  else if (action == ACTION_UPDATE) {

    switch (en->state) {
      case 0:
        sprite_play(en->sprite, "idle", SPRITE_LOOPED);
        break;

      case 20:
        sprite_play(en->sprite, "walk", SPRITE_LOOPED);
        en->x -= 60.f * app_delta();

        if (en->x <= GAP_LEFT) {
          en->removed = true;
        }
        break;
    }
  }
}

void homeless_guy_talk(Entity *en) {
  set_state(IN_HOMELESS_GUY_TALK);
}

void homeless_guy_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->texture = load_texture("data/homeless_guy.png");
    en->sprite = load_sprite("data/homeless_guy.json");
    en->width = 40;
    en->height = 30;

    en->on_use = homeless_guy_talk;
  }
}

void smoker_talk(Entity *en) {
  set_state(IN_SMOKER_TALK);
}
void smoker_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->texture = load_texture("data/smoker.png");
    en->sprite = load_sprite("data/smoker.json");
    en->width = 26;
    en->height = 50;

    en->on_use = smoker_talk;
  }

  else if (action == ACTION_UPDATE) {
    en->y = FLOOR_Y - en->height;
  }
}

void little_girl_talk(Entity *en) {
  set_state(IN_LITTGIRL_SAY);
}

void little_girl_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->texture = load_texture("data/little_girl.png");
    en->sprite = load_sprite("data/little_girl.json");
    en->width = 26;
    en->height = 40;

    en->on_use = little_girl_talk;
    en->state = 10;
  }

  else if (action == ACTION_UPDATE) {
    en->y = FLOOR_Y - en->height;
    float speed = 15;
    switch (en->state) {
      case 10: 
        {
          en->x -= speed * app_delta();
          sprite_play(en->sprite, "running", SPRITE_LOOPED);
          en->facing = FACING_LEFT;
          if (en->x < 90) {
            en->x = 90;
            en->state ++;
          }
        }
        break;

      case 11:
        en->state ++;
        break;
      case 12:
        en->state ++;
        break;
      case 13:
        en->state ++;
        break;
      case 14:
        en->state ++;
        break;
      case 15:
        {
          en->x += speed * app_delta();
          en->facing = FACING_RIGHT;
          sprite_play(en->sprite, "running", SPRITE_LOOPED);
          if (en->x > 253) {
            en->x = 253;
            en->state = 10;
          }
        }
        break;
    }
  }
}

void evil_little_girl_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->texture = load_texture("data/little_girl.png");
    en->sprite = load_sprite("data/little_girl.json");
    en->width = 26;
    en->height = 40;
  }

  else if (action == ACTION_UPDATE) {
    en->facing = FACING_LEFT;
    sprite_play(en->sprite, "shadow_running", 0);
    
    en->y = FLOOR_Y - en->height - 15;
  }
}

void light_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->light_radius = 50;
    en->color = { 1.f, 1.f, 1.f, 1.f };
  }
  else if (action == ACTION_UPDATE) {
    if (en->is_flashlight) {
      en->light_radius = 40 + get_wave(0.5) * 2;

      Entity *player = world_find_first(world, ENTITY_PLAYER);
      if (player) {
        float x = player->x + player->width / 2;
        float y = player->y + player->height / 2;
        
        if (player->facing == FACING_LEFT ) {
          x -= 13;
        }
        else {
          x += 13;
        }

        en->x = x - en->width / 2;
        en->y = y - en->height / 2;
      }

      en->color.data[0] = 143.f / 255.f;
      en->color.data[1] = 123.f / 255.f;
      en->color.data[2] = 84.f / 255.f;

      if (flashlight_on && player) {
        en->color.w = 1;
      }
      else {
        en->color.w = 0;
      }
    }
  }
}

void timer_proc(Entity *en, int action) {
  if (action == ACTION_UPDATE) {
    if (!global_state) {
      en->timer_left -= app_delta();
      if (en->timer_left <= 0) {
        if (en->on_over)
          en->on_over(en);
        en->removed = true;
      }
    }
  }
}

void rail_handle_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->sprite = load_sprite("data/rail_handle.json");
    en->texture = load_texture("data/rail_handle.png");
    if (en->sprite) {
      en->width = en->sprite->width;
      en->height = en->sprite->height;
    }
  }
}

void spider_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->sprite = load_sprite("data/spider.json");
    en->texture = load_texture("data/spider.png");
    if (en->sprite) {
      en->width = en->sprite->width;
      en->height = en->sprite->height;
    }
  }

  else if (action == ACTION_UPDATE) {
    sprite_play(en->sprite, "walk", SPRITE_LOOPED);
    en->y = FLOOR_Y - en->height - 10;
  }
}

void train_door_proc(Entity *en, int action) {
  if (action == ACTION_MAKE) {
    en->sprite = load_sprite("data/door.json");
    en->texture = load_texture("data/door.png");
    if (en->sprite) {
      en->width = en->sprite->width;
      en->height = en->sprite->height;
    }
  }
}

void call_entity_proc(Entity *en, int action) {
  switch (en->type) {
    case ENTITY_PLAYER:
      player_proc(en, action);
      break;
    case ENTITY_TRAIN_BG:
      train_bg_proc(en, action);
      break;
    case ENTITY_NEWSPAPER_GUY:
      newspaper_guy_proc(en, action);
      break;
    case ENTITY_VOMIT_GIRL:
      vomit_girl_proc(en, action);
      break;
    case ENTITY_HOMELESS_GUY:
      homeless_guy_proc(en, action);
      break;
    case ENTITY_SMOKER:
      smoker_proc(en, action);
      break;
    case ENTITY_LITTLE_GIRL:
      little_girl_proc(en, action);
      break;
    case ENTITY_LIGHT:
      light_proc(en, action);
      break;
    case ENTITY_TIMER:
      timer_proc(en, action);
      break;

    case ENTITY_RAIL_HANDLE:
      rail_handle_proc(en, action);
      break;

    case ENTITY_EVIL_LITTLE_GIRL:
      evil_little_girl_proc(en, action);
      break;

    case ENTITY_SPIDER:
      spider_proc(en, action);
      break;

    case ENTITY_TRAIN_DOOR:
      train_door_proc(en, action);
      break;

  }
}


