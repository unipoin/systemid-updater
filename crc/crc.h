//
// Created by Felix Hübner on 2020-09-21.
//

#ifndef SYSTEMID_UPDATER_CRC_H
#define SYSTEMID_UPDATER_CRC_H

#include <stdint.h>
#include "eeprom.h"

void update_crc(systemid_t *e);
uint8_t check_crc(systemid_t *e);

#endif //SYSTEMID_UPDATER_CRC_H
