#include "core.h"
#include "font.h"
#include "stb/stb_ds.h"

static Font_Glyph *font_find_glyph(Font *font, int c) {
  for (int i = 0; i < arrlen(font->glyphs); i++)
    if (font->glyphs[i].code == c)
      return &font->glyphs[i];
  return NULL;
}

void font_load_image(Font *font, const unsigned char *pixels, int w, int h, const char *characters, int spacing) {
  font->scale = 1;
  int last_left = 0;
  const char *c = NULL;
  for (int x = 0; x < w; x++) {
    if (c && !*c)
      break;

    unsigned char r = pixels[x*4 + 0];
    unsigned char g = pixels[x*4 + 1];
    unsigned char b = pixels[x*4 + 2];
    unsigned char a = pixels[x*4 + 3];

    if (r == 255 && g == 0 && b == 255 && a == 255) {
      if (c) {
        Font_Glyph g = {0};
        g.code = *c;
        g.quad[0] = (float)last_left+1;
        g.quad[1] = 1;
        g.quad[2] = (float)x - (float)last_left - 1;
        g.quad[3] = (float)h-1;
        arrput(font->glyphs, g);
        c++;
      }
      else {
        c = characters;
      }
      last_left = x;
    }
  }

  font->spacing = spacing;
  font->texture = gfx_make_texture(pixels, w, h);
}

int font_height(Font *font) {
  return (int)(font->texture->height-1) * (int)font->scale;
}

void font_draw(Font *font, const char *text, float x, float y) {
  const char *c = text;
  gfx_push();
  gfx_translate(x, y);
  gfx_scale(font->scale, font->scale);
  while (*c) {
    Font_Glyph *g = font_find_glyph(font, *c);
    if (g) {
      gfx_draw_textureq(font->texture, g->quad);
      gfx_translate(g->quad[2] + font->spacing, 0);
    }
    c++;
  }
  gfx_pop();
}

int find_next_line_break(Font *font, const char *text, int limit) {
  int size = 0;
  const char *c = text;
  int index = 0;
  int last_word_begin = -1;
  for (; *c; c++) {
    Font_Glyph *g = font_find_glyph(font, *c);
    float w = 0;
    if (g) {
      w = (g->quad[2] + font->spacing) * font->scale;
    }

    bool was_space = c > text && *(c-1) == ' ';
    if (*c != ' ') {
      if (size + w > limit && last_word_begin != -1)
        return last_word_begin;

      if (*c != ' ' && was_space)
        last_word_begin = index;
    }


    size += w;
    index++;
  }

  return c - text;
}

int font_width(Font *font, const char *text) {
  float width = 0;
  const char *c = text;
  while (*c) {
    Font_Glyph *g = font_find_glyph(font, *c);
    if (g) {
      width += (g->quad[2] + font->spacing) * font->scale;
    }
    c++;
  }
  return (int)width;
}

