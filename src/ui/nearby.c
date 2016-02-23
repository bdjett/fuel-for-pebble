#include "../common.h"

static Window *s_window;
static MenuLayer *s_menulayer;
static vector nearby_stations;
static TextLayer *loading_text_layer;

static void get_stations(char *sort) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_cstring(iter, GET_STATIONS, sort);
  app_message_outbox_send();
}

void nearby_stations_in_received_handler(DictionaryIterator *iter) {
  layer_remove_from_parent((Layer *)loading_text_layer);
  Tuple *index_tuple = dict_find(iter, INDEX);
  Tuple *name_tuple = dict_find(iter, STATION_NAME);
  Tuple *price_tuple = dict_find(iter, STATION_PRICE);
  Tuple *address_tuple = dict_find(iter, STATION_ADDRESS);
  if (index_tuple && name_tuple && price_tuple) {
    Station *station = (Station *)malloc(sizeof(Station));
    station->index = index_tuple->value->int16;
    strncpy(station->name, name_tuple->value->cstring, sizeof(station->name));
    strncpy(station->price, price_tuple->value->cstring, sizeof(station->price));
    strncpy(station->address, address_tuple->value->cstring, sizeof(station->address));
    if (nearby_stations.data != NULL) {
      vector_add(&nearby_stations, station);
      layer_mark_dirty(menu_layer_get_layer(s_menulayer));
      menu_layer_reload_data(s_menulayer);
    }
  }
}

// MENU CALLBACKS

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return nearby_stations.count;
}

static int16_t menu_get_row_height_callback(MenuLayer *layer, MenuIndex *cell_index, void *data) {
  return PBL_IF_ROUND_ELSE(81, 41);
}

static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  GRect price_rect = PBL_IF_ROUND_ELSE(GRect(0, 5, 180, 41), GRect(5, 2, 40, 41));
  GRect name_rect = PBL_IF_ROUND_ELSE(GRect(0, 36, 180, 22), GRect(50, 0, 90, 22));
  GRect location_rect = PBL_IF_ROUND_ELSE(GRect(0, 56, 180, 22), GRect(50, 20, 90, 22));
  graphics_draw_text(ctx, ((Station *)nearby_stations.data[cell_index->row])->price, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), price_rect, GTextOverflowModeTrailingEllipsis, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft), NULL);
  graphics_draw_text(ctx, ((Station *)nearby_stations.data[cell_index->row])->name, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), name_rect, GTextOverflowModeTrailingEllipsis, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft), NULL);
  graphics_draw_text(ctx, ((Station *)nearby_stations.data[cell_index->row])->address, fonts_get_system_font(FONT_KEY_GOTHIC_14), location_rect, GTextOverflowModeTrailingEllipsis, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft), NULL);
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  //show_attraction_info(((WaitTime *)wait_times.data[cell_index->row])->id);
}

// Free vector and reload data
static void clean_list(void) {
  vector_free(&nearby_stations);
	layer_mark_dirty(menu_layer_get_layer(s_menulayer));
  menu_layer_reload_data(s_menulayer);
}

// Initialize all UI components
static void initialise_ui(void) {
  s_window = window_create();

  vector_init(&nearby_stations);

  GRect bounds = layer_get_bounds(window_get_root_layer(s_window));

  s_menulayer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menulayer, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
    .get_cell_height = menu_get_row_height_callback
  });
  menu_layer_set_normal_colors(s_menulayer, GColorWhite, GColorBlack);
  menu_layer_set_highlight_colors(s_menulayer, PBL_IF_COLOR_ELSE(GColorPictonBlue, GColorBlack), GColorWhite);
  menu_layer_set_click_config_onto_window(s_menulayer, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_menulayer);

  loading_text_layer = text_layer_create(GRect(0, bounds.size.h / 2 - 15, bounds.size.w, 30));
  text_layer_set_text_alignment(loading_text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(loading_text_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(loading_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(loading_text_layer, "Loading...");
  text_layer_set_text_color(loading_text_layer, GColorBlack);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(loading_text_layer));
}

// Free memory from all UI components
static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(s_menulayer);
  text_layer_destroy(loading_text_layer);
}

// Window unload callback
static void handle_window_unload(Window* window) {
  clean_list();
  destroy_ui();
}

// Show window
void show_nearby_stations(char *sort) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);

  get_stations(sort);
}

// Hide window
void hide_nearby_stations(void) {
  window_stack_remove(s_window, true);
}
