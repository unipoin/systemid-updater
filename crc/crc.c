//
// Created by Felix Hübner on 2020-09-21.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <zlib.h>
#include "eeprom.h"
#include "crc.h"



#ifndef cpu_to_be32

#define cpu_to_be32(crc) (htonl (crc))
#endif


void update_crc(systemid_t *e) {
	uint32_t crc = (uint32_t)  crc32(0L, (void *) e, EEPROM_SIZE - 4);
	fprintf(stdout,"update CRC! (%08X -> %08X)\n", e->crc32, cpu_to_be32(crc));
	e->crc32 = cpu_to_be32(crc);
}


uint8_t check_crc(systemid_t *e) {
	uint32_t crc = (uint32_t)  crc32(0L, (void *) e, EEPROM_SIZE - 4);
	if (cpu_to_be32(crc) == e->crc32) {
		return EXIT_SUCCESS;
	} else {
		fprintf(stderr, "CRC invalid: %08X != %08X!\n",e->crc32, cpu_to_be32(crc));
		return EXIT_FAILURE;
	}
}