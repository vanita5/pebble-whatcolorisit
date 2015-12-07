#include <pebble.h>

static Window *window;

static GFont custom_font_20;
static GFont custom_font_38;

static TextLayer *text_layer_time;
static TextLayer *text_layer_rgb;

static void update_ui() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

  text_layer_set_text(text_layer_time, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_ui();
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer_time = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 48), bounds.size.w, 50));
  text_layer_rgb = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(115, 100), bounds.size.w, 50));

  text_layer_set_text(text_layer_time, "00:00");
  text_layer_set_text(text_layer_rgb, "#FFFFFF");

  custom_font_38 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_HACK_38));
  custom_font_20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_HACK_20));

  text_layer_set_font(text_layer_time, custom_font_38);
  text_layer_set_font(text_layer_rgb, custom_font_20);

  text_layer_set_text_alignment(text_layer_time, GTextAlignmentCenter);
  text_layer_set_text_alignment(text_layer_rgb, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(text_layer_time));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_rgb));
}

static void window_unload(Window *window) {
  fonts_unload_custom_font(custom_font_20);

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

  update_ui();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
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
