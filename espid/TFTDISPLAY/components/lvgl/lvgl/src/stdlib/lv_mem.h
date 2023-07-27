/**
 * @file lv_mem.h
 *
 */

#ifndef LV_MEM_H
#define LV_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "C:\Users\YUVA\Desktop\espid\TFTDISPLAY\components\lvgl\lvgl\lvgl.h\lv_conf_internal.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void * lv_mem_pool_t;

/**
 * Heap information structure.
 */
typedef struct {
    uint32_t total_size; /**< Total heap size*/
    uint32_t free_cnt;
    uint32_t free_size; /**< Size of available memory*/
    uint32_t free_biggest_size;
    uint32_t used_cnt;
    uint32_t max_used; /**< Max size of Heap memory used*/
    uint8_t used_pct; /**< Percentage used*/
    uint8_t frag_pct; /**< Amount of fragmentation*/
} lv_mem_monitor_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize to use malloc/free/realloc etc
 */
void lv_mem_init(void);

/**
 * Drop all dynamically allocated memory and reset the memory pools' state
 */
void lv_mem_deinit(void);

lv_mem_pool_t lv_mem_add_pool(void * mem, size_t bytes);

void lv_mem_remove_pool(lv_mem_pool_t pool);

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory
 */
void * lv_malloc(size_t size);

/**
 * Free an allocated data
 * @param data pointer to an allocated memory
 */
void lv_free(void * data);

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data_p pointer to an allocated memory.
 *               Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory, NULL on failure
 */
void * lv_realloc(void * data_p, size_t new_size);


/**
 * Used internally to execute a plain `malloc` operation
 * @param size      size in bytes to `malloc`
 */
void * lv_malloc_core(size_t size);



/**
 * Used internally to execute a plain `free` operation
 * @param p      memory address to free
 */
void lv_free_core(void * p);

/**
 * Used internally to execute a plain realloc operation
 * @param p         memory address to realloc
 * @param new_size  size in bytes to realloc
 */
void * lv_realloc_core(void * p, size_t new_size);

/**
 * Used internally to execute a plain malloc operation
 * @param size      size in bytes to malloc
 */
void lv_mem_monitor_core(lv_mem_monitor_t * mon_p);


lv_res_t lv_mem_test_core(void);

/**
 * @brief Tests the memory allocation system by allocating and freeing a block of memory.
 * @return LV_RES_OK if the memory allocation system is working properly, or LV_RES_INV if there is an error.
 */
lv_res_t lv_mem_test(void);

/**
 * Give information about the work memory of dynamic allocation
 * @param mon_p pointer to a lv_mem_monitor_t variable,
 *              the result of the analysis will be stored here
 */
void lv_mem_monitor(lv_mem_monitor_t * mon_p);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MEM_H*/
