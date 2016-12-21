#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;


static BitmapLayer *red_str_layer;
static GBitmap *red_str;

static BitmapLayer *blue_str_layer;
static GBitmap *blue_str;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void layer_draw_flag(Layer *layer, GContext *ctx) {
     graphics_context_set_fill_color(ctx, GColorDarkCandyAppleRed);
     graphics_fill_rect(ctx, GRect(0,0,200,50), 0, 0);
  
     graphics_context_set_fill_color(ctx, GColorBlue);
     graphics_fill_rect(ctx, GRect(0,118,200,200), 0, 0);
}

static void main_window_load(Window *window) {
  
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);

  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_time_layer, GColorDarkCandyAppleRed);
  
  text_layer_set_text(s_time_layer, "00:00");
 
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
   // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PATRIOT_48));

  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  
  // Create a Layer and set the update_proc
  Layer *flag_layer = window_get_root_layer(window);
  flag_layer = layer_create(GRect(0, 0, 200, 200));
  layer_set_update_proc(flag_layer, layer_draw_flag);
  

  // Add it as a child layer to the Window's root layer
  
  layer_add_child(window_layer, flag_layer);
  
  
  // Load the resource
red_str = gbitmap_create_with_resource(RESOURCE_ID_STARS_RED);
blue_str = gbitmap_create_with_resource(RESOURCE_ID_STARS_BLUE);

// Create the BitmapLayers
red_str_layer = bitmap_layer_create(bounds);
blue_str_layer = bitmap_layer_create(bounds);
bitmap_layer_set_bitmap(red_str_layer, red_str);
bitmap_layer_set_bitmap(blue_str_layer, blue_str);

bitmap_layer_set_alignment(red_str_layer, GAlignTop);
bitmap_layer_set_alignment(blue_str_layer, GAlignBottom);
layer_add_child(window_layer, bitmap_layer_get_layer(red_str_layer));
layer_add_child(window_layer, bitmap_layer_get_layer(blue_str_layer));
  
}

static void main_window_unload(Window *window) {
  // Unload GFont
  fonts_unload_custom_font(s_time_font);

  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  gbitmap_destroy(red_str);
  bitmap_layer_destroy(red_str_layer);
  gbitmap_destroy(blue_str);
  bitmap_layer_destroy(blue_str_layer);
}





static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
