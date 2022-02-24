#include "core.h"
#include "loader.h"
#include "game.h"
#include "audio.h"

#define FIRST_SCENE scene_train_1
  //change_scene(scene_train_1);
// #define FIRST_SCENE scene_train_final
  //change_scene(scene_train_home);

void scene_train_home(World *world);
void scene_train_final(World *world);

void goto_left_scene(Entity *en);
void goto_right_scene(Entity *en);

typedef struct Sound {
  const char *name;
  Audio_Buffer *buffer;
  Audio_Source **sources;
} Sound;

float scene_change_fadein_wait_override;
float scene_change_fadein_duration_override;

Music *music_list;

Sound *sounds;

bool request_reset;

bool fading_ambient;
float ambient_fade_target = 1;
float ambient_fade_duration;
float ambient_current_volume = 0;

bool showing_ending_text;
float ending_text_alpha;

World *world;
Canvas *canvas;
Canvas *world_canvas;
float shake_x;

float next_window_flash_timer;
float window_flash;
bool show_window_flash;

float timer_duration;
Curtain curtain;
bool flashlight_on;

bool should_shake_camera;
bool first_shke;
Vec2 camera_off;
float shake_timer;
float total_magnitude;
float impulse_timer;
float impulse_gap;

int active_id;
Texture *prompt_texture;
void (*prompt_proc)(Entity *en);
float prompt_x;
float prompt_y;

int global_state;
int last_global_state;

void (*requested_scene)(World *world);

float speech_override_timer;
int speech_progress;
float speech_char_timer;
char *speech_content;
float speech_x;
float speech_y;
char **speech_choices;
int speech_choice_index;
Font *speech_font;

Font *big_font;

int wait_for_type_removed;

int play_animation_type;
const char *play_animation;


Item *items;
Notification *notifications;


// game flags
Flags flags;

bool world_is_dark(World *world) {
  Vec3 a = world->ambient;
  return sqrtf(a.x * a.x + a.y*a.y + a.z*a.z) < 0.5;
}

void clear_items() {
  for (int i = 0; i < arrlen(items); i++) {
    items[i].quantity = 0;
  }
}

float randomf(void) {
  return (float)((double)rand() / (double)RAND_MAX);
}
float randomf_range (float low, float high) {
  float t = randomf();
  return low + (high - low) * t;
}
unsigned randomu() {
  return rand();
}

float get_wave(float period) {
  return sin(app_time() * 3.1415926 * 2 / period) * 0.5f + 0.5f;
}
float get_wave_range(float period, float low, float high) {
  return low + get_wave(period) * (high - low);
}

void update_music(void) {
  for (int i = 0; i < arrlen(music_list);) {
    if (music_list[i].fade_out) {
      music_list[i].volume -= app_delta() * 1;
      aud_set_volume(music_list[i].source, music_list[i].volume);
    }

    if (music_list[i].volume <= 0) {
      aud_delete_source(music_list[i].source);
      arrdel(music_list, i);
    }
    else {
      i++;
    }
  }
}

void fade_out_all_music() {
  for (int i = 0; i < arrlen(music_list); i++) {
    music_list[i].fade_out = true;
  }
}

bool is_music_playing(const char *path) {
  for (int i = 0; i < arrlen(music_list); i++) {
    Music *m = &music_list[i];
    if (str_eq(m->path, path))
      return true;
  }
  return false;
}
void play_music(const char *path) {
  if (is_music_playing(path))
    return;

  Audio_Source *s = load_audio_source(path);
  if (s) {
    Music m = {0};
    aud_play(s);
    m.source = s;
    m.path = path;
    m.volume = 1;
    aud_set_source_looped(s, true);
    arrput(music_list, m);
  }
}

Item *get_item(int type) {
  for (int i = 0; i < arrlen(items); i++) {
    if (items[i].type == type)
      return &items[i];
  }

  Item I = {0};
  I.type = type;
  switch (I.type) {
    case ITEM_NEWSPAPER:
      I.name = "Newspaper";
      I.texture = load_texture("data/item_newspaper.png");
      break;
    case ITEM_LIGHTER:
      I.name = "Lighter";
      I.texture = load_texture("data/item_lighter.png");
      break;
    case ITEM_PILL_BOTTLE:
      I.name = "Pill Bottle";
      I.texture = load_texture("data/item_medicine.png");
      break;
    default:
      I.name = "Unknown";
      break;
  }
  arrput(items, I);
  return &arrback(items);
}
void take_away_item(int type) {
  Item *I = get_item(type);
  if (I->quantity > 0)
    I->quantity--;
}
void give_item(int type) {
  Item *I = get_item(type);
  I->quantity++;
  char *msg = mprintf("Got '%s'", I->name);
  notify(msg); free(msg);
}

bool has_item(int type) {
  return get_item(type)->quantity > 0;
}

void draw_items(void) {
  float x = 5;
  float y = 5;
  float size = 22;
  gfx_set_color(1,1,1,1);
  for (int i = 0; i < arrlen(items); i++) {
    Item *I = &items[i];
    if (!I->quantity)
      continue;
    if (I->texture) {
      Texture *t = I->texture;
      gfx_push();
      gfx_translate(floorf(x+size/2 - t->width/2), floorf(y + size/2 - t->height/2));
      gfx_draw_texture(t);
      gfx_pop();
      x += size + 3;
    }
  }
}

void notify(const char *content) {
  Notification n = {0};
  n.content = str_copy(content);
  n.time_left = 3;
  n.time_total = 3;
  arrput(notifications, n);
}
void draw_notifications(void) {
  if (!speech_font)
    return;
  int y = 5;
  for (int i = 0; i < arrlen(notifications);) {
    Notification *n = &notifications[i];
    gfx_set_color(1,1,1, n->time_left/ n->time_total);
    int x = GAME_WIDTH/2 - font_width(speech_font, n->content)/2;
    draw_speech_box(n->content, x, y);
    y += font_height(speech_font) + 5;
    n->time_left -= app_delta();
    if (n->time_left < 0) {
      free(n->content);
      arrdel(notifications, i);
    }
    else {
      i++;
    }
  }
}

void fade_ambient_sound(float target, float duration) {
  ambient_fade_target = target;
  ambient_fade_duration = duration;
}

void update_ambient_sound() {
  move_to(&ambient_current_volume, ambient_fade_target, app_delta() / ambient_fade_duration);
  //printf("%f\n", ambient_current_volume);
  for (int i = 0; i < arrlen(world->ambient_audio_list); i++) {
    aud_set_volume(world->ambient_audio_list[i], ambient_current_volume);
  }
}

void play_sound(const char *path, float volume) {
  Sound *s = 0;
  for (int i = 0; i < arrlen(sounds); i++) {
    if (str_eq(sounds[i].name, path)) {
      s = &sounds[i];
      break;
    }
  }

  if (!s) {
    arrput(sounds, MAKE(Sound));
    s = &arrback(sounds);
    s->name = str_copy(path);
    s->buffer = load_audio_buffer(path);
  }

  if (!s->buffer)
    return;

  for (int i = 0; i < arrlen(s->sources); i++) {
    if (!aud_is_playing(s->sources[i])) {
      aud_set_volume(s->sources[i], volume);
      aud_play(s->sources[i]);
      return;
    }
  }

  Audio_Source *src = aud_make_source_from_buffer(s->buffer);
  if (src) {
    aud_set_volume(src, volume);
    aud_play(src);
    arrput(s->sources, src);
  }
}

static const char *foot_step_sounds[] = {
  "data/footstep_0.ogg",
  "data/footstep_1.ogg",
  "data/footstep_2.ogg",
  "data/footstep_3.ogg",
  "data/footstep_4.ogg",
};
static int footstep_cursor;
void play_footstep(void) {
  play_sound(foot_step_sounds[footstep_cursor++], 0.3);
  footstep_cursor = footstep_cursor % _countof(foot_step_sounds);
}

void move_to(float *x, float tx, float delta) {
  if (*x < tx) {
    *x += delta;
    if (*x >= tx) {
      *x = tx;
    }
  }
  else if (*x > tx) {
    *x -= delta;
    if (*x <= tx) {
      *x = tx;
    }
  }
}

void move_to_2f(Vec2 *v, Vec2 t, float delta) {
  Vec2 d = { t.x - v->x, t.y - v->y };
  if (size2f(d) < 0.1f) {
    *v = d;
  }
  else {
    float ang = atan2f(d.y, d.x);
    float delta_x = fabsf(cosf(ang)) * delta;
    float delta_y = fabsf(sinf(ang)) * delta;
    move_to(&v->x, t.x, delta_x);
    move_to(&v->y, t.y, delta_y);
  }
}

void show_ending_text() {
  showing_ending_text = true;
}

void draw_ending_text() {
  if (showing_ending_text) {
    move_to(&ending_text_alpha, 1, app_delta());
  }
  else {
    ending_text_alpha = 0;
  }

  float y = GAME_HEIGHT / 3 * 2;
  float x = 0;
  Font *font = speech_font;
  const char *text = "";
  int p = 3;
  float alpha = ending_text_alpha;

  if (alpha) {
    font = big_font;
    text = "The End.";
    x = GAME_WIDTH/2 - font_width(font, text)/2;
    gfx_set_color(0,0,0,1 * alpha);
    font_draw(font, text, x, y);
    y = y + font_height(font) + p;

    font = speech_font;
    text = "Thanks for playing.";
    gfx_set_color(0,0,0,0.5 * alpha);
    x = GAME_WIDTH/2 - font_width(font, text)/2;
    font_draw(font, text, x, y);
    y = y + font_height(font) + p;

    font = speech_font;
    text = "press [X] to play again.";
    gfx_set_color(0,0,0,0.5 * alpha);
    x = GAME_WIDTH/2 - font_width(font, text)/2;
    font_draw(font, text, x, y);
    y = y + font_height(font) + p;

    if (alpha == 1) {
      if (app_keypress(KEY_X)) {
        set_state(IN_FADE_OUT);
      }
    }
  }
}

void curtain_set(Curtain *c, float r, float g, float b, float a) {
  c->color[0] = r;
  c->color[1] = g;
  c->color[2] = b;
  c->color[3] = a;
  c->target[0] = r;
  c->target[1] = g;
  c->target[2] = b;
  c->target[3] = a;
}
void curtain_fade(Curtain *c, float r, float g, float b, float a, float time_left) {
  if (time_left == 0) {
    c->time_left = time_left;
    c->time_total = time_left;
    curtain_set(c, r,g,b,a);
    return;
  }
  c->start[0] = c->color[0];
  c->start[1] = c->color[1];
  c->start[2] = c->color[2];
  c->start[3] = c->color[3];
  c->target[0] = r;
  c->target[1] = g;
  c->target[2] = b;
  c->target[3] = a;
  c->time_left = time_left;
  c->time_total = time_left;
}
void curtain_draw(Curtain *c) {
  if (c->time_total) {
    c->time_left -= app_delta();
    if (c->time_left < 0) {
      c->time_left = 0;
      c->color[0] = c->target[0];
      c->color[1] = c->target[1];
      c->color[2] = c->target[2];
      c->color[3] = c->target[3];
    }
    else {
      float t = 1 - (c->time_left / c->time_total);
      for (int i = 0; i < 4; i++) {
        c->color[i] = c->start[i] +
          t * (c->target[i] - c->start[i]);
      }
    }
  }

  if (c->color[3]) {
    gfx_set_color(c->color[0], c->color[1], c->color[2], c->color[3]);
    gfx_fill_rect(-5,-5,GAME_WIDTH+10, GAME_HEIGHT+10);
  }
}

bool curtain_is_fading(Curtain *c) {
  return c->time_left > 0;
}

void show_speech_for(int type, const char *content) {
  Entity* en = world_find_first(world, type);
  if (!en) {
    return;
  }
  if (speech_content) free(speech_content);
  speech_x = floorf(en->x + en->width/2);
  speech_y = floorf(en->y - 10);
  speech_content = str_copy(content);
  speech_progress = 0;
  speech_char_timer = 0;
  if (speech_choices) {
    for (int i = 0; i < arrlen(speech_choices); i++)
      free(speech_choices[i]);
    arrfree(speech_choices);
  }
  speech_choices = 0;
  speech_choice_index = 0;
  speech_override_timer = 0;
}

void show_speech_for_and_prompt(int type, const char *content, const char **choices, int choices_count) {
  show_speech_for(type, content);
  for (int i = 0; i < choices_count; i++) {
    arrput(speech_choices, str_copy(choices[i]));
  }
}

void draw_speech_box(const char *content, int x, int y) {

  float color[4];
  gfx_get_color(color);

  float text_w = font_width(speech_font, content);
  float text_h = font_height(speech_font);

  float p = 5;
  float rx = x - p;
  float ry = y - p;
  float rw = text_w + p*2;
  float rh = text_h + p*2;

  gfx_set_color(0,0,0,0.7 * color[3]);
  gfx_fill_rect(rx-1,ry-1,rw+2,rh+2);
  gfx_set_color(1,1,1,0.8 * color[3]);
  gfx_trace_rect(rx,ry,rw,rh);

  gfx_set_color(1,1,1,0.8 * color[3]);
  font_draw(speech_font, content, x, y);

  gfx_set_color(color[0],color[1],color[2],color[3]);
}

void draw_speech(void) {
  if (!speech_content) {
    return;
  }


  if (!speech_font) {
    return;
  }

  if (speech_progress < strlen(speech_content)) {
    speech_char_timer -= app_delta();
    if (speech_char_timer < 0) {
      speech_progress++;
      speech_char_timer = 0.04f;
    }
    if (app_keypress(KEY_X)) {
      speech_progress = strlen(speech_content);
    }
  }
  else if (speech_choices) {
    if (app_keypress(KEY_X)) {
      free(speech_content);
      speech_content = NULL;
      return;
    }
    else if (app_keypress(KEY_DOWN)) {
      speech_choice_index ++;
      if (speech_choice_index >= arrlen(speech_choices)) {
        speech_choice_index = arrlen(speech_choices)-1;
      }
    }
    else if (app_keypress(KEY_UP)) {
      speech_choice_index --;
      if (speech_choice_index < 0) {
        speech_choice_index = 0;
      }
    }
  }
  else {

    bool done = false;
    if (app_keypress(KEY_X))
      done = true;
    else if (speech_override_timer > 0) {
      speech_override_timer -= app_delta();
      if (speech_override_timer <= 0) {
        done = true;
      }
    }

    if (done) {
      free(speech_content);
      speech_content = NULL;
      return;
    }
  }

  char c = speech_content[speech_progress];
  speech_content[speech_progress] = 0;

  float text_w = font_width(speech_font, speech_content);
  float text_h = font_height(speech_font);
  float x = floorf(speech_x - text_w/2);
  float y = floorf(speech_y - text_h) - 5;

  if (x + text_w + 5 > GAME_WIDTH) {
    x = GAME_WIDTH - (text_w + 5);
  }
  else if (x < 5) {
    x = 5;
  }

  float p = 5;
  float rx = x - p;
  float ry = y - p;
  float rw = text_w + p*2;
  float rh = text_h + p*2;

  draw_speech_box(speech_content, x, y);

  speech_content[speech_progress] = c;

  if (speech_choices && speech_progress == strlen(speech_content)) {
    float cx = speech_x - 10;
    float cy = y + p*2 + 2 + text_h;
    for (int i = 0; i < arrlen(speech_choices); i++) {
      gfx_set_color(1,1,1,1);
      draw_speech_box(speech_choices[i], cx, cy);

      if (speech_choice_index == i) {
        if (prompt_texture)
          draw_x(cx - prompt_texture->width/2-5, cy + text_h/2);
      }

      cy += p*2 + 2 + text_h;
    }
  }
}

bool is_showing_speech(void) {
  return !!speech_content;
}

bool state_changed(void) {
  return global_state != last_global_state;
}

void next_state(void) {
  global_state ++;
}

void clear_state_impl(void) {
  global_state = 0;
}
#define clear_state() { clear_state_impl(); return; }

void set_state(int state) {
  global_state = state;
}

void timer_wait(float duration) {
  timer_duration = duration;
}
void timer_update(void) {
  timer_duration -= app_delta();
  if (timer_duration <= 0) {
    timer_duration = 0;
  }
}

void suspend_entity_type(int type) {
  Entity *en = world_find_first(world, type);
  if (en)
    en->suspended = true;
}

void unsuspend_entity_type(int type) {
  Entity *en = world_find_first(world, type);
  if (en)
    en->suspended = false;
}

void play_animation_for_and_wait(int type, const char *anim) {
  Entity *en = world_find_first(world, type);
  if (en && en->sprite) {
    sprite_play(en->sprite, anim, 0);
    play_animation_type = type;
    play_animation = anim;
  }
}

void wait_for_entity_type_removed(int type) {
  wait_for_type_removed = type;
}

static void on_epilog_use(Entity *en) {
  set_state(IN_EPILOG);
}

void timer_over_open_door(Entity *en) {
  Entity *door = world_find_first(world, ENTITY_TRAIN_DOOR);
  if (door) {
    sprite_play(door->sprite, "open", 0);
  }


  Entity *o = world_add(world, 0);
  if (o) {
    o->height = 48;
    o->width = 10;
    o->y = FLOOR_Y - o->height;
    entity_set_center_x(o, 57);

    o->on_use = on_epilog_use;
  }
}

void update_state(void) {
  if (is_showing_speech())
    return;
  if (curtain_is_fading(&curtain))
    return;
  if (timer_duration)
    return;
  if (play_animation_type) {
    Entity *en = world_find_first(world, play_animation_type);
    if (en && en->sprite) {
      sprite_update(en->sprite, app_delta());
      if (!sprite_is_playing(en->sprite, play_animation)) {
        play_animation_type = 0;
        play_animation = 0;
      }
    }
    return;
  }
  if (wait_for_type_removed) {
    Entity *en = world_find_first(world, wait_for_type_removed);
    if (!en) {
      wait_for_type_removed = 0;
    }
    return;
  }

  switch (global_state) {
    default:
      clear_state();
      break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:

    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:

      break;


    case IN_EPILOG+0:
      break;
    case IN_EPILOG+1:
      {
        Entity *player = world_find_first(world, ENTITY_PLAYER);
        if (player) {
          if (player->color.w > 0) {
            move_to(&player->color.w, 0, app_delta() / 0.5f);
            return;
          }
        }
      }
      break;
    case IN_EPILOG+2:
      break;
    case IN_EPILOG+3:
      timer_wait(1.8);
      break;
    case IN_EPILOG+4:
      break;
    case IN_EPILOG+5:
      curtain_fade(&curtain, 1,1,1, 1, 3.0f);
      break;
    case IN_EPILOG+6:
      break;
    case IN_EPILOG+7:
      show_ending_text();
      break;
    case IN_EPILOG+8:
      break;
    case IN_EPILOG+9:
      break;


    case IN_GO_HOME+0:
      break;
    case IN_GO_HOME+1:
      {
      float dur = 4;
      curtain_set(&curtain, 1,1,1,0);
      curtain_fade(&curtain, 1,1,1,1, dur);
      fade_ambient_sound(0, dur);
      }
      break;
    case IN_GO_HOME+2:

      timer_wait(2);
      play_music("data/epilog.ogg");

      break;
    case IN_GO_HOME+3:
      should_shake_camera = false;
      request_change_scene(scene_train_home);
      break;
    case IN_GO_HOME+4:
      suspend_entity_type(ENTITY_PLAYER);
      break;
    case IN_GO_HOME+5:
      curtain_fade(&curtain, 1,1,1,0, 3.0);
      break;
    case IN_GO_HOME+6:
      break;
    case IN_GO_HOME+7:
      break;
    case IN_GO_HOME+8:
      play_animation_for_and_wait(ENTITY_PLAYER, "get_up");
      break;
    case IN_GO_HOME+9:
      unsuspend_entity_type(ENTITY_PLAYER);
      break;



    case IN_GO_HOME+10:
      {
      Entity *timer = world_add(world, ENTITY_TIMER);
      timer->timer_left = 4;
      timer->on_over = timer_over_open_door;
      }
      break;
    case IN_GO_HOME+11:
      clear_state();
      break;
    case IN_GO_HOME+12:
      break;
    case IN_GO_HOME+13:
      break;
    case IN_GO_HOME+14:
      break;
    case IN_GO_HOME+15:
      break;
    case IN_GO_HOME+16:
      break;
    case IN_GO_HOME+17:
      break;
    case IN_GO_HOME+18:
      break;
    case IN_GO_HOME+19:
      break;

    case IN_GO_HOME+20:
      break;
    case IN_GO_HOME+21:
      break;
    case IN_GO_HOME+22:
      break;
    case IN_GO_HOME+23:
      break;
    case IN_GO_HOME+24:
      break;
    case IN_GO_HOME+25:
      break;
    case IN_GO_HOME+26:
      break;
    case IN_GO_HOME+27:
      break;
    case IN_GO_HOME+28:
      break;
    case IN_GO_HOME+29:
      break;

    case IN_GO_HOME+30:
      break;
    case IN_GO_HOME+31:
      break;
    case IN_GO_HOME+32:
      break;
    case IN_GO_HOME+33:
      break;
    case IN_GO_HOME+34:
      break;
    case IN_GO_HOME+35:
      break;
    case IN_GO_HOME+36:
      break;
    case IN_GO_HOME+37:
      break;
    case IN_GO_HOME+38:
      break;
    case IN_GO_HOME+39:
      break;


    case IN_GO_HOME+40:
      break;
    case IN_GO_HOME+41:
      break;
    case IN_GO_HOME+42:
      break;
    case IN_GO_HOME+43:
      break;
    case IN_GO_HOME+44:
      break;
    case IN_GO_HOME+45:
      break;
    case IN_GO_HOME+46:
      break;
    case IN_GO_HOME+47:
      break;
    case IN_GO_HOME+48:
      break;
    case IN_GO_HOME+49:
      break;


    case IN_GOTO_FINAL_CONVERSATION+0:
      timer_wait(0.8);
      break;
    case IN_GOTO_FINAL_CONVERSATION+1:
      show_speech_for(ENTITY_VOMIT_GIRL, "That was close..");
      break;
    case IN_GOTO_FINAL_CONVERSATION+2:
      break;
    case IN_GOTO_FINAL_CONVERSATION+3:
      show_speech_for(ENTITY_VOMIT_GIRL, "You almost became her food.");
      break;
    case IN_GOTO_FINAL_CONVERSATION+4:
      timer_wait(0.8);
      break;
    case IN_GOTO_FINAL_CONVERSATION+5:
      show_speech_for(ENTITY_VOMIT_GIRL, "...or worse.");
      break;
    case IN_GOTO_FINAL_CONVERSATION+6:
      timer_wait(0.8);
      break;
    case IN_GOTO_FINAL_CONVERSATION+7:
      show_speech_for(ENTITY_VOMIT_GIRL, "Listen, thanks for helping me.");
      break;
    case IN_GOTO_FINAL_CONVERSATION+8:
      break;
    case IN_GOTO_FINAL_CONVERSATION+9:
      break;


    case IN_GOTO_FINAL_CONVERSATION+10:
      show_speech_for(ENTITY_VOMIT_GIRL, "But you need to get out of here.");
      break;
    case IN_GOTO_FINAL_CONVERSATION+11:
      break;
    case IN_GOTO_FINAL_CONVERSATION+12:
      show_speech_for(ENTITY_VOMIT_GIRL, "This train can't take you home.");
      break;
    case IN_GOTO_FINAL_CONVERSATION+13:
      timer_wait(0.8);
      break;
    case IN_GOTO_FINAL_CONVERSATION+14:
      break;
    case IN_GOTO_FINAL_CONVERSATION+15:
      show_speech_for(ENTITY_VOMIT_GIRL, "All you need to do...");
      break;
    case IN_GOTO_FINAL_CONVERSATION+16:
      show_speech_for(ENTITY_VOMIT_GIRL, "Is spin around 7 times.");
      break;
    case IN_GOTO_FINAL_CONVERSATION+17:
      break;
    case IN_GOTO_FINAL_CONVERSATION+18:
      show_speech_for(ENTITY_VOMIT_GIRL, "You hear me? 7 times.");
      break;
    case IN_GOTO_FINAL_CONVERSATION+19:
      show_speech_for(ENTITY_VOMIT_GIRL, "Don't forget.");
      break;


    case IN_GOTO_FINAL_CONVERSATION+20:
      // Train stops.
      break;
    case IN_GOTO_FINAL_CONVERSATION+21:
      timer_wait(0.8);
      break;
    case IN_GOTO_FINAL_CONVERSATION+22:
      suspend_entity_type(ENTITY_TRAIN_DOOR);
      break;
    case IN_GOTO_FINAL_CONVERSATION+23:
      play_animation_for_and_wait(ENTITY_TRAIN_DOOR, "open");
      break;
    case IN_GOTO_FINAL_CONVERSATION+24:
      unsuspend_entity_type(ENTITY_TRAIN_DOOR);
      break;
    case IN_GOTO_FINAL_CONVERSATION+25:
      show_speech_for(ENTITY_VOMIT_GIRL, "Looks like this is my stop.");
      break;
    case IN_GOTO_FINAL_CONVERSATION+26:
      break;
    case IN_GOTO_FINAL_CONVERSATION+27:
      {
      Entity *door = world_find_first(world, ENTITY_TRAIN_DOOR);
      Entity *vg = world_find_first(world, ENTITY_VOMIT_GIRL);
      if (door && vg) {
        float tx = door->x + door->width/2 - vg->width/2;
        if (vg->x > tx)
          vg->facing = FACING_LEFT;
        else if (vg->x < tx)
          vg->facing = FACING_RIGHT;
        move_to(&vg->x, tx, app_delta() * 60.f);
        sprite_play(vg->sprite, "walk", SPRITE_LOOPED);
        sprite_update(vg->sprite, app_delta());

        if (vg->x == tx) {
          sprite_play(vg->sprite, "idle_2", 0);
        }
        else {
          return;
        }
      }
      }
      break;
    case IN_GOTO_FINAL_CONVERSATION+28:
      show_speech_for(ENTITY_VOMIT_GIRL, "May we meet again.");
      break;
    case IN_GOTO_FINAL_CONVERSATION+29:
      break;



    case IN_GOTO_FINAL_CONVERSATION+30:
      show_speech_for(ENTITY_VOMIT_GIRL, "Remember, 7 times.");
      break;
    case IN_GOTO_FINAL_CONVERSATION+31:
      timer_wait(0.5);
      break;
    case IN_GOTO_FINAL_CONVERSATION+32:
      {
      Entity *vg = world_find_first(world, ENTITY_VOMIT_GIRL);
      if (vg && vg->color.data[3] > 0) {
        move_to(&vg->color.data[3], 0, app_delta() / 0.5f);
        return;
      }
      else {
        vg->removed = true;
      }
      }
      break;
    case IN_GOTO_FINAL_CONVERSATION+33:
      break;
    case IN_GOTO_FINAL_CONVERSATION+34:
      timer_wait(0.5);
      break;
    case IN_GOTO_FINAL_CONVERSATION+35:
      suspend_entity_type(ENTITY_TRAIN_DOOR);
      break;
    case IN_GOTO_FINAL_CONVERSATION+36:
      play_animation_for_and_wait(ENTITY_TRAIN_DOOR, "close");
      break;
    case IN_GOTO_FINAL_CONVERSATION+37:
      unsuspend_entity_type(ENTITY_TRAIN_DOOR);
      break;
    case IN_GOTO_FINAL_CONVERSATION+38:
      break;
    case IN_GOTO_FINAL_CONVERSATION+39:
      clear_state();
      return;
      break;



    case IN_GOTO_FINAL_CONVERSATION+40:
      break;
    case IN_GOTO_FINAL_CONVERSATION+41:
      break;
    case IN_GOTO_FINAL_CONVERSATION+42:
      break;
    case IN_GOTO_FINAL_CONVERSATION+43:
      break;
    case IN_GOTO_FINAL_CONVERSATION+44:
      break;
    case IN_GOTO_FINAL_CONVERSATION+45:
      break;
    case IN_GOTO_FINAL_CONVERSATION+46:
      break;
    case IN_GOTO_FINAL_CONVERSATION+47:
      break;
    case IN_GOTO_FINAL_CONVERSATION+48:
      break;
    case IN_GOTO_FINAL_CONVERSATION+49:
      break;

    case IN_GOTO_FINAL_CONVERSATION+50:
      break;
    case IN_GOTO_FINAL_CONVERSATION+51:
      break;
    case IN_GOTO_FINAL_CONVERSATION+52:
      break;
    case IN_GOTO_FINAL_CONVERSATION+53:
      break;
    case IN_GOTO_FINAL_CONVERSATION+54:
      break;
    case IN_GOTO_FINAL_CONVERSATION+55:
      break;
    case IN_GOTO_FINAL_CONVERSATION+56:
      break;
    case IN_GOTO_FINAL_CONVERSATION+57:
      break;
    case IN_GOTO_FINAL_CONVERSATION+58:
      break;
    case IN_GOTO_FINAL_CONVERSATION+59:
      break;



    case IN_GOTO_FINAL_CONVERSATION+60:
      break;
    case IN_GOTO_FINAL_CONVERSATION+61:
      break;
    case IN_GOTO_FINAL_CONVERSATION+62:
      break;
    case IN_GOTO_FINAL_CONVERSATION+63:
      break;
    case IN_GOTO_FINAL_CONVERSATION+64:
      break;
    case IN_GOTO_FINAL_CONVERSATION+65:
      break;
    case IN_GOTO_FINAL_CONVERSATION+66:
      break;
    case IN_GOTO_FINAL_CONVERSATION+67:
      break;
    case IN_GOTO_FINAL_CONVERSATION+68:
      break;
    case IN_GOTO_FINAL_CONVERSATION+69:
      break;

    case IN_ENCOUNTER+0:
      show_speech_for(ENTITY_PLAYER, "It won't open.");
      break;

    case IN_ENCOUNTER+1:

      if (flags.override_brightness > 0) {
        move_to(&flags.override_brightness, 0, app_delta() / 2.f);
        return;
      }

      break;
    case IN_ENCOUNTER+2:
      timer_wait(1);

      break;
    case IN_ENCOUNTER+3:

      fade_ambient_sound(0.0, 6);
      fade_out_all_music();

      break;
    case IN_ENCOUNTER+4:
      timer_wait(1);

      break;
    case IN_ENCOUNTER+5:
      break;
    case IN_ENCOUNTER+6:
      if (window_flash == 0) {
        world_add_at(world, ENTITY_EVIL_LITTLE_GIRL, 277, 0);
      }
      else {
        return;
      }
      break;
    case IN_ENCOUNTER+7:

      {
        Entity *en = world_find_first(world, ENTITY_EVIL_LITTLE_GIRL);
        if (en) {
          en->x -= app_delta() * 10;
        }

        if (en->x > 187) {
          return;
        }
        en->x = 187;
      }
      break;
    case IN_ENCOUNTER+8:
      if (window_flash == 0) {
        Entity *en = world_find_first(world, ENTITY_EVIL_LITTLE_GIRL);
        if (en) {
          world_add_at(world, ENTITY_SPIDER, en->x + en->width/2 - 53, 0);
          en->removed = true;
        }

      }
      else {
        return;
      }

      break;
    case IN_ENCOUNTER+9:

      {
        Entity *en = world_find_first(world, ENTITY_SPIDER);
        float tx = 55;

        if (en) {
          if (en->x > tx) {
            en->x -= 10 * app_delta();
            return;
          }
          else {
            curtain_set(&curtain, 0, 0, 0, 1);
            en->x = tx;

            show_window_flash = false;
          }
        }

      }

      break;


    case IN_ENCOUNTER+10:

      timer_wait(1.f);

      break;
    case IN_ENCOUNTER+11:
      break;

    case IN_ENCOUNTER+12:

      show_speech_for(ENTITY_PLAYER, "Quick! Get in here!");
      speech_override_timer = 1.f;

      break;
    case IN_ENCOUNTER+13:
      break;
    case IN_ENCOUNTER+14:

      world->left_scene = scene_train_final;
      goto_left_scene(0);
      scene_change_fadein_wait_override = 3.f;
      scene_change_fadein_duration_override = 3.f;
      return;

      break;
    case IN_ENCOUNTER+15:
      break;
    case IN_ENCOUNTER+16:
      break;
    case IN_ENCOUNTER+17:
      break;
    case IN_ENCOUNTER+18:
      break;
    case IN_ENCOUNTER+19:
      break;

    case IN_ENCOUNTER+20:
      break;
    case IN_ENCOUNTER+21:
      break;
    case IN_ENCOUNTER+22:
      break;
    case IN_ENCOUNTER+23:
      break;
    case IN_ENCOUNTER+24:
      break;
    case IN_ENCOUNTER+25:
      break;
    case IN_ENCOUNTER+26:
      break;
    case IN_ENCOUNTER+27:
      break;
    case IN_ENCOUNTER+28:
      break;
    case IN_ENCOUNTER+29:
      break;

    case IN_ENCOUNTER+30:
      break;
    case IN_ENCOUNTER+31:
      break;
    case IN_ENCOUNTER+32:
      break;
    case IN_ENCOUNTER+33:
      break;
    case IN_ENCOUNTER+34:
      break;
    case IN_ENCOUNTER+35:
      break;
    case IN_ENCOUNTER+36:
      break;
    case IN_ENCOUNTER+37:
      break;
    case IN_ENCOUNTER+38:
      break;
    case IN_ENCOUNTER+39:
      break;

    case IN_ENCOUNTER+40:
      break;
    case IN_ENCOUNTER+41:
      break;
    case IN_ENCOUNTER+42:
      break;
    case IN_ENCOUNTER+43:
      break;
    case IN_ENCOUNTER+44:
      break;
    case IN_ENCOUNTER+45:
      break;
    case IN_ENCOUNTER+46:
      break;
    case IN_ENCOUNTER+47:
      break;
    case IN_ENCOUNTER+48:
      break;
    case IN_ENCOUNTER+49:
      break;

    case IN_ENCOUNTER+50:
      break;
    case IN_ENCOUNTER+51:
      break;
    case IN_ENCOUNTER+52:
      break;
    case IN_ENCOUNTER+53:
      break;
    case IN_ENCOUNTER+54:
      break;
    case IN_ENCOUNTER+55:
      break;
    case IN_ENCOUNTER+56:
      break;
    case IN_ENCOUNTER+57:
      break;
    case IN_ENCOUNTER+58:
      break;
    case IN_ENCOUNTER+59:
      break;



    case IN_FIRST_FLASHLIGHT+0:
      timer_wait(0.6);
      break;
    case IN_FIRST_FLASHLIGHT+1:
      flashlight_on = true;
      break;
    case IN_FIRST_FLASHLIGHT+2:
      clear_state();
      break;
    case IN_FIRST_FLASHLIGHT+3:
      timer_wait(0.3);
      break;

    case IN_FADE_IN:
      curtain_set(&curtain, 0,0,0,1);
      curtain_fade(&curtain, 0,0,0,0, 3);
      break;

    case IN_FADE_OUT:
      fade_out_all_music();
      break;
    case IN_FADE_OUT+1:
      curtain_fade(&curtain, 0,0,0,1, 3);
      break;
    case IN_FADE_OUT+2:
      timer_wait(2);
      break;
    case IN_FADE_OUT+3:
      request_reset_game();
      break;
    case IN_FADE_OUT+4:
      clear_state();
      break;

    case IN_NEWSPAPER_GUY_SAY:
      if (flags.gave_old_guy_newspaper) {
        set_state(IN_NEWSPAPER_GUY_SAY + 50);
        return;
      }
      else if (has_item(ITEM_NEWSPAPER)) {
        set_state(IN_NEWSPAPER_GUY_SAY + 20);
        return;
      }
      else if (flags.talked_to_newspaper_guy) {
        set_state(IN_NEWSPAPER_GUY_REMIND);
        return;
      }
      flags.talked_to_newspaper_guy = true;
      break;

    case IN_NEWSPAPER_GUY_SAY + 1:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "No, this is not it either...");
      break;
    case IN_NEWSPAPER_GUY_SAY + 2:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "!");
      break;
    case IN_NEWSPAPER_GUY_SAY + 3:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "You there, young man!");
      break;
    case IN_NEWSPAPER_GUY_SAY + 4:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "Have you seen the latest papers!?");
      break;
    case IN_NEWSPAPER_GUY_SAY + 5:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "I've been looking for it everywhere.");
      break;
    case IN_NEWSPAPER_GUY_SAY + 6:
      break;
    case IN_NEWSPAPER_GUY_SAY + 7:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "If you happen to find it, please bring it to me.");
      break;
    case IN_NEWSPAPER_GUY_SAY + 8:
      break;
    case IN_NEWSPAPER_GUY_SAY + 9:
      clear_state();
      break;
    case IN_NEWSPAPER_GUY_SAY + 10:
      break;
    case IN_NEWSPAPER_GUY_SAY + 11:
      break;
    case IN_NEWSPAPER_GUY_SAY + 12:
      break;
    case IN_NEWSPAPER_GUY_SAY + 13:
      break;
    case IN_NEWSPAPER_GUY_SAY + 14:
      break;
    case IN_NEWSPAPER_GUY_SAY + 15:
      break;
    case IN_NEWSPAPER_GUY_SAY + 16:
      break;
    case IN_NEWSPAPER_GUY_SAY + 17:
      break;
    case IN_NEWSPAPER_GUY_SAY + 18:
      break;
    case IN_NEWSPAPER_GUY_SAY + 19:
      break;
    case IN_NEWSPAPER_GUY_SAY + 20:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "Ah! The papers you're holding!");
      break;
    case IN_NEWSPAPER_GUY_SAY + 21:
      break;
    case IN_NEWSPAPER_GUY_SAY + 22:
      break;
    case IN_NEWSPAPER_GUY_SAY + 23:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "It's exactly what I'm looking for!");
      break;
    case IN_NEWSPAPER_GUY_SAY + 24:
      break;
    case IN_NEWSPAPER_GUY_SAY + 25:
      take_away_item(ITEM_NEWSPAPER);
      flags.gave_old_guy_newspaper = true;
      break;
    case IN_NEWSPAPER_GUY_SAY + 26:
      break;
    case IN_NEWSPAPER_GUY_SAY + 27:
      break;
    case IN_NEWSPAPER_GUY_SAY + 28:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "Thanks a bunch, young man!");
      break;
    case IN_NEWSPAPER_GUY_SAY + 29:
      break;
    case IN_NEWSPAPER_GUY_SAY + 30:
      break;
    case IN_NEWSPAPER_GUY_SAY + 31:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "Here, take this!");
      break;
    case IN_NEWSPAPER_GUY_SAY + 32:
      break;
    case IN_NEWSPAPER_GUY_SAY + 33:
      give_item(ITEM_LIGHTER);
      flags.got_lighter = true;
      break;
    case IN_NEWSPAPER_GUY_SAY + 34:
      break;
    case IN_NEWSPAPER_GUY_SAY + 35:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "I have no use for it anymore.");
      break;
    case IN_NEWSPAPER_GUY_SAY + 36:
      clear_state();
      break;
    case IN_NEWSPAPER_GUY_SAY + 37:
      break;
    case IN_NEWSPAPER_GUY_SAY + 38:
      break;
    case IN_NEWSPAPER_GUY_SAY + 39:
      break;
    case IN_NEWSPAPER_GUY_SAY + 40:
      break;
    case IN_NEWSPAPER_GUY_SAY + 41:
      break;
    case IN_NEWSPAPER_GUY_SAY + 42:
      break;
    case IN_NEWSPAPER_GUY_SAY + 43:
      break;
    case IN_NEWSPAPER_GUY_SAY + 44:
      break;
    case IN_NEWSPAPER_GUY_SAY + 45:
      break;
    case IN_NEWSPAPER_GUY_SAY + 46:
      break;
    case IN_NEWSPAPER_GUY_SAY + 47:
      break;
    case IN_NEWSPAPER_GUY_SAY + 48:
      break;
    case IN_NEWSPAPER_GUY_SAY + 49:
      break;
    case IN_NEWSPAPER_GUY_SAY + 50:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "Thanks again, young man.");
      break;
    case IN_NEWSPAPER_GUY_SAY + 51:
      break;
    case IN_NEWSPAPER_GUY_SAY + 52:
      break;
    case IN_NEWSPAPER_GUY_SAY + 53:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "Now let's see... 4-13-6-...");
      break;
    case IN_NEWSPAPER_GUY_SAY + 54:
      break;
    case IN_NEWSPAPER_GUY_SAY + 55:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "Oh, rats!");
      break;
    case IN_NEWSPAPER_GUY_SAY + 56:
      break;
    case IN_NEWSPAPER_GUY_SAY + 57:
      break;
    case IN_NEWSPAPER_GUY_SAY + 58:
      break;
    case IN_NEWSPAPER_GUY_SAY + 59:
      break;


    // Other thing
    case IN_NEWSPAPER_GUY_REMIND:
      show_speech_for(ENTITY_NEWSPAPER_GUY, "Don't forget, it's the latest papers!");
      break;


    case IN_GOTO_SCENE_LEFT:
      curtain_fade(&curtain, 0,0,0, 1, 0.2);
      break;

    case IN_GOTO_SCENE_LEFT+1:
      if (world)
        request_change_scene(world->left_scene);
      break;

    case IN_GOTO_SCENE_LEFT+2:
      if (scene_change_fadein_wait_override)
        timer_wait(scene_change_fadein_wait_override);
      scene_change_fadein_wait_override = 0;
      break;

    case IN_GOTO_SCENE_LEFT+3:
      {
        curtain_fade(&curtain, 0,0,0,0, 0.2+scene_change_fadein_duration_override);
        scene_change_fadein_duration_override = 0;
      }
      break;

    
    case IN_GOTO_SCENE_RIGHT:
      curtain_fade(&curtain, 0,0,0, 1, 0.2);
      break;

    case IN_GOTO_SCENE_RIGHT+1:
      if (world)
        request_change_scene(world->right_scene);
      break;

    case IN_GOTO_SCENE_RIGHT+2:
      if (scene_change_fadein_wait_override)
        timer_wait(scene_change_fadein_wait_override);
      scene_change_fadein_wait_override = 0;
      break;

    case IN_GOTO_SCENE_RIGHT+3:
      {
        curtain_fade(&curtain, 0,0,0,0, 0.2+scene_change_fadein_duration_override);
        scene_change_fadein_duration_override = 0;
      }
      break;

// HERE
    case IN_VOMIT_GIRL_TALK+0:
      if (false) {}
      else if (flags.talked_to_vomit_girl) {
      }
      else if (has_item(ITEM_PILL_BOTTLE)) {
        set_state(IN_VOMIT_GIRL_TALK + 20);
        return;
      }
      else {
        //set_state(IN_VOMIT_GIRL_TALK + 20);
        set_state(IN_VOMIT_GIRL_TALK+3);
        return;
      }
      break;
    case IN_VOMIT_GIRL_TALK+1:
      break;
    case IN_VOMIT_GIRL_TALK+2:
      break;
    case IN_VOMIT_GIRL_TALK+3:
      show_speech_for(ENTITY_VOMIT_GIRL, "Ughhh..");
      break;
    case IN_VOMIT_GIRL_TALK+4:
      timer_wait(1.5);
      break;
    case IN_VOMIT_GIRL_TALK+5:
      show_speech_for(ENTITY_VOMIT_GIRL, "Urghhhhhhhhh..");
      break;
    case IN_VOMIT_GIRL_TALK+6:
      clear_state();
      break;
    case IN_VOMIT_GIRL_TALK+7:
      break;
    case IN_VOMIT_GIRL_TALK+8:
      break;
    case IN_VOMIT_GIRL_TALK+9:
      break;

    case IN_VOMIT_GIRL_TALK+10:
      break;
    case IN_VOMIT_GIRL_TALK+11:
      break;
    case IN_VOMIT_GIRL_TALK+12:
      break;
    case IN_VOMIT_GIRL_TALK+13:
      break;
    case IN_VOMIT_GIRL_TALK+14:
      break;
    case IN_VOMIT_GIRL_TALK+15:
      break;
    case IN_VOMIT_GIRL_TALK+16:
      break;
    case IN_VOMIT_GIRL_TALK+17:
      break;
    case IN_VOMIT_GIRL_TALK+18:
      break;
    case IN_VOMIT_GIRL_TALK+19:
      break;

    case IN_VOMIT_GIRL_TALK+20:
      show_speech_for(ENTITY_VOMIT_GIRL, "Ughhh..");
      flags.talked_to_vomit_girl = true;
      break;
    case IN_VOMIT_GIRL_TALK+21:
      break;
    case IN_VOMIT_GIRL_TALK+22:
      timer_wait(0.5);
      break;
    case IN_VOMIT_GIRL_TALK+23:
      break;
    case IN_VOMIT_GIRL_TALK+24:
      show_speech_for(ENTITY_VOMIT_GIRL, "Ah... You found my medicine.");
      break;
    case IN_VOMIT_GIRL_TALK+25:
      break;
    case IN_VOMIT_GIRL_TALK+26:
      timer_wait(0.5);
      break;
    case IN_VOMIT_GIRL_TALK+27:
      take_away_item(ITEM_PILL_BOTTLE);
      suspend_entity_type(ENTITY_VOMIT_GIRL);
      play_animation_for_and_wait(ENTITY_VOMIT_GIRL, "recover");
      break;
    case IN_VOMIT_GIRL_TALK+28:
      break;
    case IN_VOMIT_GIRL_TALK+29:
      show_speech_for(ENTITY_VOMIT_GIRL, "...That's better.");
      break;


    case IN_VOMIT_GIRL_TALK+30:
      break;
    case IN_VOMIT_GIRL_TALK+31:
      break;
    case IN_VOMIT_GIRL_TALK+32:
      timer_wait(0.5);
      break;
    case IN_VOMIT_GIRL_TALK+33:
      break;
    case IN_VOMIT_GIRL_TALK+34:
      show_speech_for(ENTITY_VOMIT_GIRL, "Sir, you're a life saver! I can't believe...");
      break;
    case IN_VOMIT_GIRL_TALK+35:
      break;
    case IN_VOMIT_GIRL_TALK+36:
      timer_wait(0.5);
      break;
    case IN_VOMIT_GIRL_TALK+37:
      break;
    case IN_VOMIT_GIRL_TALK+38:
      break;
    case IN_VOMIT_GIRL_TALK+39:
      show_speech_for(ENTITY_VOMIT_GIRL, "Wait..");
      break;


    case IN_VOMIT_GIRL_TALK+40:
      break;
    case IN_VOMIT_GIRL_TALK+41:
      break;
    case IN_VOMIT_GIRL_TALK+42:
      show_speech_for(ENTITY_VOMIT_GIRL, "This is all wrong..");
      break;
    case IN_VOMIT_GIRL_TALK+43:
      break;
    case IN_VOMIT_GIRL_TALK+44:
      show_speech_for(ENTITY_VOMIT_GIRL, "You shouldn't be here.");
      break;
    case IN_VOMIT_GIRL_TALK+45:
      break;
    case IN_VOMIT_GIRL_TALK+46:
      show_speech_for(ENTITY_VOMIT_GIRL, "Listen, you have to leave.");
      break;
    case IN_VOMIT_GIRL_TALK+47:
      break;
    case IN_VOMIT_GIRL_TALK+48:
      show_speech_for(ENTITY_VOMIT_GIRL, "Quick, follow me!");
      break;
    case IN_VOMIT_GIRL_TALK+49:
      break;


    case IN_VOMIT_GIRL_TALK+50:
      break;
    case IN_VOMIT_GIRL_TALK+51:
      {
        Entity *en = world_find_first(world, ENTITY_VOMIT_GIRL);
        if (en) {
          en->state = 20;
        }
        unsuspend_entity_type(ENTITY_VOMIT_GIRL);
      }
      break;
    case IN_VOMIT_GIRL_TALK+52:
      wait_for_entity_type_removed(ENTITY_VOMIT_GIRL);
      break;
    case IN_VOMIT_GIRL_TALK+53:
      break;
    case IN_VOMIT_GIRL_TALK+54:
      clear_state();
      break;
    case IN_VOMIT_GIRL_TALK+55:
      break;
    case IN_VOMIT_GIRL_TALK+56:
      break;
    case IN_VOMIT_GIRL_TALK+57:
      break;
    case IN_VOMIT_GIRL_TALK+58:
      break;
    case IN_VOMIT_GIRL_TALK+59:
      break;

    case IN_VOMIT_GIRL_TALK+60:
      break;
    case IN_VOMIT_GIRL_TALK+61:
      break;
    case IN_VOMIT_GIRL_TALK+62:
      break;
    case IN_VOMIT_GIRL_TALK+63:
      break;
    case IN_VOMIT_GIRL_TALK+64:
      break;
    case IN_VOMIT_GIRL_TALK+65:
      break;
    case IN_VOMIT_GIRL_TALK+66:
      break;
    case IN_VOMIT_GIRL_TALK+67:
      break;
    case IN_VOMIT_GIRL_TALK+68:
      break;
    case IN_VOMIT_GIRL_TALK+69:
      break;

    case IN_WONT_OPEN:
    {
      const char *messages[] = {
        "This door won't open.",
        "Looks like it's locked.",
        "Can't go through here.",
        "This isn't the way to go.",
      };
      u32 idx = randomu() % _countof(messages);
      show_speech_for(ENTITY_PLAYER, messages[min(idx, _countof(messages)-1)]);
    } break;


    case IN_HOMELESS_GUY_TALK:
    {
      if (!flags.talked_to_newspaper_guy) {
        show_speech_for(ENTITY_HOMELESS_GUY, "Zzz...");
      }
      else {
        set_state(IN_HOMELESS_GUY_TALK + 3);
        return;
      }
    } break;
    case IN_HOMELESS_GUY_TALK + 1:
      clear_state();
      return;
      break;
    case IN_HOMELESS_GUY_TALK + 2:
      break;
    case IN_HOMELESS_GUY_TALK + 3:
      if (flags.homeless_guy_gave_newspaper) {
        set_state(IN_HOMELESS_GUY_TALK + 60);
        return;
      }
      else if (flags.talked_to_homeless_guy) {
        set_state(IN_HOMELESS_GUY_TALK + 30);
        return;
      }
      else {
        flags.talked_to_homeless_guy = true;
        show_speech_for(ENTITY_HOMELESS_GUY, "Oh hey there brother, I didn't see you there.");
      }
      break;
    case IN_HOMELESS_GUY_TALK + 4:
      break;
    case IN_HOMELESS_GUY_TALK + 5:
      show_speech_for(ENTITY_HOMELESS_GUY, "Is there something you need?");
      break;
    case IN_HOMELESS_GUY_TALK + 6:
      break;
    case IN_HOMELESS_GUY_TALK + 7:
      break;
    case IN_HOMELESS_GUY_TALK + 8:
      show_speech_for(ENTITY_HOMELESS_GUY, "Oh, you want one of my newspapers.");
      break;
    case IN_HOMELESS_GUY_TALK + 9:
      break;
    case IN_HOMELESS_GUY_TALK + 10:
      break;
    case IN_HOMELESS_GUY_TALK + 11:
      show_speech_for(ENTITY_HOMELESS_GUY, "Well for sure, brother!");
      break;
    case IN_HOMELESS_GUY_TALK + 12:
      break;
    case IN_HOMELESS_GUY_TALK + 13:
      break;
    case IN_HOMELESS_GUY_TALK + 14:
      show_speech_for(ENTITY_HOMELESS_GUY, "I can only give you one though, or I'll get cold.");
      break;
    case IN_HOMELESS_GUY_TALK + 15:
      break;
    case IN_HOMELESS_GUY_TALK + 16:
      break;
    case IN_HOMELESS_GUY_TALK + 17:
      {
      const char *dates[] = {
        "This one...",
        "Nevermind",
      };
      show_speech_for_and_prompt(ENTITY_HOMELESS_GUY, "Which one do you want?", dates, _countof(dates));
      }
      break;
    case IN_HOMELESS_GUY_TALK + 18:
      if (speech_choice_index == 1) {
        set_state(IN_HOMELESS_GUY_TALK + 45);
        return;
      }
      else {
        give_item(ITEM_NEWSPAPER);
        flags.homeless_guy_gave_newspaper = true;
      }
      break;
    case IN_HOMELESS_GUY_TALK + 19:
      break;
    case IN_HOMELESS_GUY_TALK + 20:
      break;
    case IN_HOMELESS_GUY_TALK + 21:
      break;
    case IN_HOMELESS_GUY_TALK + 22:
      break;
    case IN_HOMELESS_GUY_TALK + 23:
      show_speech_for(ENTITY_HOMELESS_GUY, "Here you go! Sleep tight brother!");
      break;
    case IN_HOMELESS_GUY_TALK + 24:
      clear_state();
      return;
      break;
    case IN_HOMELESS_GUY_TALK + 25:
      break;
    case IN_HOMELESS_GUY_TALK + 26:
      break;
    case IN_HOMELESS_GUY_TALK + 27:
      break;
    case IN_HOMELESS_GUY_TALK + 28:
      break;
    case IN_HOMELESS_GUY_TALK + 29:
      break;
    case IN_HOMELESS_GUY_TALK + 30:
      show_speech_for(ENTITY_HOMELESS_GUY, "Hello again brother! Still want that paper?");
      break;
    case IN_HOMELESS_GUY_TALK + 31:
      set_state(IN_HOMELESS_GUY_TALK + 17);
      return;
      break;
    case IN_HOMELESS_GUY_TALK + 32:
      break;
    case IN_HOMELESS_GUY_TALK + 33:
      break;
    case IN_HOMELESS_GUY_TALK + 34:
      break;
    case IN_HOMELESS_GUY_TALK + 35:
      break;
    case IN_HOMELESS_GUY_TALK + 36:
      break;
    case IN_HOMELESS_GUY_TALK + 37:
      break;
    case IN_HOMELESS_GUY_TALK + 38:
      break;
    case IN_HOMELESS_GUY_TALK + 39:
      break;
    case IN_HOMELESS_GUY_TALK + 40:
      break;
    case IN_HOMELESS_GUY_TALK + 41:
      break;
    case IN_HOMELESS_GUY_TALK + 42:
      break;
    case IN_HOMELESS_GUY_TALK + 43:
      break;
    case IN_HOMELESS_GUY_TALK + 44:
      break;
// This is where you say you don't want newspaper
    case IN_HOMELESS_GUY_TALK + 45:
      show_speech_for(ENTITY_HOMELESS_GUY, "Let me know if you change your mind brother!");
      break;
    case IN_HOMELESS_GUY_TALK + 46:
      clear_state();
      return;
      break;
    case IN_HOMELESS_GUY_TALK + 47:
      break;
    case IN_HOMELESS_GUY_TALK + 48:
      break;
    case IN_HOMELESS_GUY_TALK + 49:
      break;
    case IN_HOMELESS_GUY_TALK + 50:
      break;
    case IN_HOMELESS_GUY_TALK + 51:
      break;
    case IN_HOMELESS_GUY_TALK + 52:
      break;
    case IN_HOMELESS_GUY_TALK + 53:
      break;
    case IN_HOMELESS_GUY_TALK + 54:
      break;
    case IN_HOMELESS_GUY_TALK + 55:
      break;
    case IN_HOMELESS_GUY_TALK + 56:
      break;
    case IN_HOMELESS_GUY_TALK + 57:
      break;
    case IN_HOMELESS_GUY_TALK + 58:
      break;
    case IN_HOMELESS_GUY_TALK + 59:
      break;
// You already have the newspaper
    case IN_HOMELESS_GUY_TALK + 60:
      show_speech_for(ENTITY_HOMELESS_GUY, "What is it brother? Anything else you need?");
      break;
    case IN_HOMELESS_GUY_TALK + 61:
      break;
    case IN_HOMELESS_GUY_TALK + 62:
      break;
    case IN_HOMELESS_GUY_TALK + 63:
      break;
    case IN_HOMELESS_GUY_TALK + 64:
      break;
    case IN_HOMELESS_GUY_TALK + 65:
      break;
    case IN_HOMELESS_GUY_TALK + 66:
      break;
    case IN_HOMELESS_GUY_TALK + 67:
      break;
    case IN_HOMELESS_GUY_TALK + 68:
      break;
    case IN_HOMELESS_GUY_TALK + 69:
      break;
    case IN_HOMELESS_GUY_TALK + 70:
      clear_state();
      return;
      break;

    case IN_SMOKER_TALK+0:
      break;
    case IN_SMOKER_TALK+1:
      if (false) {}
      else if (flags.gave_away_lighter) {
        set_state(IN_SMOKER_TALK + 45);
        return;
      }
      else if (has_item(ITEM_LIGHTER)) {
        set_state(IN_SMOKER_TALK + 8);
        return;
      }
      else {
        show_speech_for(ENTITY_SMOKER, "Damn... This lighter doesn't work.");
      }
      break;
    case IN_SMOKER_TALK+2:
      clear_state();
      break;
    case IN_SMOKER_TALK+3:
      break;
    case IN_SMOKER_TALK+4:
      break;
    case IN_SMOKER_TALK+5:
      break;
    case IN_SMOKER_TALK+6:
      break;
    case IN_SMOKER_TALK+7:
      break;
    case IN_SMOKER_TALK+8:
      show_speech_for(ENTITY_SMOKER, "Hey kid! That lighter you're holding, could you let me borrow it?");
      flags.gave_away_lighter = true;
      break;
    case IN_SMOKER_TALK+9:
      break;
    case IN_SMOKER_TALK+10:
      timer_wait(1.f);
      break;
    case IN_SMOKER_TALK+11:
      break;
    case IN_SMOKER_TALK+12:
      show_speech_for(ENTITY_SMOKER, "Ahhh.. That hits the spot.");
      break;
    case IN_SMOKER_TALK+13:
      break;
    case IN_SMOKER_TALK+14:
      break;
    case IN_SMOKER_TALK+15:
      break;
    case IN_SMOKER_TALK+16:
      show_speech_for(ENTITY_SMOKER, "You know kid, this was going to be my last cigarette.");
      break;
    case IN_SMOKER_TALK+17:
      break;
    case IN_SMOKER_TALK+18:
      break;
    case IN_SMOKER_TALK+19:
      break;
    case IN_SMOKER_TALK+20:
      break;
    case IN_SMOKER_TALK+21:
      show_speech_for(ENTITY_SMOKER, "I was going to retire from the force too.");
      break;
    case IN_SMOKER_TALK+22:
      break;
    case IN_SMOKER_TALK+23:
      show_speech_for(ENTITY_SMOKER, "After this last train ride, I'll go home and see my family.");
      break;
    case IN_SMOKER_TALK+24:
      break;
    case IN_SMOKER_TALK+25:
      break;
    case IN_SMOKER_TALK+26:
      show_speech_for(ENTITY_SMOKER, "Here look, a picture of my dog. Ain't she cute.");
      break;
    case IN_SMOKER_TALK+27:
      timer_wait(0.5);
      break;
    case IN_SMOKER_TALK+28:
      show_speech_for(ENTITY_SMOKER, "Anyway kid, smoking is bad for you.");
      break;
    case IN_SMOKER_TALK+29:
      break;
    case IN_SMOKER_TALK+30:
      break;
    case IN_SMOKER_TALK+31:
      show_speech_for(ENTITY_SMOKER, "Here's a bit of intel for you.");
      break;
    case IN_SMOKER_TALK+32:
      show_speech_for(ENTITY_SMOKER, "The lass who's sick between those the two cars...");
      break;
    case IN_SMOKER_TALK+33:
      show_speech_for(ENTITY_SMOKER, "She dropped something of hers earlier.");
      break;
    case IN_SMOKER_TALK+34:
      timer_wait(0.5);
      break;
    case IN_SMOKER_TALK+35:
      show_speech_for(ENTITY_SMOKER, "(Whisper) I saw a child took it.");
      break;
    case IN_SMOKER_TALK+36:
      show_speech_for(ENTITY_SMOKER, "What could a child want with something like that?");
      break;
    case IN_SMOKER_TALK+37:
      break;
    case IN_SMOKER_TALK+38:
      timer_wait(0.5);
      break;
    case IN_SMOKER_TALK+39:
      show_speech_for(ENTITY_SMOKER, "Anyways kid, take it easy!");
      break;
    case IN_SMOKER_TALK+40:
      timer_wait(0.5);
      break;
    case IN_SMOKER_TALK+41:
      clear_state();
      break;
    case IN_SMOKER_TALK+42:
      break;
    case IN_SMOKER_TALK+43:
      break;
    case IN_SMOKER_TALK+44:
      break;
    case IN_SMOKER_TALK+45:
      show_speech_for(ENTITY_SMOKER, "Don't forget, smoking is bad for you!");
      break;
    case IN_SMOKER_TALK+46:
      clear_state();
      break;
    case IN_SMOKER_TALK+47:
      break;
    case IN_SMOKER_TALK+48:
      break;
    case IN_SMOKER_TALK+49:
      break;
    case IN_SMOKER_TALK+50:
      break;
    case IN_SMOKER_TALK+51:
      break;
    case IN_SMOKER_TALK+52:
      break;
    case IN_SMOKER_TALK+53:
      break;
    case IN_SMOKER_TALK+54:
      break;
    case IN_SMOKER_TALK+55:
      break;
    case IN_SMOKER_TALK+56:
      break;
    case IN_SMOKER_TALK+57:
      break;
    case IN_SMOKER_TALK+58:
      break;
    case IN_SMOKER_TALK+59:
      break;
    case IN_SMOKER_TALK+60:
      break;
    case IN_SMOKER_TALK+61:
      break;
    case IN_SMOKER_TALK+62:
      break;
    case IN_SMOKER_TALK+63:
      break;
    case IN_SMOKER_TALK+64:
      break;
    case IN_SMOKER_TALK+65:
      break;
    case IN_SMOKER_TALK+66:
      break;
    case IN_SMOKER_TALK+67:
      break;
    case IN_SMOKER_TALK+68:
      break;
    case IN_SMOKER_TALK+69:
      break;



    case IN_LITTGIRL_SAY+0:
      break;
    case IN_LITTGIRL_SAY+1:
      if (!flags.gave_away_lighter || flags.spoke_to_little_girl) {
        set_state(IN_LITTGIRL_SAY+5);
        return;
      }
      else {
        set_state(IN_LITTGIRL_SAY + 20);
        return;
      }
      break;
    case IN_LITTGIRL_SAY+2:
      break;
    case IN_LITTGIRL_SAY+3:
      break;
    case IN_LITTGIRL_SAY+4:
      break;
    case IN_LITTGIRL_SAY+5:

      suspend_entity_type(ENTITY_LITTLE_GIRL);
      {
        const char *lines[] = {
          "One, two, buckle my shoes..",
          "three, four, lock the door..",
          "five, six, get your fix..",
          "seven, eight, don't be late..",
          "nine, ten, you're not my friend..",
        };

        const char *line = lines[flags.little_girl_rhyme_index++];
        if (flags.little_girl_rhyme_index >= _countof(lines))
          flags.little_girl_rhyme_index = 0;

        show_speech_for(ENTITY_LITTLE_GIRL, line);
      }

      break;
    case IN_LITTGIRL_SAY+6:
      unsuspend_entity_type(ENTITY_LITTLE_GIRL);
      clear_state();
      break;
    case IN_LITTGIRL_SAY+7:
      break;
    case IN_LITTGIRL_SAY+8:
      break;
    case IN_LITTGIRL_SAY+9:
      break;
    case IN_LITTGIRL_SAY+10:
      break;
    case IN_LITTGIRL_SAY+11:
      break;
    case IN_LITTGIRL_SAY+12:
      break;
    case IN_LITTGIRL_SAY+13:
      break;
    case IN_LITTGIRL_SAY+14:
      break;
    case IN_LITTGIRL_SAY+15:
      break;
    case IN_LITTGIRL_SAY+16:
      break;
    case IN_LITTGIRL_SAY+17:
      break;
    case IN_LITTGIRL_SAY+18:
      break;
    case IN_LITTGIRL_SAY+19:
      break;
    case IN_LITTGIRL_SAY+20:

      suspend_entity_type(ENTITY_LITTLE_GIRL);
      flags.spoke_to_little_girl = true;
      show_speech_for(ENTITY_LITTLE_GIRL, "Hi, mister!");
      break;
    case IN_LITTGIRL_SAY+21:
      break;
    case IN_LITTGIRL_SAY+22:
      show_speech_for(ENTITY_LITTLE_GIRL, "You want what that lady dropped?");
      break;
    case IN_LITTGIRL_SAY+23:
      break;
    case IN_LITTGIRL_SAY+24:
      show_speech_for(ENTITY_LITTLE_GIRL, "Sure mister, but you have to answer a question first.");
      break;
    case IN_LITTGIRL_SAY+25:
      break;
    case IN_LITTGIRL_SAY+26:
      show_speech_for(ENTITY_LITTLE_GIRL, "Ready? here goes.");
      break;
    case IN_LITTGIRL_SAY+27:
      timer_wait(0.7);
      break;
    case IN_LITTGIRL_SAY+28:
      break;
    case IN_LITTGIRL_SAY+29:
      {
        const char *options[] = {
          "<- East",
          "West ->",
        };
        show_speech_for_and_prompt(ENTITY_LITTLE_GIRL,
            "Which way do you think the train is going?", options, _countof(options));
      }
      break;
    case IN_LITTGIRL_SAY+30:
      break;
    case IN_LITTGIRL_SAY+31:
      show_speech_for(ENTITY_LITTLE_GIRL, "So that's what you think..");
      break;
    case IN_LITTGIRL_SAY+32:
      break;
    case IN_LITTGIRL_SAY+33:
      show_speech_for(ENTITY_LITTLE_GIRL, "Hahaha wrong! That was a trick question.");
      break;
    case IN_LITTGIRL_SAY+34:
      break;
    case IN_LITTGIRL_SAY+35:
      break;
    case IN_LITTGIRL_SAY+36:
      show_speech_for(ENTITY_LITTLE_GIRL, "We're not going east OR west!");
      break;
    case IN_LITTGIRL_SAY+37:
      break;
    case IN_LITTGIRL_SAY+38:
      break;
    case IN_LITTGIRL_SAY+39:
      show_speech_for(ENTITY_LITTLE_GIRL, "Huh? Where exactly IS it that we're going?");
      break;
    case IN_LITTGIRL_SAY+40:
      break;
    case IN_LITTGIRL_SAY+41:
      show_speech_for(ENTITY_LITTLE_GIRL, "You're funny, mister!");
      break;
    case IN_LITTGIRL_SAY+42:
      break;
    case IN_LITTGIRL_SAY+43:
      show_speech_for(ENTITY_LITTLE_GIRL, "Shouldn't you know where you want to go before getting on a train?");
      break;
    case IN_LITTGIRL_SAY+44:
      break;
    case IN_LITTGIRL_SAY+45:
      timer_wait(0.7);
      break;
    case IN_LITTGIRL_SAY+46:
      break;
    case IN_LITTGIRL_SAY+47:
      show_speech_for(ENTITY_LITTLE_GIRL, "Since you answered my question, here.");
      break;
    case IN_LITTGIRL_SAY+48:
      timer_wait(0.7);
      break;
    case IN_LITTGIRL_SAY+49:
      give_item(ITEM_PILL_BOTTLE);
      break;
    case IN_LITTGIRL_SAY+50:
      timer_wait(0.7);
      break;
    case IN_LITTGIRL_SAY+51:
      show_speech_for(ENTITY_LITTLE_GIRL, "You better give it to her soon, mister.");
      break;
    case IN_LITTGIRL_SAY+52:
      show_speech_for(ENTITY_LITTLE_GIRL, "She's getting pretty sick.");
      break;
    case IN_LITTGIRL_SAY+53:
      break;
    case IN_LITTGIRL_SAY+54:
      unsuspend_entity_type(ENTITY_LITTLE_GIRL);
      clear_state();
      break;
    case IN_LITTGIRL_SAY+55:
      break;
    case IN_LITTGIRL_SAY+56:
      break;
    case IN_LITTGIRL_SAY+57:
      break;
    case IN_LITTGIRL_SAY+58:
      break;
    case IN_LITTGIRL_SAY+59:
      break;
  }

  if (global_state != 0) {
    global_state++;
  }
}

void show_prompt(float x, float y, void (*proc)(Entity *en)) {
  prompt_x = x;
  prompt_y = y;
  prompt_proc = proc;
}

void draw_x(int x, int y) {
  if (!prompt_texture) {
    prompt_texture = load_texture("data/x.png");
  }

  if (prompt_texture) {
    gfx_push();
    gfx_set_color(1,1,1, get_wave_range(1.8, 0.5, 0.8));
    //gfx_translate(GAME_WIDTH - prompt_texture->width*2 - 10, GAME_HEIGHT - prompt_texture->height*2);
    gfx_translate(floorf(x - prompt_texture->width/2), floorf(y - prompt_texture->height/2));
    gfx_translate(0, floorf(get_wave(1) * 2));
    gfx_draw_texture(prompt_texture);
    gfx_pop();
  }
}

void draw_prompt(void) {
  if (!prompt_proc) return;
  if (global_state) return;
  draw_x(prompt_x, prompt_y);

  if (app_keypress(KEY_X)) {
    Entity *en = world_find(world, active_id);
    prompt_proc(en);
  }

  prompt_proc = 0;
  active_id = 0;
}


void shake_camera(float mag) {
  total_magnitude = mag;
}
void apply_camera_impulse(float mag) {
  float ang = randomf() * 2 * 3.1415926f;
#if 0
  camera_off.x = sinf(ang) * mag;
#else
  camera_off.x = sinf(ang) * mag;
  camera_off.y = cosf(ang) * mag;
#endif
}
void update_camera_shake(void) {
  shake_timer -= app_delta();
  if (shake_timer <= 0) {
    impulse_gap = 0.05;
    shake_timer = randomf_range(5.7, 6.75);
    shake_camera(randomf_range(1.0, 1.5));
    first_shke = true;
  }

  //move_to_2f(&camera_off, make2f(0,0), app_delta() * 30);

  if (should_shake_camera) {
    move_to(&total_magnitude , 0, app_delta() * 0.5);
    impulse_timer -= app_delta();
    if (impulse_timer <= 0) {
      //impulse_gap += 0.051;
      impulse_timer = impulse_gap;
      apply_camera_impulse(total_magnitude);

      if (first_shke) {
        first_shke = false;
        const char *anim = camera_off.x > 0 ? "swing2" : "swing";
        for (int i = 0; i < arrlen(world->entities); i++) {
          Entity *en = world->entities[i];
          if (en->type == ENTITY_RAIL_HANDLE) {
            sprite_play(en->sprite, anim, 0);
            en->sprite->rate = randomf_range(0.8, 1.3);
          }
        }
      }
    }
  }
  else {
    camera_off = {};
  }
}


void world_delete(World *world) {
  for (int i = 0; i < arrlen(world->ambient_audio_list); i++) {
    aud_delete_source(world->ambient_audio_list[i]);
  }
  arrfree(world->ambient_audio_list);
  for (int i = 0; i < arrlen(world->entities); i++) {
    entity_free(world->entities[i]);
    free(world->entities[i]);
  }
  arrfree(world->entities);
  free(world);
}
void world_update(World *world) {

  for (int i = 0; i < arrlen(world->entities);) {
    if (world->entities[i]->removed) {
      entity_free(world->entities[i]);
      free(world->entities[i]);
      arrdel(world->entities, i);
    }
    else {
      i++;
    }
  }

  for (int i = 0; i < arrlen(world->entities); i++) {
    entity_update(world->entities[i]);
  }
}
void world_draw(World *world) {
  for (int i = 0; i < arrlen(world->entities); i++) {
    entity_draw(world->entities[i]);
  }
}
Entity *world_find(World *world, int id) {
  for (int i = 0; i < arrlen(world->entities); i++) {
    if (world->entities[i]->id == id)
      return world->entities[i];
  }
  return 0;
}
Entity *world_add(World *world, int type) {
  Entity *en = make_entity(type);
  en->id = ++world->max_id;
  arrput(world->entities, en);
  return en;
}
Entity *world_add_at (World *world, int type, int x, int y) {
  Entity *en = world_add(world, type);
  if (en) {
    en->x = x;
    en->y = y;
  }
  return en;
}
Entity *world_find_first (World *world, int type) {
  for (int i = 0; i < arrlen(world->entities); i++) {
    if (world->entities[i]->type == type)
      return world->entities[i];
  }
  return 0;
}

void world_add_texture (World *world, const char *path, int x, int y){
  Entity *en = world_add(world, ENTITY_UNKNOWN);
  if (en) {
    en->x = x;
    en->y = y;
    en->texture = load_texture(path);
    if (en->texture) {
      en->width = en->texture->width;
      en->height = en->texture->height;
    }
  }
}

static void world_add_ambient_sound(World *world, const char *file) {
  Audio_Source *src = load_audio_source(file);
  if (src) {
    arrput(world->ambient_audio_list, src);
    aud_set_volume(src, ambient_current_volume);
    aud_set_source_looped(src, true);
    aud_play(src);
  }
}

enum {
  TRAIN_NORMAL,
  TRAIN_DECREPIT,
  TRAIN_WEB_0,
  TRAIN_WEB_1,
  TRAIN_WEB_2,
};

void add_train_bg_impl(World *world, int type) {
  world_add_ambient_sound(world, "data/train_ambient_interior.ogg");
  switch (type) {
    case TRAIN_NORMAL:
      {
      world_add_texture(world, "data/train.png", 0, 0);
      Entity *door = world_add_at(world, ENTITY_TRAIN_DOOR, 36, 83);
      if (door)
        door->y = FLOOR_Y - door->height;
      }
      break;
    case TRAIN_DECREPIT:
      world_add_texture(world, "data/train_decrepit.png", 0, 0);
      break;
    case TRAIN_WEB_0:
      world_add_texture(world, "data/train_webbed_0.png", 0, 0);
      break;
    case TRAIN_WEB_1:
      world_add_texture(world, "data/train_webbed_1.png", 0, 0);
      break;
    case TRAIN_WEB_2:
      world_add_texture(world, "data/train_webbed_2.png", 0, 0);
      break;
  }
}
void add_train_bg(World *world) {
  add_train_bg_impl(world, 0);
}
void add_train_fg(World *world) {
  world_add_texture(world, "data/railing.png", 15, 67);

  int y = 85;
  int x = 3;
  world_add_at(world, ENTITY_RAIL_HANDLE, x+57, y);
  world_add_at(world, ENTITY_RAIL_HANDLE, x+96, y);
  world_add_at(world, ENTITY_RAIL_HANDLE, x+166, y);
  world_add_at(world, ENTITY_RAIL_HANDLE, x+240, y);
}
void add_gap_bg(World *world) {
  world_add_ambient_sound(world, "data/train_ambient.ogg");
  world->type = WORLD_GAP;
  world_add_texture(world, "data/gap_rails.png", 151, 115);
}
void add_gap_fg(World *world) {
  world->type = WORLD_GAP;
  world_add_texture(world, "data/gap_left.png", 0, 0);
  world_add_texture(world, "data/gap_right.png", GAME_WIDTH-124, 0);
}





void scene_train_1(World *world);
void scene_gap_1(World *world);
void scene_train_2(World *world);
void scene_gap_2(World *world);
void scene_train_3(World *world);
void scene_gap_3(World *world);
void scene_train_final(World *world);
void scene_train_home(World *world);

void scene_dark_gap_0(World *world);
void scene_dark_gap_1(World *world);
void scene_dark_gap_2(World *world);
void scene_dark_gap_3(World *world);
void scene_train_dark_1(World *world);
void scene_train_web_0(World *world);
void scene_train_web_1(World *world);
void scene_train_web_2(World *world);
void scene_train_final(World *world);

void scene_train_1(World *world) {
  add_train_bg(world);

  if (!flags.homeless_guy_gave_newspaper) {
    world_add_at(world, ENTITY_HOMELESS_GUY, 211, 99);
  }

  world_add_at(world, ENTITY_PLAYER, 85, 0);

  add_train_fg(world);
  world->right_scene = scene_gap_1;


  bool skip_left = false;
#ifdef DEV_BUILD
  skip_left = true;
#endif

  if (skip_left || flags.talked_to_vomit_girl) {
    world->left_scene = scene_dark_gap_0;
  }
}

void scene_gap_1(World *world) {
  add_gap_bg(world);

  world_add(world, ENTITY_PLAYER);
  if (!flags.gave_away_lighter) {
    world_add_at(world, ENTITY_SMOKER, 171, 91);
  }

  add_gap_fg(world);

  world->left_scene = scene_train_1;
  world->right_scene = scene_train_2;
}

void scene_train_2(World *world) {
  add_train_bg(world);

  world_add(world, ENTITY_PLAYER);

  if (!flags.spoke_to_little_girl) {
    world_add(world, ENTITY_LITTLE_GIRL);
  }

  add_train_fg(world);
  world->left_scene = scene_gap_1;
  world->right_scene = scene_gap_2;
}

void scene_gap_2(World *world) {
  add_gap_bg(world);

  if (!flags.talked_to_vomit_girl) {
    world_add_at(world, ENTITY_VOMIT_GIRL, 167, 91);
  }

  world_add(world, ENTITY_PLAYER);

  add_gap_fg(world);


  world->left_scene = scene_train_2;
  world->right_scene = scene_train_3;
}

void scene_train_3(World *world) {
  add_train_bg(world);

  world_add_texture(world, "data/newspaper.png", 173, 110);
  if (!flags.got_lighter) {
    world_add_at(world, ENTITY_NEWSPAPER_GUY, 196, 71);
  }

  world_add(world, ENTITY_PLAYER);

  add_train_fg(world);
  world->left_scene = scene_gap_2;
}

void scene_dark_gap_0(World *world) {
  add_gap_bg(world);

  Entity *player = world_add_at(world, ENTITY_PLAYER, 1000, 0);
  if (player )
    player->facing = FACING_LEFT;

  add_gap_fg(world);
  world->left_scene = scene_train_dark_1;
}

void scene_train_dark_1(World *world) {
  world->ambient = { 0.f, 0.f, 0.f };

  add_train_bg(world);

  play_music("data/pulse.ogg");

  world_add(world, ENTITY_PLAYER);

  add_train_fg(world);

  world->right_scene = scene_dark_gap_0;
  world->left_scene = scene_dark_gap_1;
}

void scene_dark_gap_1(World *world) {
  add_gap_bg(world);

  world_add(world, ENTITY_PLAYER);
  add_gap_fg(world);

  world->right_scene = scene_train_dark_1;
  world->left_scene = scene_train_web_0;
}

void scene_train_web_0(World *world) {
  world->ambient = { 0.f, 0.f, 0.f };

  add_train_bg_impl(world, TRAIN_WEB_0);

  world_add(world, ENTITY_PLAYER);

  add_train_fg(world);

  world->right_scene = scene_dark_gap_1;
  world->left_scene = scene_dark_gap_2;
}

void scene_dark_gap_2(World *world) {
  add_gap_bg(world);
  world_add(world, ENTITY_PLAYER);

  add_gap_fg(world);

  world->right_scene = scene_train_web_0;
  world->left_scene = scene_train_web_1;
}

void scene_train_web_1(World *world) {
  world->ambient = { 0.f, 0.f, 0.f };

  add_train_bg_impl(world, TRAIN_WEB_1);

  world_add(world, ENTITY_PLAYER);

  add_train_fg(world);

  world->right_scene = scene_dark_gap_2;
  world->left_scene = scene_dark_gap_3;
}

void scene_dark_gap_3(World *world) {
  add_gap_bg(world);
  world_add(world, ENTITY_PLAYER);

  add_gap_fg(world);

  world->right_scene = scene_train_web_1;
  world->left_scene = scene_train_web_2;
}

void timer_fade_out(Entity *en) {
  set_state(IN_FADE_OUT);
}

void scene_train_web_2(World *world) {
  world->ambient = { 0.f, 0.f, 0.f };

  add_train_bg_impl(world, TRAIN_WEB_2);

  Entity *player = world_add_at(world, ENTITY_PLAYER, 1000, 0);
  if (player)
    player->facing = FACING_LEFT;

  add_train_fg(world);
  // Entity *t = world_add(world, ENTITY_TIMER);
  // t->timer_left = 8;
  // if (t) {
  //   t->on_over = timer_fade_out;
  // }

  //world->right_scene = scene_dark_gap_3;
}

void scene_train_final(World *world) {
  world->ambient = { 0.1f, 0.06f, 0.03f };
  //world->ambient = {};

  add_train_bg_impl(world, 0);

  float off = 30;

  Entity *player = world_add_at(world, ENTITY_PLAYER, 220 - off, 0);
  if (player)
    player->facing = FACING_LEFT;

  Entity *vg = world_add_at(world, ENTITY_VOMIT_GIRL, 195 - off, 0);
  if (vg) {
    vg->facing = FACING_RIGHT;
    vg->suspended = true;
    if (vg->sprite)
      sprite_play(vg->sprite, "idle_2", 0);

    vg->y = FLOOR_Y - vg->height;
  }

  add_train_fg(world);
}

void scene_train_home(World *world) {
  //world->ambient = { 1.0f + 18.f/255.f, 1.0f + 12.f/255.f, 1.0f + 7.f/255.f };
  //world->ambient.data[2] *= 2;
  //world->ambient.data[1] *= 2;
  //world->ambient.data[0] *= 2;

  world->ambient.data[0] += 106.f / 255.f;
  world->ambient.data[1] += 62.f / 255.f;
  world->ambient.data[2] += 26.f / 255.f;

  add_train_bg_impl(world, 0);

  Entity *player = world_add_at(world, ENTITY_PLAYER, 220, 0);
  if (player) {
    player->y = FLOOR_Y - player->height;
    player->facing = FACING_LEFT;
    player->suspended = true;
    entity_set_center_x(player, 107);
    sprite_play(player->sprite, "sit_idle", SPRITE_LOOPED);
  }

  add_train_fg(world);
}

void request_change_scene(void (*proc)(World *world)) {
  requested_scene = proc;
}

void goto_left_scene(Entity *en) {
  if (world) {


    if (world->left_scene) {
      set_state(IN_GOTO_SCENE_LEFT);
    }
    else {
      set_state(IN_WONT_OPEN);


      if (world->cur_scene == scene_train_web_2) {
        set_state(IN_ENCOUNTER);
      }


    }
  }
}

void goto_right_scene(Entity *en) {
  if (world) {


    if (world->right_scene) {
      set_state(IN_GOTO_SCENE_RIGHT);
    }
    else {
      set_state(IN_WONT_OPEN);
    }
  }
}

void change_scene(void (*proc)(World *world)) {
  Scene last_scene = 0;

  show_window_flash = true;
  flags.override_brightness = 1;
  //ambient_current_volume = 1;
  //ambient_fade_target = 1;

  if (world) {
    last_scene = world->cur_scene;
    world_delete(world);
  }

  world = NEW(World);
  world->ambient = { 1.f, 1.f, 1.f };
  if (proc) {
    proc(world);
    world->cur_scene = proc;
  }

  if (last_scene) {
    Entity *player = world_find_first(world, ENTITY_PLAYER);
    if (player) {
      if (world->right_scene == last_scene) {
        player->x = GAME_WIDTH;
        player->facing = FACING_LEFT;
      }
      else if (world->left_scene == last_scene) {
        player->x = 0;
        player->facing = FACING_RIGHT;
      }
    }
  }

  if (world->cur_scene == scene_train_final) {
    fade_ambient_sound(1, 3);
  }

  Entity *l = world_add(world, ENTITY_LIGHT);
  l->is_flashlight = true;
}


void request_reset_game(void) {
  request_reset = true;
}
static void reset_game() {
  fade_out_all_music();
  clear_items();
  flags = {};
  change_scene(FIRST_SCENE);
  //change_scene(scene_train_1);
  //change_scene(scene_train_final);
  //change_scene(scene_train_home);

  fade_ambient_sound(1, 1);
  set_state(IN_FADE_IN);

  should_shake_camera = true;
  next_window_flash_timer = 3;
  flags.override_brightness = 1;
  show_window_flash = true;
  showing_ending_text = false;
  scene_change_fadein_duration_override = 0;
  scene_change_fadein_wait_override = 0;
}

static void load(void) {
  init_loader();

  aud_load();

  big_font = load_image_font("data/font3.png", " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:-_\"'?!()[]<>/", 1);
  speech_font = load_image_font("data/font.png", " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,\"'()[]<>-_!?/", 1);

  load_lighting();
  canvas = gfx_make_canvas(GAME_WIDTH, GAME_HEIGHT);
  world_canvas = gfx_make_canvas(GAME_WIDTH, GAME_HEIGHT);
  //change_scene(scene_gap_1);
  //change_scene(scene_gap_2);

  reset_game();
}
static void frame(void) {
  if (app_keypress(KEY_ESCAPE)) app_quit();

  if (request_reset) {
    reset_game();
    request_reset = false;
  }

  update_ambient_sound();
  update_music();

  update_state();


  if (world->cur_scene == scene_train_final && !flags.shown_final_convo && !global_state) {
    flags.shown_final_convo = true;
    set_state(IN_GOTO_FINAL_CONVERSATION);
  }

  if (world_is_dark(world)) {
    if (!flags.flash_first_on) {
      if (!global_state) {
        if (!flags.flash_first_on) {
          set_state(IN_FIRST_FLASHLIGHT);
          flags.flash_first_on = true;
        }
      }
    }
    else {
      flashlight_on = true;
    }
  }
  else {
    flashlight_on = false;
  }
  timer_update();

  if (requested_scene) {
    change_scene(requested_scene);
    requested_scene = 0;
  }

  update_camera_shake();

  world_update(world);
  Entity *player = world_find_first(world, ENTITY_PLAYER);
  if (player) {
    float left = TRAIN_LEFT;
    float right = TRAIN_RIGHT;

    if (world->type == WORLD_GAP) {
      left = GAP_LEFT;
      right = GAP_RIGHT;
    }

    if (player->x < left) {
      player->x = left;
    }
    else if (player->x > right-player->width) {
      player->x = right-player->width;
    }

    if (!global_state) {
      if (player->x <= left + 2 && player->facing == FACING_LEFT) {
        show_prompt(left, 80, goto_left_scene);
      }
      else if (player->x+player->width >= right - 2 && player->facing == FACING_RIGHT) {
        show_prompt(right, 80, goto_right_scene);
      }

      for (int i = 0; i < arrlen(world->entities); i++) {
        Entity *en = world->entities[i];
        if (en == player) continue;
        if (!en->on_use) continue;
        float px = player->x+player->width/2;
        if (px >= en->x && px <= en->x+en->width) {
          active_id = en->id;
          show_prompt(floorf(en->x+en->width/2), en->y, en->on_use);
          break;
        }
      }
    }

    if (!global_state && world->cur_scene == scene_train_final) {
      static float spin_timer = 0;
      static int spin_count = 0;
      static int first_spin_dir = -1;
      static int last_spin_dir = 0;
      const float SPIN_GAP = 0.5;

      int last_spin_count = spin_count;

      if (app_keypress(KEY_LEFT)) {
        last_spin_dir = KEY_RIGHT;
        if (first_spin_dir == -1)
          first_spin_dir = KEY_LEFT;

        if (first_spin_dir != KEY_LEFT) {
          spin_count++;
        }
        spin_timer = 0;
      }
      else if (app_keypress(KEY_RIGHT)) {
        last_spin_dir = KEY_LEFT;
        if (first_spin_dir == -1)
          first_spin_dir = KEY_RIGHT;

        if (first_spin_dir != KEY_RIGHT) {
          spin_count++;
        }
        spin_timer = 0;
      }
      else {
        spin_timer += app_delta();
        if (spin_timer >= SPIN_GAP && spin_timer <= SPIN_GAP * 2) {
          spin_timer = 0;
          spin_count = 0;
          first_spin_dir = -1;
        }
      }

      if (spin_count >= 7 && last_spin_count != spin_count) {
        spin_count = 0;
        set_state(IN_GO_HOME);
      }
    }
  }

  if (world->cur_scene == scene_train_web_2) {
    if (!global_state) {
      //set_state(IN_FADE_OUT);
    }
  }

  gfx_set_canvas(world_canvas);
  {
    if (world->type == WORLD_TRAIN)
      gfx_clear(0,0,0,0);
    else if (world->type == WORLD_GAP)
      gfx_clear(0,0,0,1);

    gfx_push();
    gfx_translate(floorf(camera_off.x), floorf(camera_off.y));
    gfx_set_color(1,1,1,1);
    world_draw(world);
    gfx_pop();
  }
  gfx_set_canvas(0);

  gfx_set_canvas(canvas);

  {
    float lighting_brightness = 1;
    if (world->type == WORLD_TRAIN) {

      gfx_set_color(0,0,0,1);
      gfx_fill_rect(31, 79, 274, 52);
      gfx_fill_rect(-15, -15, GAME_WIDTH + 30, 30);
      gfx_fill_rect(-15, GAME_HEIGHT-15, GAME_WIDTH + 30, 30);
      gfx_fill_rect(-15, -15, 30, GAME_HEIGHT+30);
      gfx_fill_rect(GAME_WIDTH-15, -15, 30, GAME_HEIGHT+30);

      if (world_is_dark(world)) {

        next_window_flash_timer -= app_delta();
        if (next_window_flash_timer <= 0) {
          next_window_flash_timer = 2;
          window_flash = 1.f;
        }

        if (window_flash > 0) {
          float t = (1.f - window_flash);
          t = cosf((t-0.5f) * 2 * 3.1415926f) * 0.5 + 0.5f;
          t = 1;

          lighting_brightness = 1-t;

          gfx_set_color(127.f/255,132.f/255,105.f/255,1.f * t);

          if (show_window_flash)
            gfx_fill_rect(31, 79, 274, 52);

          move_to(&window_flash, 0, app_delta() / 0.2f);
        }
      }
      else if (world->cur_scene == scene_train_home) {
        gfx_set_color(1,1,1,1);
        gfx_fill_rect(31, 79, 274, 52);
      }
    }

    for (int i = 0; i < arrlen(world->entities); i++) {
      Entity *en = world->entities[i];
      if (en->type == ENTITY_LIGHT) {
        add_light_to_lighting(en->x+en->width/2, en->y+en->height/2, en->light_radius,
            en->color.x, en->color.y, en->color.z, en->color.w);
      }
    }

    lighting_brightness *= flags.override_brightness;
    gfx_set_color(lighting_brightness, lighting_brightness, lighting_brightness, 1);
    draw_lighting(&world_canvas->texture, world->ambient );
  }

  gfx_push();
  //gfx_translate(floorf(camera_off.x), floorf(camera_off.y));
  draw_prompt();
  gfx_pop();

  curtain_draw(&curtain);
  draw_items();
  draw_speech();
  draw_notifications();
  draw_ending_text();

  gfx_set_canvas(0);

  gfx_push();
  gfx_set_color(1,1,1,1);
  gfx_scale(GAME_SCALE, GAME_SCALE);
  gfx_draw_texture(&canvas->texture);
  gfx_pop();
}

Entity *make_entity(int type) {
  Entity *en = NEW(Entity);
  en->width = 10;
  en->height = 10;
  en->type = type;
  en->color = {1.f, 1.f, 1.f, 1.f};
  call_entity_proc(en, ACTION_MAKE);
  return en;
}

#if !defined(_WIN32) || defined(DEV_BUILD)
int main(void)
#else
#include <windows.h>
int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nShowCmd
)
#endif
{
  App_Config c = {0};
  c.width = GAME_WIDTH * GAME_SCALE;
  c.height = GAME_HEIGHT * GAME_SCALE;
  c.step = frame;
  c.load = load;
  app_start(&c);
  return 0;
}

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

