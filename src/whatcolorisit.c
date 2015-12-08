#include <pebble.h>

static Window *window;

static GFont custom_font_20;
static GFont custom_font_38;

static TextLayer *text_layer_time;
static TextLayer *text_layer_rgb;

static unsigned long getRGB(int r, int g, int b) {
  return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

static void update_ui() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

  int r = (int)(255 * (tick_time->tm_hour / 23.0));
  int g = (int)(255 * (tick_time->tm_min / 59.0));
  int b = (int)(255 * (tick_time->tm_sec / 59.0));

  unsigned long hex = getRGB(r, g, b);

  static char s_hex_buffer[16];
  snprintf(s_hex_buffer, sizeof(s_hex_buffer), "#%02X%02X%02X", r, g, b);

  text_layer_set_text(text_layer_time, s_buffer);
  text_layer_set_text(text_layer_rgb, s_hex_buffer);

  //text_layer_set_text_color(text_layer_time, GColorWhite);
  //text_layer_set_text_color(text_layer_rgb, GColorWhite);

  window_set_background_color(window, GColorFromHEX(hex));
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

  text_layer_set_background_color(text_layer_time, GColorClear);
  text_layer_set_background_color(text_layer_rgb, GColorClear);

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
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
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
