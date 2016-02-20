#include <pebble.h>
#include "vector.h"
#include "ui/main_menu.h"
#include "ui/nearby.h"

typedef struct {
  int index;
  char name[30];
  char price[5];
  char address[100];
} Station;

enum {
  INDEX = 0x0,
  STATION_NAME = 0x1,
  STATION_PRICE = 0x2,
  STATION_ADDRESS = 0x3,
  GET_STATIONS = 0x4
};
