//
// Created by Felix Hübner on 2020-09-21.
//

#ifndef SYSTEMID_UPDATER_EEPROM_H
#define SYSTEMID_UPDATER_EEPROM_H

#include <stdint.h>

#define CCID_MAC_PORTS 8

typedef struct __attribute__ ((__packed__)) {
	uint8_t YY;
	uint8_t MM;
	uint8_t DD;
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
} systemid_bcd_date_t;

typedef struct __attribute__ ((__packed__)) {
	char tagid[4];
	uint8_t major;
	uint8_t minor;
	char sn[10];
	char errata[2];
	systemid_bcd_date_t date;
	uint8_t res_0[40];
	uint8_t macsize;
	uint8_t macflags;
	uint8_t mac[CCID_MAC_PORTS][6];
	uint32_t crc32;
} systemid_t;

#define EEPROM_SIZE sizeof(systemid_t)


uint8_t read_eeprom(systemid_t *e, const char *eeprom_path);
uint8_t write_eeprom(systemid_t *e, const char *eeprom_path);
void print_eeprom(systemid_t *e);
uint8_t init_eeprom(systemid_t *e, const char *eeprom_path, uint8_t read_hw_mac);
void check_eeprom(systemid_t *e);
void write_hw_rev(systemid_t *e, char *hw_rev);
void write_mac_address(uint8_t *emac, char *mac);

#endif //SYSTEMID_UPDATER_EEPROM_H
