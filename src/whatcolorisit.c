#include <pebble.h>

static Window *window;
static TextLayer *text_layer_time;
static TextLayer *text_layer_rgb;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer_time = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 48), bounds.size.w, 50));
  text_layer_rgb = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(115, 100), bounds.size.w, 50));

  text_layer_set_text(text_layer_time, "00:00");
  text_layer_set_text(text_layer_rgb, "#FFFFFF");

  text_layer_set_font(text_layer_time, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_font(text_layer_rgb, fonts_get_system_font(FONT_KEY_GOTHIC_24));

  text_layer_set_text_alignment(text_layer_time, GTextAlignmentCenter);
  text_layer_set_text_alignment(text_layer_rgb, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(text_layer_time));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_rgb));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer_time);
  text_layer_destroy(text_layer_rgb);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
