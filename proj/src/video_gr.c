#include <lcom/lcf.h>
#include <lcom/video_gr.h>

#include "keyboard.h"
#include "video_gr_gameAPI.h"

#include "auxiliary_data_structures.h"

#define MOVEMENT_STEP 5;
#define SIZE_FRONT_END 5;
#define COLOR_BLUE 0x1F51FF;
#define COLOR_ORANGE 0xFF5F1F;

uint32_t my_color = 0x1F51FF, other_color = 0xFF5F1F;

static void *video_mem;
static unsigned h_res;
static unsigned v_res;
static unsigned bytes_per_pixel;
static uint16_t operatingMode;
static uint8_t RedMaskSize;
static uint8_t GreenMaskSize;
static uint8_t BlueMaskSize;
uint16_t img_height;
uint16_t img_width;

static struct PlayerPosition me, other;
// iniciar o modo e zerar as posicoes dos jogadores

struct mousePos mouse;

uint8_t get_red_mask_size() {
  return RedMaskSize;
}

uint8_t get_blue_mask_size() {
  return BlueMaskSize;
}

uint8_t get_green_mask_size() {
  return GreenMaskSize;
}

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
  operatingMode = mode;
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));
  reg.ah = 0x4F;
  reg.al = 0x02;
  reg.bx = mode | BIT(14);
  reg.intno = 0x10;
  if (sys_int86(&reg) != OK) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }
  return 0;
}

int new_vg_init(uint16_t mode) {
  vbe_mode_info_t info;
  int r;
  struct minix_mem_range mr;
  vbe_get_mode_info(mode, &info);

  unsigned int vram_base = info.PhysBasePtr;
  unsigned int vram_size = (info.BitsPerPixel * info.XResolution * info.YResolution) / 8;
  h_res = info.XResolution;
  v_res = info.YResolution;
  bytes_per_pixel = (info.BitsPerPixel + 7) / 8;
  BlueMaskSize = info.BlueMaskSize;
  RedMaskSize = info.RedMaskSize;
  GreenMaskSize = info.GreenMaskSize;

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;
  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return 1;
  }

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return 2;
  }

  return set_mode(mode);
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  char *start = (char *) video_mem + (y * h_res + x) * bytes_per_pixel;
  memcpy(start, &color, bytes_per_pixel);
  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (int offset_x = 0; offset_x < len; offset_x++) {
    vg_draw_pixel(x + offset_x, y, color);
  }
  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  uint8_t flag = 0;
  if (find_color(x, y) != 0 && find_color(x, y) != 0xffffff) {
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

int draw_img2(xpm_image_t img, uint16_t x, uint16_t y) {
  for (int offset_x = 0; offset_x < img.width; offset_x++) {
    for (int offset_y = 0; offset_y < img.height; offset_y++) {
      uint32_t color;
      memcpy(&color, &img.bytes[(offset_y * img.width + offset_x) * bytes_per_pixel], bytes_per_pixel);
      vg_draw_pixel(x + offset_x, y + offset_y, color);
    }
  }
  return 0;
}

int draw_img(xpm_image_t img, uint16_t x, uint16_t y) {
  uint32_t numBytes = bytes_per_pixel * img.width;
  for (int offset_y = 0; offset_y < img.height; offset_y++) {
    memcpy((char *) video_mem + ((y + offset_y) * h_res + x) * bytes_per_pixel, img.bytes + (offset_y * img.width) * bytes_per_pixel, numBytes);
  }
  return 0;
}

bool continueLoop(uint16_t xi, uint16_t xf, uint16_t yi, uint16_t yf, bool isHorizontal, uint8_t scanCode) {
  if (scanCode == 0x81)
    return false;
  if (isHorizontal && xi >= xf)
    return false;
  else if (!isHorizontal && yi >= yf)
    return false;
  return true;
}

// PROJECT:

int start_screen(uint16_t x1, uint16_t y1, uint32_t color1, uint16_t x2, uint16_t y2, uint32_t color2, uint16_t OBJ_SIZE) {
  if (vg_draw_rectangle(x1, y1, OBJ_SIZE, OBJ_SIZE, color1) || vg_draw_rectangle(x2, y2, OBJ_SIZE, OBJ_SIZE, color2))
    return 1;
  return 0;
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
  for (int offset_x = 0; offset_x < img.width; offset_x++) {
    for (int offset_y = 0; offset_y < img.height; offset_y++) {
      uint32_t color;
      memcpy(&color, &img.bytes[(offset_y * img.width + offset_x)*bytes_per_pixel], bytes_per_pixel);
      if (color != 0x123456)
        vg_draw_pixel(x + offset_x, y + offset_y, color);
    }
  }
  return 0;
}

int move_player(struct MovementInfo movementInfo, bool isPassiveMovement) {
  uint16_t movementStep = MOVEMENT_STEP;
  uint16_t dimensions = SIZE_FRONT_END;
  struct PlayerPosition tmp;
  uint32_t color;
  uint8_t flag;
  if (movementInfo.playerID == ME) {
    tmp = me;
    color = my_color;
  }
  else {
    tmp = other;
    color = other_color;
  }
  if (!isPassiveMovement && movementInfo.dir == tmp.currentDirection) {
    return 0;
  }
  if (movementInfo.dir == opposite(tmp.currentDirection))
    return 0;
  tmp.currentDirection = movementInfo.dir;

  // idk why, but i need to pass the parameters like this
  switch (movementInfo.dir) {
    case UP:
      tmp.y -= movementStep;
      flag = vg_draw_rectangle(tmp.x, tmp.y, dimensions, dimensions, color);
      break;
    case DOWN:
      tmp.y += movementStep;
      flag = vg_draw_rectangle(tmp.x, tmp.y, dimensions, dimensions, color);
      break;
    case LEFT:
      tmp.x -= movementStep;
      flag = vg_draw_rectangle(tmp.x, tmp.y, dimensions, dimensions, color);
      break;
    case RIGHT:
      tmp.x += movementStep;
      flag = vg_draw_rectangle(tmp.x, tmp.y, dimensions, dimensions, color);
      break;
    default:
      return 1;
  }
  if (movementInfo.playerID == ME)
    me = tmp;
  else
    other = tmp;
  return flag;
}

int start_game(uint8_t hour) {
  unsigned char a = 0x19;
  if (hour >= a) {
    memset(video_mem, 255, h_res * v_res * bytes_per_pixel);
  } else{
    memset(video_mem, 0, h_res * v_res * bytes_per_pixel);
  }
  me.currentDirection = RIGHT;
  me.x = h_res / 2 - 100;
  me.y = v_res / 2;

  other.currentDirection = LEFT;
  other.x = h_res / 2 + 100;
  other.y = v_res / 2;
  start_screen(me.x, me.y, my_color, other.x, other.y, other_color, 5);
  return 0;
}

int(find_color)(uint16_t x, uint16_t y) {
  unsigned int color = 0;
  memcpy(&color, (char *) video_mem + (y * h_res + x) * bytes_per_pixel, bytes_per_pixel);
  return color;
}

int(setMouseInitPos)(xpm_image_t cursor) {
  mouse.x = h_res / 2;
  mouse.y = v_res / 2;
  draw_cursor(cursor, mouse.x, mouse.y);
  return 0;
}

int(mouseMovement)(uint16_t x, uint16_t y, xpm_image_t cursor) {
  mouse.x += x;
  mouse.y -= y;

  if (mouse.x >= h_res - 30) {
    mouse.x = h_res - 30;
  }
  if (mouse.x <= 30) {
    mouse.x = 30;
  }
  if (mouse.y >= v_res - 30) {
    mouse.y = v_res - 30;
  }
  if (mouse.y <= 30) {
    mouse.y = 30;
  }

  draw_cursor(cursor, mouse.x, mouse.y);
  return 0;
}

bool mouseInPlace(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  if (mouse.x >= x1 && mouse.x <= x2 && mouse.y >= y1 && mouse.y <= y2) {
    return true;
  }
  return false;
}
