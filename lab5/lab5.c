// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>
#include <lcom/video_gr.h>
#include "keyboard.h"
#include "video_new.h"

#include <stdint.h>
#include <stdio.h>


enum RGB_components {R, G, B};

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  //set_mode(mode);
  tickdelay(micros_to_ticks(delay * 1000000));
  vg_exit();
  return 0;
}

int(wait_for_esc)() {
  uint8_t bit_no;
  extern uint8_t scanCode;
  keyboard_subscribe_int(&bit_no);
  int r, ipc_status;
  message msg;
  scanCode = 0;
  while( scanCode != ESC_BREAK_CODE ) {
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & BIT(bit_no)) { /* subscribed interrupt */
                  kbc_ih();   /* process it */
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
  }
  keyboard_unsubscribe_int();
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  vg_init(mode);
  vg_draw_rectangle(x, y, width, height, color);
  wait_for_esc();
  vg_exit();
  return 0;
}

struct RGB_decomposition{
  uint8_t R;
  uint8_t G;
  uint8_t B;
};

int(get_color_components)(uint32_t color, uint16_t mode, struct RGB_decomposition *result){
  uint8_t R, G, B;
  switch (mode)
  {
  case 0x110:
    color = (color << 17) >> 17;
    R = color >> 10;
    G = (color << 22) >> 22;
    B = (color << 27) >> 27;
    break;
  case 0x115:
    color = (color << 8) >> 8; 
    R = (color & 0x00FF0000) >> 16;
    G = (color & 0x0000FF00) >> 8;
    B = (color & 0x000000FF);
    break;
  case 0x11A:
    color = (color << 16) >> 16;
    R = (color & 0x0000F800) >> 11;
    G = (color & 0x000007D0) >> 5;
    B = (color & 0x0000001F);
    break;
  case 0x14C:
    R = (color & 0x00FF0000) >> 16;
    G = (color & 0x0000FF00) >> 8;
    B = (color & 0x000000FF);
    break;
  default:
    printf("Invalid VBE mode!\n");
    return 1;
  }
  result->R = R;
  result->G = G;
  result->B = B;
  return 0;
}

int(get_color)(uint32_t first, uint8_t step, uint8_t no_rectangles, uint32_t row, uint32_t col, uint16_t mode, bool isPacked){
  unsigned BitsPerPixel;
  BitsPerPixel = get_bits_per_pixel();

  if (isPacked){
    return (first + (row * no_rectangles + col) * step) % (1 << BitsPerPixel);
  }
  struct RGB_decomposition decomposition;
  get_color_components(first, mode, &decomposition);
  uint8_t R = (decomposition.R + col * step) % (1 << get_red_mask_size());
  uint8_t G = (decomposition.G + row * step) % (1 << get_green_mask_size());
  uint8_t B = (decomposition.B + (col + row) * step) % (1 << get_blue_mask_size());
  return R << (get_blue_mask_size() + get_green_mask_size()) | G << get_blue_mask_size() | B;
}


int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  vg_init(mode);
  unsigned h_res = get_h_res();
  unsigned v_res = get_v_res();
  unsigned rectangle_width = (h_res - h_res % no_rectangles) / no_rectangles;
  unsigned rectangle_height = (v_res - v_res % no_rectangles) / no_rectangles;
  uint32_t x = 0, y = 0;
  while(y < v_res - v_res%no_rectangles){
    for (int offset = 0; offset + x < h_res - h_res%no_rectangles; offset += rectangle_width){
      vg_draw_rectangle(x+offset, y, rectangle_width, rectangle_height, get_color(first, step, no_rectangles, x+offset, y, mode, mode == 0x105));
    }
    y += rectangle_height;
  }
  wait_for_esc();
  vg_exit();
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  vg_init(0x105);
  xpm_image_t img;
  xpm_load(xpm, XPM_8_8_8, &img);
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
