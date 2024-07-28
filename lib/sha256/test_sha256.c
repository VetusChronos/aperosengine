#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "my_sha256.h"

unsigned char* SHA256(const unsigned char *d, size_t n, unsigned char *md);

void print_hash(const unsigned char *hash) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

void hex_to_bytes(const char *hex, unsigned char *bytes, size_t len) {
    for (size_t i = 0; i < len; i++) {
        sscanf(hex + 2 * i, "%2hhx", &bytes[i]);
    }
}

int main() {
    const char *input = "Aperos!";
    unsigned char hash[SHA256_DIGEST_LENGTH];
    const char *expected_hash_hex = "f4ece09ad33a796bb94fba40419df3c8ffcb133e0fc6ec83ad55d024d3b1f511";
    unsigned char expected_hash[SHA256_DIGEST_LENGTH];

    // Converts the expected hash from hexadecimal to bytes
    hex_to_bytes(expected_hash_hex, expected_hash, SHA256_DIGEST_LENGTH);

    // Calculate the hash
    SHA256((const unsigned char *)input, strlen(input), hash);

    printf("Calculated SHA256 hash: ");
    print_hash(hash);

    printf("Expected SHA256 hash: ");
    print_hash(expected_hash);

    if (memcmp(hash, expected_hash, SHA256_DIGEST_LENGTH) == 0) {
        printf("Test passed!\n");
    } else {
        printf("Test failed.\n");
    }

    return 0;
}
