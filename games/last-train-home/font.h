#pragma once

typedef struct Font_Glyph {
  int code;
  float quad[4];
} Font_Glyph;

typedef struct Font {
  Font_Glyph *glyphs;
  Texture *texture;
  float scale;
  int spacing;
} Font;

void font_load_image(Font *font, const unsigned char *pixels, int width, int height, const char *glyphs, int spacing);
int font_height(Font *font);
int font_width(Font *font, const char *text);
int font_sub_width(Font *font, const char *text);
void font_draw(Font *font, const char *text, float x, float y);
char **font_format_lines(Font *font, const char *text, int limit);
int find_next_line_break(Font *font, const char *text, int limit);
