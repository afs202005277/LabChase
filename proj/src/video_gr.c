#include "auxiliary_data_structures.h"
#include "keyboard.h"
#include "video_new.h"
#include <lcom/lcf.h>
#include <lcom/video_gr.h>

#define MOVEMENT_STEP 5;
#define SIZE_FRONT_END 5;
#define COLOR_BLUE 0x000000FF;
#define COLOR_ORANGE 0x00FF8000;

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

static struct PlayerPosition bluePlayer, orangePlayer;
// iniciar o modo e zerar as posicoes dos jogadores

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

unsigned get_bits_per_pixel() {
  return bytes_per_pixel / 8;
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

void *new_vg_init(uint16_t mode) {
  vbe_mode_info_t info;
  int r;
  struct minix_mem_range mr;
  vbe_get_mode_info(mode, &info);

  unsigned int vram_base = info.PhysBasePtr;
  unsigned int vram_size = (info.BitsPerPixel * info.XResolution * info.YResolution) / 8;
  h_res = info.XResolution;
  v_res = info.YResolution;
  bytes_per_pixel = info.BytesPerScanLine / h_res;
  BlueMaskSize = info.BlueMaskSize;
  RedMaskSize = info.RedMaskSize;
  GreenMaskSize = info.GreenMaskSize;

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;
  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED)
    panic("couldn’t map video memory");

  set_mode(mode);
  return video_mem;
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
  for (int offset = 0; offset < height; offset++) {
    vg_draw_hline(x, y + offset, width, color);
  }
  return 0;
}

int xpm_drawer(xpm_map_t xpm, uint16_t x, uint16_t y) {
  xpm_image_t img;
  xpm_load(xpm, XPM_INDEXED, &img);
  img_height = img.height;
  img_width = img.width;
  for (int height = 0; height < img.height; height++) {
    for (int width = 0; width < img.width; width++) {
      vg_draw_hline(x + width, y + height, 1, img.bytes[height * img.width + width]);
    }
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

int xpm_move(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  uint16_t prevX = xi;
  uint16_t prevY = yi;
  int r, ipc_status;
  extern int totalInterrupts;
  extern uint8_t scanCode;
  message msg;
  uint8_t timerBIT, kbcBIT;
  int frames = 60 / fr_rate;
  bool isHorizontal = false;
  if (yi == yf)
    isHorizontal = true;
  timer_subscribe_int(&timerBIT);
  keyboard_subscribe_int(&kbcBIT);
  xpm_drawer(xpm, xi, yi);
  while (continueLoop(xi, xf, yi, yf, isHorizontal, scanCode)) {

    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(timerBIT)) {
            totalInterrupts++;
            if (isHorizontal) {
              if (xi + speed > xf) {
                xi = xf;
                vg_draw_rectangle(prevX, prevY, img_width, img_height, 0x00000000);
                xpm_drawer(xpm, xi, yi);
                break;
              }
              else {
                xi += speed;
              }
            }
            else {
              if (yi + speed > yf) {
                yi = yf;
                vg_draw_rectangle(prevX, prevY, img_width, img_height, 0x00000000);
                xpm_drawer(xpm, xi, yi);
                break;
              }
              else {
                yi += speed;
              }
            }
            if (totalInterrupts % frames == 0) {
              vg_draw_rectangle(prevX, prevY, img_width, img_height, 0x00000000);
              prevX = xi;
              prevY = yi;
              xpm_drawer(xpm, xi, yi);
            }
          }
          if (msg.m_notify.interrupts & BIT(kbcBIT)) {
            kbc_ih();
          }
          break;
        default:
          break;
      }
    }
    else {
    }
  }
  timer_unsubscribe_int();
  keyboard_unsubscribe_int();
  return 0;
}

int movePlayer(struct MovementInfo movementInfo) {
  uint16_t movementStep = MOVEMENT_STEP;
  uint16_t dimensions = SIZE_FRONT_END;
  struct PlayerPosition tmp;
  uint32_t color;
  uint8_t flag;
  if (movementInfo.playerColor == BLUE) {
    tmp = bluePlayer;
    color = COLOR_BLUE;
  }
  else {
    tmp = orangePlayer;
    color = COLOR_ORANGE;
  }
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
  }
  if (movementInfo.playerColor == BLUE)
    bluePlayer = tmp;
  else
    orangePlayer = tmp;
  return flag;
}
