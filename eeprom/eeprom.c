//
// Created by Felix Hübner on 2020-09-21.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include "eeprom.h"
#include "crc.h"

#define FIXED_MAC_OFFSET    0xFA
#define TAG_ID_CCID            "CCID"

uint8_t read_fixed_mac(uint8_t *mac, const char *eeprom_path) {
	FILE *filehandle;
	uint8_t exitcode;

	filehandle = fopen(eeprom_path, "rb");
	if(filehandle == NULL) {
		fprintf(stderr, "\nError while opening file: %s\n", eeprom_path);
		return EXIT_FAILURE;
	}

	exitcode = (uint8_t) fseek(filehandle, FIXED_MAC_OFFSET, SEEK_SET);

	if(exitcode != EXIT_FAILURE && fread(mac, 6, 1, filehandle) != 1) {
		fprintf(stderr, "\nError while reading mac from file\n");
		exitcode = EXIT_FAILURE;
	}

	fclose(filehandle);
	return exitcode;
}

uint8_t read_eeprom(systemid_t *e, const char *eeprom_path) {
	struct __sFILE *filehandle;
	uint8_t exitcode = EXIT_SUCCESS;

	filehandle = fopen(eeprom_path, "rb");
	if(filehandle == NULL) {
		fprintf(stderr, "\nError while opening file: %s\n", eeprom_path);
		return EXIT_FAILURE;
	}

	if(fread(e, EEPROM_SIZE, 1, filehandle) != 1) {
		fprintf(stderr, "\nError while reading eeprom content from file\n");
		exitcode = EXIT_FAILURE;
	}

	fclose(filehandle);
	return exitcode;
}

uint8_t write_eeprom(systemid_t *e, const char *eeprom_path) {
	struct __sFILE *filehandle;
	uint8_t exitcode = EXIT_SUCCESS;

	filehandle = fopen(eeprom_path, "wb");
	if(filehandle == NULL) {
		fprintf(stderr, "\nError while opening file: %s\n", eeprom_path);
		return EXIT_FAILURE;
	}

	if(fwrite(e, EEPROM_SIZE, 1, filehandle) != 1) {
		fprintf(stderr, "\nError while writing eeprom content to file\n");
		exitcode = EXIT_FAILURE;
	}

	fclose(filehandle);
	return exitcode;
}

void print_eeprom(systemid_t *e) {
	char line[256];
	for(uint8_t i = 0; i <= (EEPROM_SIZE / 16); i++) {
		uint8_t len = sprintf(line, "%02X: ", i * 16);
		for(uint8_t j = 0; j < 16; j++) {
			if(i * 16 + j < EEPROM_SIZE) {
				len += sprintf(&(line[len]), "%02X ", *((uint8_t *) e + (i * 16 + j)));
			} else {
				len += sprintf(&(line[len]), "   ");
			}
		}
		len += sprintf(&(line[len]), "| ");
		for(uint8_t j = 0; j < 16; j++) {
			if(i * 16 + j < EEPROM_SIZE) {
				len += sprintf(&(line[len]), "%1c", *((uint8_t *) e + (i * 16 + j)));
			}
		}
		fprintf(stdout, "%s\n", line);
	}
}

uint8_t init_eeprom(systemid_t *e, const char *eeprom_path, uint8_t read_hw_mac) {
	uint8_t exitcode = EXIT_SUCCESS;
	// set tag-id
	memcpy(e->tagid, TAG_ID_CCID, 4);

	// set current date as build date
	__darwin_time_t now = time(NULL);
	if(now != -1) {
		struct tm *p_utc_now = gmtime(&now);
		if(p_utc_now != NULL) {
			e->date.YY = (uint8_t) (p_utc_now->tm_year % 100);
			e->date.MM = (uint8_t) (p_utc_now->tm_mon + 1);
			e->date.DD = (uint8_t) p_utc_now->tm_mday;
			e->date.hh = (uint8_t) p_utc_now->tm_hour;
			e->date.mm = (uint8_t) p_utc_now->tm_min;
			e->date.ss = (uint8_t) p_utc_now->tm_sec;
		} else {
			exitcode = EXIT_FAILURE;
		}
	} else {
		exitcode = EXIT_FAILURE;
	}

	//read fixed mac and set in systemid eeprom
	if(exitcode == EXIT_SUCCESS && read_hw_mac) {
		if((exitcode = read_fixed_mac(e->mac[0], eeprom_path)) == EXIT_SUCCESS) {
			e->macsize = 1;
		}

	}

	update_crc(e);

	return exitcode;
}


void check_eeprom(systemid_t *e) {
	char value[16];
	fprintf(stdout, "check EEPROM:\n");
	if (check_crc(e) == EXIT_FAILURE) {
		return;
	}
	snprintf(value, 5, "%s", e->tagid);
	fprintf(stdout, "TagID: %s\n", value);
	if (e->errata[0] || e->errata[1]) {
		fprintf(stdout, "hw_rev: v%d.%d.%c%c\n", e->major, e->minor,e->errata[0],e->errata[1]);
	} else {
		fprintf(stdout, "hw_rev: v%d.%d\n", e->major, e->minor);
	}
	fprintf(stdout, "serialnumber: %s\n", e->sn);
	fprintf(stdout, "build date: %02d.%02d.20%02d %02d:%02d:%02d\n", e->date.DD, e->date.MM, e->date.YY, e->date.hh, e->date.mm, e->date.ss);
	fprintf(stdout, "mac flags: %02X\n", e->macflags);
	for (uint8_t i = 0; i < 6 && i < e->macsize; i++) {
		fprintf(stdout, "mac%d: %02X:%02X:%02X:%02X:%02X:%02X\n", i+1, e->mac[i][0],e->mac[i][1],e->mac[i][2],e->mac[i][3],e->mac[i][4],e->mac[i][5]);
	}
	fprintf(stdout, "CRC: %08X\n", e->crc32);
}


void write_hw_rev(systemid_t *e, char *hw_rev) {
	//get major number:
	char *tok = strtok(hw_rev, "v.");
	if (tok != NULL) {
		printf("major: %s\n", tok);
		uint8_t num = (uint8_t) strtol(tok, NULL, 10);
		e->major = num;

		//get minor number:
		tok = strtok(NULL, "v.");
		if (tok != NULL) {
			printf("minor: %s\n", tok);
			num = (uint8_t) strtol(tok, NULL, 10);
			e->minor = num;

			//get errata:
			tok = strtok(NULL, "v.");
			if (tok != NULL) {
				printf("errata: %s\n", tok);
				strncpy(e->errata,tok,2);
			}
		}
	}
}


void write_mac_address(uint8_t *emac, char *mac) {
	char *tok = strtok(mac, ":");
	for (uint8_t i = 0; i < 6 && tok != NULL; i++) {
		uint8_t block = (uint8_t) strtol(tok, NULL, 16);
		emac[i] = block;
		tok = strtok(NULL, ":");
	}
}