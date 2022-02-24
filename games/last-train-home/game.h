#pragma once

#include "core.h"
#include "sprite.h"
#include "stb/stb_ds.h"
#include "loader.h"
#include "font.h"

#define NEW(T) ((T *)calloc(1,sizeof(T)))
#ifdef __cplusplus
#define MAKE(T) (T{})
#else
#define MAKE(T) ((T){0})
#endif

#define GAME_WIDTH 320
#define GAME_HEIGHT 180
#define GAME_SCALE 4
#define FLOOR_Y 131
#define TRAIN_LEFT 7
#define TRAIN_RIGHT 309
#define GAP_LEFT (140-8)
#define GAP_RIGHT (208+8)

enum {
  ITEM_LIGHTER,
  ITEM_NEWSPAPER,
  ITEM_PILL_BOTTLE,
  ITEM_,
};

enum {
  WORLD_TRAIN,
  WORLD_GAP,
};

enum {
  FACING_RIGHT,
  FACING_LEFT,
};
enum {
  ACTION_MAKE,
  ACTION_UPDATE,
  ACTION_DRAW,
};
enum {
  ENTITY_UNKNOWN,
  ENTITY_PLAYER,
  ENTITY_DOOR,
  ENTITY_TRAIN_BG,
  ENTITY_NEWSPAPER_GUY,
  ENTITY_VOMIT_GIRL,
  ENTITY_HOMELESS_GUY,
  ENTITY_SMOKER,
  ENTITY_LITTLE_GIRL,
  ENTITY_LIGHT,
  ENTITY_TIMER,
  ENTITY_RAIL_HANDLE,
  ENTITY_EVIL_LITTLE_GIRL,
  ENTITY_SPIDER,
  ENTITY_TRAIN_DOOR,
};

enum {
  IN_NEWSPAPER_GUY_SAY = 100,
  IN_NEWSPAPER_GUY_REMIND = 200,
  IN_FADE_IN = 300,
  IN_GOTO_SCENE_LEFT = 400,
  IN_GOTO_SCENE_RIGHT = 500,
  IN_VOMIT_GIRL_TALK = 600,
  IN_WONT_OPEN = 700,
  IN_HOMELESS_GUY_TALK = 800,
  IN_SMOKER_TALK = 900,
  IN_LITTGIRL_SAY = 1000,
  IN_FIRST_FLASHLIGHT = 1200,
  IN_FADE_OUT = 1300,
  IN_ENCOUNTER = 1400,
  IN_GOTO_FINAL_CONVERSATION = 1600,
  IN_GO_HOME = 2000,
  IN_EPILOG = 3000,
};

typedef struct Entity Entity;
typedef void On_Use(Entity *en);

#define ENTITY_BASE union { Entity; Entity base; }
struct Entity {
  int id;
  int state;
  int type;
  int facing;
  union { Vec2 pos; struct {float x,y;}; };
  union { Vec2 last_pos; struct {float last_x,last_y;}; };
  union { Vec2 size; struct {float width,height;}; };
  Texture *texture;
  Sprite *sprite;
  Vec4 color;

  On_Use *on_use;
  bool suspended;
  bool removed;

  float light_radius;
  bool is_flashlight;
  On_Use *on_over;
  float timer_left;
};

typedef struct World {
  Entity **entities;
  int max_id;

  int type;
  void (*cur_scene)(struct World *world);
  void (*left_scene)(struct World *world);
  void (*right_scene)(struct World *world);
  struct Audio_Source **ambient_audio_list;

  Vec3 ambient;
  Vec3 target_ambient;
  Vec3 start_ambient;

} World;

typedef struct Curtain {
  float color[4];
  float target[4];
  float start[4];
  float time_left;
  float time_total;
  float brightness;
} Curtain;

typedef struct Item {
  int type;
  int quantity;
  const char *name;
  struct Texture *texture;
} Item;

typedef struct Notification {
  char *content;
  float time_left;
  float time_total;
} Notification;

typedef struct Flags {
  bool talked_to_newspaper_guy;
  bool gave_old_guy_newspaper;
  bool homeless_guy_gave_newspaper;
  bool talked_to_homeless_guy;
  bool got_lighter;
  bool gave_away_lighter;
  int little_girl_rhyme_index;
  bool spoke_to_little_girl;
  bool talked_to_vomit_girl;
  bool flash_first_on;
  float override_brightness;
  bool shown_final_convo;
} Flags;

typedef struct Music {
  const char *path;
  struct Audio_Source *source;
  bool fade_out;
  float volume;
} Music;

extern Flags flags;
extern World *world;
extern bool flashlight_on;

Entity *make_entity(int type);
void call_entity_proc(Entity *en, int action);
void entity_free(Entity *en);
void entity_set_center_x(Entity *en, float x);

void change_scene(void (*proc)(World *world));
void world_add_texture (World *world, const char *path, int x, int y);
Entity *world_add(World *world, int type);
Entity *world_add_at (World *world, int type, int x, int y);
Entity *world_find_first (World *world, int type);
void world_add_texture (World *world, const char *path, int x, int y);
Entity *world_find(World *world, int id);

void request_change_scene(void (*proc)(World *world));

typedef void (*Scene)(World *world);
void entity_draw(Entity *en);
void entity_update(Entity *en);
float randomf(void);
unsigned randomu();

void draw_x(int x, int y);

bool is_showing_speech(void);
void draw_speech(void);
void show_speech_for(int type, const char *content);

void set_state(int state);
extern int global_state;

void curtain_set(Curtain *c, float r, float g, float b, float a);
void curtain_fade(Curtain *c, float r, float g, float b, float a, float time_left);
void curtain_draw(Curtain *c);
bool curtain_is_fading(Curtain *c);

void play_sound(const char *path, float volume);
void play_footstep(void);

void notify(const char *content);
void draw_speech_box(const char *content, int x, int y);

bool has_item(int type);
void take_away_item(int type);

void suspend_entity_type(int type);
void unsuspend_entity_type(int type);

void load_lighting(void);
void add_light_to_lighting(float x, float y, float radius, float r, float g, float b, float a);
void draw_lighting(Texture *texture, Vec3 in_ambient);

float get_wave(float period);
void move_to(float *x, float tx, float delta);

bool world_is_dark(World *world);

bool is_music_playing(const char *path);
void play_music(const char *path);

void request_reset_game(void);

