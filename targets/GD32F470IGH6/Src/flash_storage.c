/**
 * @file    flash_storage.c
 * @brief   Flash persistent storage module for GD32F470IGH6
 *
 * Uses the last 128KB Flash sector (0x080E0000 - 0x080EFFFF) for
 * persistent data storage. Data is organized as key-value pairs
 * with a simple header for validity checking.
 *
 * Storage format:
 * [Entry Header][Key][Value][Padding to 4-byte alignment]
 *
 * Entry Header (16 bytes):
 *   - magic (4 bytes): 0x53544F52 for valid, 0xFFFFFFFF for erased
 *   - key_len (2 bytes): length of key including null terminator
 *   - value_len (2 bytes): length of value data
 *   - reserved (8 bytes): reserved for future use
 */

#include "flash_storage.h"
#include "gd32f4xx_fmc.h"
#include <string.h>
#include <stdio.h>

/* Entry header structure */
typedef struct {
    uint32_t magic;      /* Magic number */
    uint16_t key_len;    /* Key length (including null terminator) */
    uint16_t value_len;  /* Value data length */
    uint32_t reserved[2];/* Reserved */
} flash_entry_header_t;

#define ENTRY_HEADER_SIZE   sizeof(flash_entry_header_t)
#define ALIGN4(size)        (((size) + 3U) & ~3U)

/* Check if flash is erased (all 0xFF) */
static int is_erased(uint32_t addr, uint32_t size)
{
    const uint8_t *p = (const uint8_t *)addr;
    for (uint32_t i = 0; i < size; i++) {
        if (p[i] != 0xFF) {
            return 0;
        }
    }
    return 1;
}

/* Find next valid entry starting from given address */
static uint32_t find_next_entry(uint32_t addr)
{
    while (addr < FLASH_STORAGE_BASE_ADDR + FLASH_STORAGE_SIZE) {
        flash_entry_header_t *header = (flash_entry_header_t *)addr;

        /* Check if area is erased */
        if (is_erased(addr, ENTRY_HEADER_SIZE)) {
            return addr; /* Found empty space */
        }

        /* Check magic number */
        if (header->magic == FLASH_STORAGE_MAGIC) {
            /* Valid entry, skip to next */
            uint32_t entry_size = ENTRY_HEADER_SIZE +
                                  ALIGN4(header->key_len) +
                                  ALIGN4(header->value_len);
            addr += entry_size;
            continue;
        }

        /* Invalid entry (deleted or corrupted), skip */
        addr += ENTRY_HEADER_SIZE;
    }

    return FLASH_STORAGE_BASE_ADDR + FLASH_STORAGE_SIZE; /* End of storage */
}

/* Find entry by key, returns address of header or 0 if not found */
static uint32_t find_entry_by_key(const char *key)
{
    uint32_t addr = FLASH_STORAGE_BASE_ADDR;
    uint32_t last_valid = 0;

    while (addr < FLASH_STORAGE_BASE_ADDR + FLASH_STORAGE_SIZE) {
        flash_entry_header_t *header = (flash_entry_header_t *)addr;

        /* Check if area is erased */
        if (is_erased(addr, ENTRY_HEADER_SIZE)) {
            break; /* End of data */
        }

        /* Check magic number */
        if (header->magic == FLASH_STORAGE_MAGIC) {
            /* Check if key matches */
            const char *entry_key = (const char *)(addr + ENTRY_HEADER_SIZE);
            if (header->key_len == strlen(key) + 1 &&
                memcmp(entry_key, key, header->key_len) == 0) {
                last_valid = addr; /* Found matching key, keep searching for newer version */
            }
            uint32_t entry_size = ENTRY_HEADER_SIZE +
                                  ALIGN4(header->key_len) +
                                  ALIGN4(header->value_len);
            addr += entry_size;
            continue;
        }

        /* Invalid entry, skip */
        addr += ENTRY_HEADER_SIZE;
    }

    return last_valid;
}

/* Calculate used space */
static uint32_t calculate_used_space(void)
{
    uint32_t addr = FLASH_STORAGE_BASE_ADDR;

    while (addr < FLASH_STORAGE_BASE_ADDR + FLASH_STORAGE_SIZE) {
        if (is_erased(addr, 4)) {
            break;
        }

        flash_entry_header_t *header = (flash_entry_header_t *)addr;
        if (header->magic == FLASH_STORAGE_MAGIC) {
            uint32_t entry_size = ENTRY_HEADER_SIZE +
                                  ALIGN4(header->key_len) +
                                  ALIGN4(header->value_len);
            addr += entry_size;
        } else {
            addr += ENTRY_HEADER_SIZE;
        }
    }

    return addr - FLASH_STORAGE_BASE_ADDR;
}

int flash_storage_init(void)
{
    /* Verify storage area is within valid flash range */
    if (FLASH_STORAGE_BASE_ADDR < 0x08000000U ||
        FLASH_STORAGE_BASE_ADDR + FLASH_STORAGE_SIZE > 0x08100000U) {
        return FLASH_STORAGE_ERR_INVALID;
    }

    return FLASH_STORAGE_OK;
}

int flash_storage_save(const char *key, const void *value, uint16_t len)
{
    if (key == NULL || value == NULL || len == 0 ||
        strlen(key) >= FLASH_STORAGE_MAX_KEY_LEN ||
        len > FLASH_STORAGE_MAX_VALUE_LEN) {
        return FLASH_STORAGE_ERR_INVALID;
    }

    uint16_t key_len = (uint16_t)(strlen(key) + 1);
    uint32_t entry_size = ENTRY_HEADER_SIZE + ALIGN4(key_len) + ALIGN4(len);

    /* Find write position (first erased area) */
    uint32_t write_addr = find_next_entry(FLASH_STORAGE_BASE_ADDR);

    /* Check if there's enough space */
    if (write_addr + entry_size > FLASH_STORAGE_BASE_ADDR + FLASH_STORAGE_SIZE) {
        /* Need to compact or erase - for now, return error */
        return FLASH_STORAGE_ERR_FULL;
    }

    /* Write entry header */
    flash_entry_header_t header;
    header.magic = FLASH_STORAGE_MAGIC;
    header.key_len = key_len;
    header.value_len = len;
    header.reserved[0] = 0xFFFFFFFF;
    header.reserved[1] = 0xFFFFFFFF;

    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR |
                   FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

    /* Write header */
    uint32_t *hdr_ptr = (uint32_t *)&header;
    for (uint32_t i = 0; i < ENTRY_HEADER_SIZE / 4; i++) {
        if (fmc_word_program(write_addr + i * 4, hdr_ptr[i]) != FMC_READY) {
            fmc_lock();
            return FLASH_STORAGE_ERR_WRITE;
        }
    }

    /* Write key */
    uint32_t offset = ENTRY_HEADER_SIZE;
    uint32_t aligned_key_len = ALIGN4(key_len);
    uint8_t key_buf[FLASH_STORAGE_MAX_KEY_LEN + 4] = {0};
    memcpy(key_buf, key, key_len);

    for (uint32_t i = 0; i < aligned_key_len / 4; i++) {
        uint32_t word;
        memcpy(&word, &key_buf[i * 4], 4);
        if (fmc_word_program(write_addr + offset + i * 4, word) != FMC_READY) {
            fmc_lock();
            return FLASH_STORAGE_ERR_WRITE;
        }
    }

    /* Write value */
    offset += aligned_key_len;
    uint32_t aligned_val_len = ALIGN4(len);
    uint8_t val_buf[FLASH_STORAGE_MAX_VALUE_LEN + 4] = {0};
    memcpy(val_buf, value, len);

    for (uint32_t i = 0; i < aligned_val_len / 4; i++) {
        uint32_t word;
        memcpy(&word, &val_buf[i * 4], 4);
        if (fmc_word_program(write_addr + offset + i * 4, word) != FMC_READY) {
            fmc_lock();
            return FLASH_STORAGE_ERR_WRITE;
        }
    }

    fmc_lock();
    return FLASH_STORAGE_OK;
}

int flash_storage_load(const char *key, void *value, uint16_t max_len, uint16_t *out_len)
{
    if (key == NULL || value == NULL) {
        return FLASH_STORAGE_ERR_INVALID;
    }

    uint32_t addr = find_entry_by_key(key);
    if (addr == 0) {
        return FLASH_STORAGE_ERR_NOT_FOUND;
    }

    flash_entry_header_t *header = (flash_entry_header_t *)addr;

    if (header->value_len > max_len) {
        return FLASH_STORAGE_ERR_INVALID;
    }

    /* Read value */
    uint32_t value_offset = addr + ENTRY_HEADER_SIZE + ALIGN4(header->key_len);
    memcpy(value, (const void *)value_offset, header->value_len);

    if (out_len != NULL) {
        *out_len = header->value_len;
    }

    return FLASH_STORAGE_OK;
}

int flash_storage_delete(const char *key)
{
    uint32_t addr = find_entry_by_key(key);
    if (addr == 0) {
        return FLASH_STORAGE_ERR_NOT_FOUND;
    }

    /* Invalidate by overwriting magic number with 0xFFFFFFFF */
    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR |
                   FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

    if (fmc_word_program(addr, 0xFFFFFFFF) != FMC_READY) {
        fmc_lock();
        return FLASH_STORAGE_ERR_WRITE;
    }

    fmc_lock();
    return FLASH_STORAGE_OK;
}

int flash_storage_erase_all(void)
{
    fmc_sector_info_struct sector_info;

    /* Get sector info for storage address */
    sector_info = fmc_sector_info_get(FLASH_STORAGE_BASE_ADDR);

    if (sector_info.sector_name == FMC_WRONG_SECTOR_NAME) {
        return FLASH_STORAGE_ERR_INVALID;
    }

    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR |
                   FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

    if (fmc_sector_erase(sector_info.sector_num) != FMC_READY) {
        fmc_lock();
        return FLASH_STORAGE_ERR_ERASE;
    }

    fmc_lock();
    return FLASH_STORAGE_OK;
}

uint32_t flash_storage_get_used(void)
{
    return calculate_used_space();
}

uint32_t flash_storage_get_free(void)
{
    return FLASH_STORAGE_SIZE - calculate_used_space();
}
