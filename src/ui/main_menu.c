#include "../common.h"

static Window *s_window;
static MenuLayer *s_menulayer;
static GBitmap *dollar_bitmap;
static GBitmap *location_bitmap;
static GBitmap *settings_bitmap;

// MENU CALLBACKS

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return 2;
      break;
    case 1:
      return 1;
      break;
    default:
      return 0;
      break;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *layer, uint16_t section_index, void *data) {
  if (section_index == 0) {
    return MENU_CELL_BASIC_HEADER_HEIGHT;
  } else {
    return 5;
  }
}

static void menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  if (section_index == 0) {
    menu_cell_basic_header_draw(ctx, cell_layer, "Sort Stations By");
  }
}

static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  if (cell_index->section == 0) {
    if (cell_index->row == 0) {
      menu_cell_basic_draw(ctx, cell_layer, "Price", NULL, dollar_bitmap);
    } else {
      menu_cell_basic_draw(ctx, cell_layer, "Location", NULL, location_bitmap);
    }
  } else {
    menu_cell_basic_draw(ctx, cell_layer, "Settings", NULL, settings_bitmap);
  }
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  if (cell_index->section == 0) {
    if (cell_index->row == 0) {
      show_nearby_stations("price");
    } else {
      show_nearby_stations("location");
    }
  }
}

// Initialize all UI components
static void initialise_ui(void) {
  s_window = window_create();

  settings_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SETTINGS);
  dollar_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DOLLAR);
  location_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LOCATION);

  // menu_layer
  s_menulayer = menu_layer_create(GRect(0, 0, 144, 152));
  menu_layer_set_callbacks(s_menulayer, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_row = menu_draw_row_callback,
    .draw_header = menu_draw_header_callback,
    .select_click = menu_select_callback
  });
  menu_layer_set_click_config_onto_window(s_menulayer, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_menulayer);
}

// Free all memory form UI components
static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(s_menulayer);
  gbitmap_destroy(settings_bitmap);
  gbitmap_destroy(dollar_bitmap);
  gbitmap_destroy(location_bitmap);
}

// Window unload callback
static void handle_window_unload(Window* window) {
  destroy_ui();
}

// Show window
void show_main_menu(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

// Hide window
void hide_main_menu(void) {
  window_stack_remove(s_window, true);
}
