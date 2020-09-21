//
// Created by Felix Hübner on 2020-09-21.
//

#ifndef SYSTEMID_UPDATER_EEPROM_H
#define SYSTEMID_UPDATER_EEPROM_H

#define EEPROM_SIZE sizeof(systemid_t)

typedef struct {
	uint8_t YY;
	uint8_t MM;
	uint8_t DD;
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
} systemid_bcd_date_t;

typedef struct {
	char tagid[4];
	uint8_t major;
	uint8_t minor;
	char sn[10];
	char errata[2];
	systemid_bcd_date_t date;
	uint8_t macsize;
	uint8_t macflags;
	uint8_t mac[6][6];
	uint32_t crc32;
} systemid_t;




uint8_t read_eeprom(systemid_t *e, const char *eeprom_path);
uint8_t write_eeprom(systemid_t *e, const char *eeprom_path);
void print_eeprom(systemid_t *e);
uint8_t init_eeprom(systemid_t *e, const char *eeprom_path, uint8_t read_hw_mac);
void check_eeprom(systemid_t *e);
void write_hw_rev(systemid_t *e, char *hw_rev);
void write_mac_address(uint8_t *emac, char *mac);

#endif //SYSTEMID_UPDATER_EEPROM_H
