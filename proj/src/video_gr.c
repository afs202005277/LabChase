#include <lcom/lcf.h>
#include <lcom/video_gr.h>

#include "video_gr_gameAPI.h"
#include "video_gr_macros.h"

#include "XPMs/DarkModeGame.xpm"
#include "XPMs/LightModeGame.xpm"
#include "auxiliary_data_structures.h"

static void *video_mem;
static unsigned h_res;
static unsigned v_res;
static unsigned bytes_per_pixel;

static struct PlayerPosition me, other;

struct mousePos mouse;

unsigned get_h_res() {
  return h_res;
}

unsigned get_v_res() {
  return v_res;
}

void *get_video_mem() {
  return video_mem;
}

unsigned get_bits_per_pixel() {
  return bytes_per_pixel * 8;
}

int(set_mode)(uint16_t mode) {
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));
  reg.ah = VBE_SERVICES;
  reg.al = SET_VBE_MOD;
  reg.bx = mode | ENABLE_LIN_BUF;
  reg.intno = BIOS_VIDEO_SERVICES;
  if (sys_int86(&reg) != OK) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }
  return OK;
}

void* new_vg_init(uint16_t mode) {
  vbe_mode_info_t info;
  int r;
  struct minix_mem_range mr;
  if (vbe_get_mode_info(mode, &info) != OK) {
    printf("Unable to get vbe mode info.\n");
    return NULL;
  }

  unsigned int vram_base = info.PhysBasePtr;
  unsigned int vram_size = (info.BitsPerPixel * info.XResolution * info.YResolution) / 8;
  h_res = info.XResolution;
  v_res = info.YResolution;
  bytes_per_pixel = (info.BitsPerPixel + 7) / 8;

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;
  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return NULL;
  }

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return NULL;
  }
  set_mode(mode);
  return video_mem;
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  char *start = (char *) video_mem + (y * h_res + x) * bytes_per_pixel;
  memcpy(start, &color, bytes_per_pixel);
  return OK;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (int offset_x = 0; offset_x < len; offset_x++) {
    vg_draw_pixel(x + offset_x, y, color);
  }
  return OK;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  uint8_t flag = 0;
  if (find_color(x, y) != NIGHT_BACKGROUND && find_color(x, y) != DAY_BACKGROUND) {
    flag = 1;
  }
  for (int offset = 0; offset < height; offset++) {
    vg_draw_hline(x, y + offset, width, color);
  }
  return flag;
}

xpm_image_t load_image(xpm_map_t xpm) {
  xpm_image_t img;
  xpm_load(xpm, XPM_8_8_8, &img);
  return img;
}

int draw_img(xpm_image_t img, uint16_t x, uint16_t y) {
  uint32_t numBytes = bytes_per_pixel * img.width;
  for (int offset_y = 0; offset_y < img.height; offset_y++) {
    memcpy((char *) video_mem + ((y + offset_y) * h_res + x) * bytes_per_pixel, img.bytes + (offset_y * img.width) * bytes_per_pixel, numBytes);
  }
  return OK;
}

int start_screen(uint16_t x1, uint16_t y1, uint32_t color1, uint16_t x2, uint16_t y2, uint32_t color2) {
  uint8_t size = SIZE_FRONT_END;
  return vg_draw_rectangle(x1, y1, size, size, color1) || vg_draw_rectangle(x2, y2, size, size, color2);
}

int opposite(int dir) {
  switch (dir) {
    case LEFT:
      return RIGHT;
    case UP:
      return DOWN;
    case DOWN:
      return UP;
    case RIGHT:
      return LEFT;
    default:
      return 1;
  }
}

int passive_move_players() {
  struct MovementInfo passive_movement_me = {.dir = me.currentDirection, .playerID = ME}, passive_movement_other = {.dir = other.currentDirection, .playerID = OTHER};
  if (move_player(passive_movement_me, true) != 0)
    return 1;
  if (move_player(passive_movement_other, true) != 0)
    return 2;
  return 0;
}

int draw_cursor(xpm_image_t img, uint16_t x, uint16_t y) {
  uint32_t color;
  for (int offset_x = 0; offset_x < img.width; offset_x++) {
    for (int offset_y = 0; offset_y < img.height; offset_y++) {
      color = 0;
      memcpy(&color, &img.bytes[(offset_y * img.width + offset_x) * bytes_per_pixel], bytes_per_pixel);
      if (color != TRANSPARENCY) {
        vg_draw_pixel(x + offset_x, y + offset_y, color);
      }
    }
  }
  return OK;
}

int move_player(struct MovementInfo movementInfo, bool isPassiveMovement) {
  uint16_t movementStep = MOVEMENT_STEP;
  uint16_t dimensions = SIZE_FRONT_END;
  struct PlayerPosition tmp;
  uint32_t color;
  uint8_t flag;
  if (movementInfo.playerID == ME) {
    tmp = me;
    color = COLOR_PLAYER1;
  }
  else {
    tmp = other;
    color = COLOR_PLAYER2;
  }
  if (!isPassiveMovement && movementInfo.dir == tmp.currentDirection) {
    return OK;
  }
  if (movementInfo.dir == opposite(tmp.currentDirection))
    return OK;
  tmp.currentDirection = movementInfo.dir;

  switch (movementInfo.dir) {
    case UP:
      tmp.y -= movementStep;
      break;
    case DOWN:
      tmp.y += movementStep;
      break;
    case LEFT:
      tmp.x -= movementStep;
      break;
    case RIGHT:
      tmp.x += movementStep;
      break;
    default:
      return 1;
  }
  flag = vg_draw_rectangle(tmp.x, tmp.y, dimensions, dimensions, color);
  if (movementInfo.playerID == ME)
    me = tmp;
  else
    other = tmp;
  return flag;
}

int start_game(uint8_t hour) {
  unsigned char a = 0x19;
  if (hour >= a) {
    draw_img(load_image(LightModeGameScreen), 0, 0);
  }
  else {
    draw_img(load_image(DarkModeGameScreen), 0, 0);
  }
  me.currentDirection = RIGHT;
  me.x = h_res / 2 - 100;
  me.y = v_res / 2;
  uint32_t player1Color = COLOR_PLAYER1;

  other.currentDirection = LEFT;
  other.x = h_res / 2 + 100;
  other.y = v_res / 2;
  uint32_t player2Color = COLOR_PLAYER2;

  return start_screen(me.x, me.y, player1Color, other.x, other.y, player2Color);
}

int find_color(uint16_t x, uint16_t y) {
  unsigned int color = 0;
  memcpy(&color, (char *) video_mem + (y * h_res + x) * bytes_per_pixel, bytes_per_pixel);
  return color;
}

int setMouseInitPos(xpm_image_t cursor) {
  mouse.x = h_res / 2;
  mouse.y = v_res / 2;
  return draw_cursor(cursor, mouse.x, mouse.y);
}

int mouseMovement(uint16_t x, uint16_t y, xpm_image_t cursor) {
  mouse.x += x;
  mouse.y -= y;

  if (mouse.x + MOUSE_BORDER_WIDTH >= (int) h_res) {
    mouse.x = h_res - MOUSE_BORDER_WIDTH;
  }
  if (mouse.x <= MOUSE_BORDER_WIDTH) {
    mouse.x = MOUSE_BORDER_WIDTH;
  }
  if (mouse.y + MOUSE_BORDER_WIDTH >= (int) v_res) {
    mouse.y = v_res - MOUSE_BORDER_WIDTH;
  }
  if (mouse.y <= MOUSE_BORDER_WIDTH) {
    mouse.y = MOUSE_BORDER_WIDTH;
  }

  return draw_cursor(cursor, mouse.x, mouse.y);
}

bool mouseInPlace(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  return mouse.x >= x1 && mouse.x <= x2 && mouse.y >= y1 && mouse.y <= y2;
}
