#include "core.h"

#define NEW(T) ((T *)calloc(1, sizeof(T)))

static bool keys_down[KEY_MAX_];
static bool keys_delta[KEY_MAX_];
static bool quit_flag;
static int width;
static int height;
static double last_time;
static float delta;

bool app_keypress(int key) { return keys_down[key] && keys_delta[key]; }
bool app_keyrelease(int key) { return !keys_down[key] && keys_delta[key]; }
bool app_keydown(int key) { return keys_down[key]; }
void app_quit(void) { quit_flag = true; }
float app_delta(void) { 
#ifdef DEV_BUILD
  if (app_keydown(KEY_LSHIFT)) {
    return delta * 5;
  }
#endif
  return delta; 
}

#include <stdio.h>
#include <stdbool.h>

static void on_keydown(int key) {
  if (key) {
    keys_down[key] = true;
    keys_delta[key] = true;
  }
}

static void on_keyup(int key) {
  if (key) {
    keys_down[key] = false;
    keys_delta[key] = true;
  }
}

static void reset_input(void) {
  memset(keys_delta, 0, sizeof(keys_delta));
}

void gfx_load(void);

//-- Windows -----------------------------------------------------------

#ifdef _WIN32

#include <windows.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "gdi32")
#pragma comment( lib, "opengl32")

static double frequency;
double app_time(void) {
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return (double)counter.QuadPart / frequency;
}

int win32_translate_key(int syskey) {
  if (syskey >= 0x41 && syskey <= 0x5A) return syskey - 0x41 + KEY_A;
  if (syskey >= 0x30 && syskey <= 0x39) return syskey - 0x30 + KEY_A;
  switch (syskey) {
  case VK_MENU: return KEY_LALT;
  case VK_ESCAPE: return KEY_ESCAPE;
  case VK_LEFT: return KEY_LEFT;
  case VK_RIGHT: return KEY_RIGHT;
  case VK_UP: return KEY_UP;
  case VK_DOWN: return KEY_DOWN;
  case VK_SHIFT: return KEY_LSHIFT;
  }
  return 0;
}

static
LRESULT CALLBACK win32_window_proc(
  _In_ HWND   hwnd,
  _In_ UINT   uMsg,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
) {
  switch (uMsg) {
    case WM_QUIT:
    case WM_DESTROY:
      quit_flag = true;
      break;

    case WM_LBUTTONDOWN: on_keydown(KEY_MOUSE_LEFT); break;
    case WM_LBUTTONUP: on_keyup(KEY_MOUSE_LEFT); break;
    case WM_RBUTTONDOWN: on_keydown(KEY_MOUSE_RIGHT); break;
    case WM_RBUTTONUP: on_keyup(KEY_MOUSE_RIGHT); break;
    case WM_MBUTTONDOWN: on_keydown(KEY_MOUSE_MIDDLE); break;
    case WM_MBUTTONUP: on_keyup(KEY_MOUSE_MIDDLE); break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
      int key = win32_translate_key(wParam);
      if (key)
        on_keyup(key);
    } break;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
      int key = win32_translate_key(wParam);
      if (key && !(lParam & 0x40000000))
        on_keydown(key);
      if (key == VK_MENU)
        return 0;
    } break;

    case WM_CHAR:
    {
      int code = wParam;
    } break;
  }
  return DefWindowProcA(hwnd,uMsg,wParam,lParam);
}

static
void init_gl(HDC dc) {
  if (dc != NULL) {
    PIXELFORMATDESCRIPTOR pixelFormat = {0};
    pixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixelFormat.nVersion = 1;
    pixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixelFormat.iPixelType = PFD_TYPE_RGBA;
    pixelFormat.cColorBits = 32;
    pixelFormat.cAlphaBits = 0;
    pixelFormat.cAccumBits = 0;
    pixelFormat.cDepthBits = 24;
    pixelFormat.cStencilBits = 8;
    pixelFormat.cAuxBuffers = 0;
    pixelFormat.iLayerType = PFD_MAIN_PLANE;
    int iPixelFormat = ChoosePixelFormat(dc, &pixelFormat);
    SetPixelFormat(dc, iPixelFormat, &pixelFormat);

    HGLRC glContext = wglCreateContext(dc);
    wglMakeCurrent(dc, glContext);

    { // Specify the required opengl version. Amd drivers sometimes need this.
      typedef HGLRC (__stdcall *wglCreateContextAttribsARB_t)(HDC hDC, HGLRC hShareContext, const int *attribList);
      wglCreateContextAttribsARB_t wglCreateContextAttribsARB = 0;
      wglCreateContextAttribsARB = (wglCreateContextAttribsARB_t)wglGetProcAddress("wglCreateContextAttribsARB");
      if (wglCreateContextAttribsARB) {
        static const int attribs[5] = {
          0x2091 /* major version */, 3,
          0x2092 /* minor version */, 0,
          0,
        };
        HGLRC new_ctx = wglCreateContextAttribsARB(dc, 0, attribs);
        if (new_ctx) {
          wglDeleteContext(glContext);
          wglMakeCurrent(dc, new_ctx);
          glContext = new_ctx;
        }
      }
    }

    { // Set swap interval
      typedef BOOL(APIENTRY wglSwapInterval_t)(int interval);
      wglSwapInterval_t *wglSwapInterval = (wglSwapInterval_t *)wglGetProcAddress("wglSwapIntervalEXT");
      if (wglSwapInterval) {
        wglSwapInterval(0);
      }
    }
  }
  else {
    assert(false && "Cannot initialize opengl.");
  }
}

void app_start(App_Config *conf) {

  WCHAR executable_path[MAX_PATH];
  GetModuleFileNameW(NULL, executable_path, MAX_PATH);
  for (int i = (int)wcslen(executable_path) - 1; i >= 0; i--) {
    if (executable_path[i] == '/' || executable_path[i] == '\\') {
      executable_path[i] = 0;
      break;
    }
  }
  if (!SetCurrentDirectoryW(executable_path)) {
    printf("Warning, could not set working directory.\n");
  }

  LARGE_INTEGER win32_frequency;
  QueryPerformanceFrequency(&win32_frequency);
  frequency = (double)win32_frequency.QuadPart;

  width = conf->width ? conf->width : 800;
  height = conf->height ? conf->height : 600;

  const char *name = conf->title ? conf->title : "untitled";

  HMODULE hInstance = GetModuleHandle(0);

  WNDCLASSA window_class = {0};
  window_class.style         = CS_DBLCLKS | CS_PARENTDC;
  window_class.lpfnWndProc   = win32_window_proc;
  window_class.cbClsExtra    = 0;
  window_class.cbWndExtra    = 0;
  window_class.hInstance     = hInstance;
  window_class.hIcon         = NULL;
  window_class.hCursor       = LoadCursor(NULL, (LPTSTR)IDC_ARROW);
  window_class.hbrBackground = NULL;
  window_class.lpszMenuName  = NULL;
  window_class.lpszClassName = name;

  RegisterClassA(&window_class);

  UINT style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;

  RECT window_rect = {0};
  window_rect.left = 0;
  window_rect.top = 0;
  window_rect.right  = width;
  window_rect.bottom = height;
  AdjustWindowRect(&window_rect,style,0);

  int window_width = window_rect.right - window_rect.left;
  int window_height = window_rect.bottom - window_rect.top;

  int x = GetSystemMetrics(SM_CXSCREEN)/2 - window_width/2;
  int y = GetSystemMetrics(SM_CYSCREEN)/2 - window_height/2;

  HWND window = CreateWindowA(name,name,style,
      x, y, window_width, window_height,
      0,0,hInstance,0);
  ShowWindow(window, SW_SHOW);

  HDC dc = GetDC(window);
  init_gl(dc);

  gfx_width = width;
  gfx_height = height;
  gfx_load();

  if (conf && conf->load)
    conf->load();

  last_time = app_time();
  while (!quit_flag) {
    reset_input();


    double now = app_time();
    delta = (float)(now - last_time);
    last_time = now;

    MSG msg = { 0 };
    while (PeekMessageA (&msg, window, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
    }

    if (conf && conf->step)
      conf->step();

    SwapBuffers(dc);
  }
}

//-- Emscripten -----------------------------------------------------------

#elif defined(__EMSCRIPTEN__)

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

double app_time(void) {
  return emscripten_get_now() / 1000.0;
}

void (*main_frame)(void);
static void em_main_loop(void) {

  double now = app_time();
  delta = (float)(now - last_time);
  last_time = now;

  if (main_frame)
    main_frame();

  reset_input();
}

int em_translate_key(int key) {
  if (key >= 65 && key <= 65+'z'-'a') {
    return ((key-65) + KEY_A);
  }
  if (key >= 48 && key <= 48+'0'-'9') {
    return ((key-48) + KEY_0);
  }
  switch (key) {
    case 37: return KEY_LEFT;
    case 39: return KEY_RIGHT;
    case 38: return KEY_UP;
    case 40: return KEY_DOWN;
    case 16: return KEY_LSHIFT;
    //case 32: return KEY_SPACE;
    //case 189: return KEY_MINUS;
    //case 187: return KEY_EQUAL;
  }
  return 0;
}

EM_BOOL em_key_down(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData) {
  if (!keyEvent->repeat) {
    int k = em_translate_key(keyEvent->keyCode);
    if (k) {
      on_keydown(k);
    }
  }
  return true;
}
EM_BOOL em_key_up(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData) {
  int k = em_translate_key(keyEvent->keyCode);
  if (k) {
    on_keyup(k);
  }
  return true;
}

void app_start(App_Config *conf) {
  width = conf->width ? conf->width : 800;
  height = conf->height ? conf->height : 600;

  const char *id = "#canvas";

  emscripten_set_canvas_element_size(id, width, height);

  {
    EmscriptenWebGLContextAttributes attr;
    attr.alpha = EM_TRUE;
    attr.depth = EM_TRUE;
    attr.stencil = EM_FALSE;
    attr.antialias = EM_TRUE;
    attr.premultipliedAlpha = EM_TRUE;
    attr.preserveDrawingBuffer = EM_TRUE;
    attr.preferLowPowerToHighPerformance = EM_TRUE;
    attr.failIfMajorPerformanceCaveat = EM_FALSE;
    attr.majorVersion = 1;
    attr.minorVersion = 0;
    attr.enableExtensionsByDefault = EM_FALSE;
    int ctx = emscripten_webgl_create_context(id, &attr);
    emscripten_webgl_make_context_current(ctx);
    //emscripten_set_resize_callback(0, 0, EM_TRUE, &hb_on_resize_);
  }

  emscripten_set_keydown_callback("#window", NULL, true, &em_key_down);
  emscripten_set_keyup_callback("#window", NULL, true, &em_key_up);

  gfx_width = width;
  gfx_height = height;
  gfx_load();

  if (conf->load)
    conf->load();

  main_frame = conf->step;

  emscripten_set_main_loop(em_main_loop, 0, true);
}

#endif

