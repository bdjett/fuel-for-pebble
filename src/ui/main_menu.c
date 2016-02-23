#include "../common.h"

static Window *s_window;
static MenuLayer *s_menulayer;
static GBitmap *dollar_bitmap;
static GBitmap *location_bitmap;
static Layer *content_up_layer;
static Layer *content_down_layer;
static ContentIndicator *content_indicator;

// MENU CALLBACKS

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return 2;
}

static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  GRect bounds = layer_get_bounds(cell_layer);
  GPoint center = GPoint(bounds.size.w / 2, bounds.size.h / 2);

  GColor fill_color = GColorBlack;
  switch (cell_index->row) {
    case 0:
      fill_color = PBL_IF_COLOR_ELSE(GColorIslamicGreen, GColorBlack);
      break;
    case 1:
      fill_color = PBL_IF_COLOR_ELSE(GColorChromeYellow, GColorBlack);
      break;
  }

  graphics_context_set_fill_color(ctx, fill_color);
  PBL_IF_ROUND_ELSE(graphics_fill_circle(ctx, center, 40), graphics_fill_rect(ctx, GRect(center.x - 40, center.y - 40, 80, 80), 0, GCornerNone));

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  switch (cell_index->row) {
    case 0:
      graphics_draw_bitmap_in_rect(ctx, dollar_bitmap, GRect(center.x - 12, center.y - 14, 24, 28));
      break;
    case 1:
      graphics_draw_bitmap_in_rect(ctx, location_bitmap, GRect(center.x - 12, center.y - 14, 24, 28));
      break;
  }


  GRect label_rect = GRect(0, center.y + 45, bounds.size.w, 20);
  const char *label = "";
  switch (cell_index->row) {
    case 0:
      label = "Price";
      break;
    case 1:
      label = "Location";
      break;
  }
  graphics_draw_text(ctx, label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), label_rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
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

static int16_t menu_get_row_height_callback(MenuLayer *layer, MenuIndex *cell_index, void *data) {
  return layer_get_bounds(menu_layer_get_layer(layer)).size.h;
}

static void menu_selection_changed_callback(MenuLayer *menu_layer, MenuIndex new_index, MenuIndex old_index, void *data) {
  switch (new_index.row) {
    case 0:
      content_indicator_set_content_available(content_indicator, ContentIndicatorDirectionDown, true);
      content_indicator_set_content_available(content_indicator, ContentIndicatorDirectionUp, false);
      break;
    case 1:
      content_indicator_set_content_available(content_indicator, ContentIndicatorDirectionDown, false);
      content_indicator_set_content_available(content_indicator, ContentIndicatorDirectionUp, true);
      break;
  }
}

// Initialize all UI components
static void initialise_ui(void) {
  s_window = window_create();

  dollar_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DOLLAR);
  location_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LOCATION);

  // menu_layer
  GRect bounds = layer_get_bounds(window_get_root_layer(s_window));
  s_menulayer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menulayer, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
    .get_cell_height = menu_get_row_height_callback,
    .selection_changed = menu_selection_changed_callback
  });
  menu_layer_set_click_config_onto_window(s_menulayer, s_window);
  menu_layer_pad_bottom_enable(s_menulayer, false);
  menu_layer_set_normal_colors(s_menulayer, GColorWhite, GColorBlack);
  menu_layer_set_highlight_colors(s_menulayer, GColorWhite, GColorBlack);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_menulayer);

  content_indicator = content_indicator_create();
  content_up_layer = layer_create(GRect(0, 0, bounds.size.w, STATUS_BAR_LAYER_HEIGHT));
  content_down_layer = layer_create(GRect(0, bounds.size.h - STATUS_BAR_LAYER_HEIGHT, bounds.size.w, STATUS_BAR_LAYER_HEIGHT));
  layer_add_child(window_get_root_layer(s_window), content_up_layer);
  layer_add_child(window_get_root_layer(s_window), content_down_layer);
  ContentIndicatorConfig up_config = (ContentIndicatorConfig) {
    .layer = content_up_layer,
    .times_out = false,
    .alignment = GAlignCenter,
    .colors = {
      .foreground = GColorBlack,
      .background = GColorWhite
    }
  };
  ContentIndicatorConfig down_config = (ContentIndicatorConfig) {
    .layer = content_down_layer,
    .times_out = false,
    .alignment = GAlignCenter,
    .colors = {
      .foreground = GColorBlack,
      .background = GColorWhite
    }
  };
  content_indicator_configure_direction(content_indicator, ContentIndicatorDirectionUp, &up_config);
  content_indicator_configure_direction(content_indicator, ContentIndicatorDirectionDown, &down_config);
  content_indicator_set_content_available(content_indicator, ContentIndicatorDirectionDown, true);
}

// Free all memory form UI components
static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(s_menulayer);
  gbitmap_destroy(dollar_bitmap);
  gbitmap_destroy(location_bitmap);
  layer_destroy(content_up_layer);
  layer_destroy(content_down_layer);
  content_indicator_destroy(content_indicator);
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
