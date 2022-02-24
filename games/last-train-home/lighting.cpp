#include "game.h"

#include "game.h"
#include "core.h"

#include <stdint.h>
#include <math.h>

static int32_t dither_buffer[64] = {
  0,  32, 8,  40, 2,  34, 10, 42, /* 8x8 Bayer ordered dithering */
  48, 16, 56, 24, 50, 18, 58, 26, /* pattern. Each input pixel */
  12, 44, 4,  36, 14, 46, 6,  38, /* is scaled to the 0..63 range */
  60, 28, 52, 20, 62, 30, 54, 22, /* before looking in this table */
  3,  35, 11, 43, 1,  33, 9,  41, /* to determine the action. */
  51, 19, 59, 27, 49, 17, 57, 25,
  15, 47, 7,  39, 13, 45, 5,  37,
  63, 31, 55, 23, 61, 29, 53, 21
};

#define MAX_LIGHTS 10
static int num_lights;
static Vec2 positions[MAX_LIGHTS];
static Vec4 colors[MAX_LIGHTS];
static float radii[MAX_LIGHTS];
static Shader *shader;

void load_lighting(void) {
  int size = 0;
  char *source = load_file("data/lighting.glsl", &size);
  if (!source)
    return;

#ifdef _WIN32
  {
    char *old_source = source;
    source = mprintf("#version 130\n%s\n", old_source);
    size = strlen(source);
    free(old_source);
  }
#endif
  shader = gfx_make_shader(source, size);
  free(source);
}

void add_light_to_lighting(float x, float y, float radius, float r, float g, float b, float a) {
  if (num_lights >= MAX_LIGHTS)
    return;
  int i = num_lights++;
  positions[i] = {floorf(x),floorf(y)};
  colors[i] = {r,g,b,a};
  radii[i] = radius;
}

void draw_lighting(Texture *texture, Vec3 in_ambient) {
  if (!shader)
    return;

  for (int i = num_lights; i < MAX_LIGHTS; i++) {
    colors[i].w = 0;
  }
  for (int i = 0; i < num_lights; i++) {
    positions[i].x = floorf(positions[i].x);
    positions[i].y = floorf(positions[i].y);
  }

  Vec2 cam_pos = {0};
  Vec3 ambient = in_ambient;
  Vec2 screen_size = { (float)texture->width, (float)texture->height };

  gfx_set_shader(shader);
  {
    gfx_shader_send("colors",        GFX_UNIFORM_FLOAT4,  colors, MAX_LIGHTS);
    gfx_shader_send("positions",     GFX_UNIFORM_FLOAT2,  positions, MAX_LIGHTS);
    gfx_shader_send("radii",         GFX_UNIFORM_FLOAT,   radii, MAX_LIGHTS);
    gfx_shader_send("camera_pos",    GFX_UNIFORM_FLOAT2, &cam_pos, 1);
    gfx_shader_send("dither_buffer", GFX_UNIFORM_INT,    dither_buffer, 64);
    gfx_shader_send("ambient",       GFX_UNIFORM_FLOAT3, &ambient, 1);
    gfx_shader_send("screen_size",   GFX_UNIFORM_FLOAT2,  &screen_size, 1);
  }

  gfx_draw_texture(texture);
  gfx_set_shader(0);

  num_lights = 0;
}


