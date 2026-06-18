/**
 * @file    flash_storage.h
 * @brief   Flash persistent storage module for GD32F470IGH6
 *
 * Uses the last 128KB Flash sector (0x080E0000 - 0x080EFFFF) for
 * persistent data storage. Data is organized as key-value pairs
 * with a simple header for validity checking.
 */

#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include <stdint.h>
#include <stddef.h>

/* Flash storage base address - last 128KB sector */
#define FLASH_STORAGE_BASE_ADDR     0x080E0000U
#define FLASH_STORAGE_SIZE          0x00020000U  /* 128KB */

/* Magic number to identify valid data */
#define FLASH_STORAGE_MAGIC         0x53544F52U  /* "STOR" */

/* Maximum key length */
#define FLASH_STORAGE_MAX_KEY_LEN   32U

/* Maximum value length per entry */
#define FLASH_STORAGE_MAX_VALUE_LEN 256U

/* Return codes */
#define FLASH_STORAGE_OK            0
#define FLASH_STORAGE_ERR_FULL     -1
#define FLASH_STORAGE_ERR_NOT_FOUND -2
#define FLASH_STORAGE_ERR_WRITE    -3
#define FLASH_STORAGE_ERR_ERASE    -4
#define FLASH_STORAGE_ERR_INVALID  -5

/**
 * @brief Initialize flash storage module
 * @return FLASH_STORAGE_OK on success
 */
int flash_storage_init(void);

/**
 * @brief Save a key-value pair to flash
 * @param key   Key name (null-terminated string, max FLASH_STORAGE_MAX_KEY_LEN)
 * @param value Pointer to data to save
 * @param len   Length of data in bytes
 * @return FLASH_STORAGE_OK on success
 */
int flash_storage_save(const char *key, const void *value, uint16_t len);

/**
 * @brief Load a value by key from flash
 * @param key     Key name to search for
 * @param value   Buffer to store the loaded data
 * @param max_len Maximum buffer size
 * @param out_len Pointer to store actual data length (can be NULL)
 * @return FLASH_STORAGE_OK on success, FLASH_STORAGE_ERR_NOT_FOUND if key doesn't exist
 */
int flash_storage_load(const char *key, void *value, uint16_t max_len, uint16_t *out_len);

/**
 * @brief Delete a key-value pair from flash
 * @param key Key name to delete
 * @return FLASH_STORAGE_OK on success
 */
int flash_storage_delete(const char *key);

/**
 * @brief Erase all stored data
 * @return FLASH_STORAGE_OK on success
 */
int flash_storage_erase_all(void);

/**
 * @brief Get used space in flash storage
 * @return Number of bytes used
 */
uint32_t flash_storage_get_used(void);

/**
 * @brief Get free space in flash storage
 * @return Number of bytes free
 */
uint32_t flash_storage_get_free(void);

#endif /* FLASH_STORAGE_H */
