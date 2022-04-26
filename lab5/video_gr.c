#include <lcom/lcf.h>
#include <lcom/video_gr.h>
#include "video_new.h"

static void *video_mem;		/* Process (virtual) address to which VRAM is mapped */

static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel */
static uint16_t operatingMode;
static uint8_t RedMaskSize;
static uint8_t GreenMaskSize;
static uint8_t BlueMaskSize;

uint8_t get_red_mask_size(){
  return RedMaskSize;
}

uint8_t get_blue_mask_size(){
  return BlueMaskSize;
}

uint8_t get_green_mask_size (){
  return GreenMaskSize;
}

unsigned get_h_res (){
  return h_res;
}

unsigned get_v_res (){
  return v_res;
}

unsigned get_bits_per_pixel(){
  return bytes_per_pixel/8;
}

int(set_mode)(uint16_t mode){
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

void *(vg_init)(uint16_t mode){
  vbe_mode_info_t info;
  set_mode(mode);
  vbe_get_mode_info(mode, &info);
  int r;
  struct minix_mem_range mr; /* physical memory range */
  unsigned int vram_base = info.PhysBasePtr;    /* VRAM’s physical addresss */
  unsigned int vram_size = (info.BitsPerPixel * info.XResolution * info.YResolution) / 8;/* VRAM’s size, but you can use the frame-buffer size, instead */
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
  return video_mem;
}


int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
  char* tmp;
  for (int i=x;i<x+len;i++){

    tmp = (char *) video_mem + (y*h_res + i) * bytes_per_pixel;
    memcpy(tmp, &color, bytes_per_pixel);
  }
  return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for (int k=0;k<height;k++){
    vg_draw_hline(x, y+k, width, color);
  }
  return 0;
}
