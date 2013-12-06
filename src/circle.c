#include <pebble.h>

#define BAR_MOVE_AMOUNT 50

static Window *window;
static Layer *circle;
static Layer *left_bar;

static void move(Layer *layer, int x, int y, int time) {
  GRect frame = layer_get_frame(layer);

  GRect from_frame = layer_get_frame(layer);
  GRect to_frame = GRect(x, y, frame.size.w, frame.size.h);

  Animation* property_animation =
    (Animation*) property_animation_create_layer_frame(
        layer, &from_frame, &to_frame);
  animation_set_duration(property_animation, time);
  animation_schedule(property_animation);
}

static void circle_draw(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_fill_color(ctx, GColorBlack);
  const int16_t half_h = bounds.size.h / 2;
  graphics_fill_circle(ctx, GPoint(half_h, half_h), half_h);
}

static void fill(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  GRect frame = layer_get_frame(left_bar);
  move(left_bar, frame.origin.x, frame.origin.y - BAR_MOVE_AMOUNT, 500);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  GRect frame = layer_get_frame(left_bar);
  move(left_bar, frame.origin.x, frame.origin.y + BAR_MOVE_AMOUNT, 500);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  move(circle, rand() % 144, rand() % 144, 5000);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  circle = layer_create(GRect(0, 0, 5, 5));
  layer_set_update_proc(circle, circle_draw);
  layer_add_child(window_layer, circle);

  left_bar = layer_create(GRect(0, 72, 10, 50));
  layer_set_update_proc(left_bar, fill);
  layer_add_child(window_layer, left_bar);
}

static void window_unload(Window *window) {
  layer_destroy(circle);
  layer_destroy(left_bar);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
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
