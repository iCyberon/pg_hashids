#ifndef HASHIDS_H
#define HASHIDS_H 1

#include <stdlib.h>

/* version constants */
#define HASHIDS_VERSION "1.2.1"
#define HASHIDS_VERSION_MAJOR 1
#define HASHIDS_VERSION_MINOR 2
#define HASHIDS_VERSION_PATCH 1

/* minimal alphabet length */
#define HASHIDS_MIN_ALPHABET_LENGTH 16u

/* separator divisor */
#define HASHIDS_SEPARATOR_DIVISOR 3.5f

/* guard divisor */
#define HASHIDS_GUARD_DIVISOR 12u

/* default salt */
#define HASHIDS_DEFAULT_SALT ""

/* default minimal hash length */
#define HASHIDS_DEFAULT_MIN_HASH_LENGTH 0u

/* default alphabet */
#define HASHIDS_DEFAULT_ALPHABET "abcdefghijklmnopqrstuvwxyz" \
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                                 "1234567890"

/* default separators */
#define HASHIDS_DEFAULT_SEPARATORS "cfhistuCFHISTU"

/* error codes */
#define HASHIDS_ERROR_OK                0
#define HASHIDS_ERROR_ALLOC             -1
#define HASHIDS_ERROR_ALPHABET_LENGTH   -2
#define HASHIDS_ERROR_ALPHABET_SPACE    -3
#define HASHIDS_ERROR_INVALID_HASH      -4
#define HASHIDS_ERROR_INVALID_NUMBER    -5

/* thread-safe hashids_errno indirection */
extern int *__hashids_errno_addr(void);
#define hashids_errno (*__hashids_errno_addr())

/* alloc & free */
extern void *(*_hashids_alloc)(size_t size);
extern void (*_hashids_free)(void *ptr);

/* the hashids "object" */
struct hashids_s {
    char *alphabet;
    char *alphabet_copy_1;
    char *alphabet_copy_2;
    size_t alphabet_length;

    char *salt;
    size_t salt_length;

    char *separators;
    size_t separators_count;

    char *guards;
    size_t guards_count;

    size_t min_hash_length;
};
typedef struct hashids_s hashids_t;

/* exported function definitions */
void
hashids_shuffle(char *str, size_t str_length, char *salt, size_t salt_length);

void
hashids_free(hashids_t *hashids);

hashids_t *
hashids_init3(const char *salt, size_t min_hash_length,
    const char *alphabet);

hashids_t *
hashids_init2(const char *salt, size_t min_hash_length);

hashids_t *
hashids_init(const char *salt);

size_t
hashids_estimate_encoded_size(hashids_t *hashids, size_t numbers_count,
    unsigned long long *numbers);

size_t
hashids_estimate_encoded_size_v(hashids_t *hashids, size_t numbers_count, ...);

size_t
hashids_encode(hashids_t *hashids, char *buffer, size_t numbers_count,
    unsigned long long *numbers);

size_t
hashids_encode_v(hashids_t *hashids, char *buffer, size_t numbers_count, ...);

size_t
hashids_encode_one(hashids_t *hashids, char *buffer,
    unsigned long long number);

size_t
hashids_numbers_count(hashids_t *hashids, const char *str);

size_t
hashids_decode(hashids_t *hashids, const char *str,
    unsigned long long *numbers, size_t numbers_max);

size_t
hashids_decode_unsafe(hashids_t *hashids, const char *str,
    unsigned long long *numbers);

size_t
hashids_decode_safe(hashids_t *hashids, const char *str,
    unsigned long long *numbers, size_t numbers_max);

size_t
hashids_encode_hex(hashids_t *hashids, char *buffer, const char *hex_str);

size_t
hashids_decode_hex(hashids_t *hashids, char *str, char *output);

#endif
