#include "keyboard.h"
#include "video_new.h"
#include <lcom/lcf.h>
#include <lcom/video_gr.h>

static void *video_mem; /* Process (virtual) address to which VRAM is mapped */

static unsigned h_res;           /* Horizontal resolution in pixels */
static unsigned v_res;           /* Vertical resolution in pixels */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel */
static uint16_t operatingMode;
static uint8_t RedMaskSize;
static uint8_t GreenMaskSize;
static uint8_t BlueMaskSize;
uint16_t img_height;
uint16_t img_width;

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
  struct minix_mem_range mr; /* physical memory range */
  vbe_get_mode_info(mode, &info);

  unsigned int vram_base = info.PhysBasePtr;                                              /* VRAM’s physical addresss */
  unsigned int vram_size = (info.BitsPerPixel * info.XResolution * info.YResolution) / 8; /* VRAM’s size, but you can use the frame-buffer size, instead */
  h_res = info.XResolution;
  v_res = info.YResolution;
  bytes_per_pixel = info.BytesPerScanLine / h_res;
  BlueMaskSize = info.BlueMaskSize;
  RedMaskSize = info.RedMaskSize;
  GreenMaskSize = info.GreenMaskSize;

  /* Allow memory mapping */
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;
  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */
  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED)
    panic("couldn’t map video memory");

  set_mode(mode);
  return video_mem;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  char *tmp;
  for (int i = x; i < x + len; i++) {

    tmp = (char *) video_mem + (y * h_res + i) * bytes_per_pixel;
    memcpy(tmp, &color, bytes_per_pixel);
  }
  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (int k = 0; k < height; k++) {
    vg_draw_hline(x, y + k, width, color);
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

    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
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
            // printf("xi:%d, xf:%d, yi:%d, yf:%d\n", xi, xf, yi, yf);
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
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }
  printf("xi:%d, xf:%d, yi:%d, yf:%d\n", xi, xf, yi, yf);
  timer_unsubscribe_int();
  keyboard_unsubscribe_int();
  return 0;
}
