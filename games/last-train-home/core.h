#pragma once

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

// #define DEV_BUILD

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

typedef size_t uintptr;

#ifndef min
#define min(a, b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a, b) ((a)>(b)?(a):(b))
#endif
#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define lerp(a, b, t) (a + t*(b-a))
#ifndef _countof
#define _countof(a) array_count(a)
#endif

//==================== App ======================

enum {
  KEY_UNKNOWN,

  KEY_MOUSE_LEFT,
  KEY_MOUSE_RIGHT,
  KEY_MOUSE_MIDDLE,

  KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G,
  KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N,
  KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U,
  KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,

  KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6,
  KEY_7, KEY_8, KEY_9,

  KEY_ESCAPE, KEY_RETURN, KEY_LCTRL, KEY_LALT,
  KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN,
  KEY_LSHIFT,

  KEY_MAX_
};

typedef struct App_Config {
  const char *title;
  int width;
  int height;
  void (*step)(void);
  void (*load)(void);
} App_Config;

bool app_keypress(int key);
bool app_keyrelease(int key);
bool app_keydown(int key);
void app_quit(void);
float app_delta(void);
double app_time(void);
void app_start(App_Config *conf);

//==================== Graphics ======================

enum {
  GFX_UNIFORM_FLOAT,
  GFX_UNIFORM_FLOAT2,
  GFX_UNIFORM_FLOAT3,
  GFX_UNIFORM_FLOAT4,
  //GFX_UNIFORM_UINT,
  GFX_UNIFORM_INT,
};

enum {
  GFX_BLEND_ALPHA,
  GFX_BLEND_MULTIPLY,
};

typedef struct Shader {
  uintptr handle;
} Shader;

typedef struct Texture {
  uintptr handle;
  int width;
  int height;
} Texture;

typedef struct Canvas {
  uintptr handle;
  int width;
  int height;
  Texture texture;
} Canvas;

extern int gfx_width;
extern int gfx_height;
void gfx_line_trace(float x0, float y0, float x1, float y1);
void gfx_set_color(float r, float g, float b, float a);
void gfx_get_color(float out_color[4]);
void gfx_imm_begin(void);
void gfx_imm_vertex(float x, float y, float u, float v, u32 color);
void gfx_imm_end(void);
void gfx_push(void);
void gfx_pop(void);
void gfx_translate(float tx, float ty);
void gfx_scale(float sx, float sy);
void gfx_rotate(float r);
void gfx_clear(float r, float g, float b, float a);
void gfx_line(float x0, float y0, float x1, float y1);
void gfx_trace_rect(float x, float y, float w, float h);
void gfx_fill_rect(float x, float y, float w, float h);
void gfx_set_canvas(Canvas* canvas);
void gfx_shader_send(const char *name, int type, const void *data, size_t count);
void gfx_draw_texture(Texture *t);
void gfx_draw_textureq(Texture *t, float quad[4]);
void gfx_set_shader(Shader *shader);

Canvas* gfx_make_canvas(int width, int height);
Shader *gfx_make_shader(const char *src, size_t size);
Texture *gfx_make_texture(const void *data, int width, int height);
void gfx_delete_texture(Texture *texture);

// Math
typedef struct Vec2 {
  union {
    struct { float x, y; };
    float data[2];
  };
} Vec2;
inline Vec2 make2f(float x, float y) { Vec2 v = {x,y}; return v; }
inline float size2f(Vec2 v) { return sqrtf(v.x*v.x + v.y*v.y); }
inline Vec2 norm2f(Vec2 v) { float size = size2f(v); Vec2 r = { v.x/size,v.y/size }; return r; }
inline Vec2 div2f(Vec2 v, Vec2 w) { Vec2 r = {v.x/w.x, v.y/w.y}; return r;}

typedef struct Vec3 {
  union {
    struct { float x, y, z; };
    float data[3];
  };
} Vec3;

typedef struct Vec4 {
  union {
    struct { float x, y, z, w; };
    float data[4];
  };
} Vec4;

// String
char *str_copy(const char *str);
char *str_copy_part(const char *str, size_t size);
int str_eq(const char *a, const char *b);
int str_ends_with(const char *str, const char *sub);
char *mprintf(const char *fmt, ...);


