/***************************************************
 * GasBuddy For Pebble
 *
 * NOT ASSOCIATED WITH GASBUDY
 *
 * Created by: Brian Jett (http://logicalpixels.com)
 */

#include "common.h"

// APPMESSAGE CALLBACKS

void out_sent_handler(DictionaryIterator *sent, void *ctx) {
  // Outgoing message was delivered
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *ctx) {
  // Outgoing message failed
}

void in_received_handler(DictionaryIterator *iter, void *ctx) {
  // Incoming message received
  nearby_stations_in_received_handler(iter);
}

void in_dropped_handler(AppMessageResult reason, void *ctx) {
  // Incoming message dropped
}

// INITIALIZATION

static void init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  app_message_open(800, 400);

  show_main_menu();
}

// MAIN

int main(void) {
  init();
  app_event_loop();
}
