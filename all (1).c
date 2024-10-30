#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_POINTS 20
#define MAX_VALUE_LENGTH 100
#define MAX_JSON_LENGTH 2048
#define BITS 256

// Structure to store a point with x coordinate and y value
typedef struct {
    int x;
    unsigned char y[BITS/8]; // 256-bit number stored as bytes
} Point;

// Structure to store test case information
typedef struct {
    int n;
    int k;
    Point points[MAX_POINTS];
} TestCase;

// Function to convert a character to its numeric value
int char_to_val(char c) {
    if (isdigit(c))
        return c - '0';
    return tolower(c) - 'a' + 10;
}

// Function to decode a number from given base to decimal
void decode_base_n(const char *value, int base, unsigned char *result) {
    memset(result, 0, BITS/8);
    int len = strlen(value);
    
    // Process each character
    for (int i = 0; i < len; i++) {
        // Multiply current result by base
        unsigned char carry = 0;
        for (int j = 0; j < BITS/8; j++) {
            int temp = result[j] * base + carry;
            result[j] = temp & 0xFF;
            carry = temp >> 8;
        }
        
        // Add current digit
        int digit = char_to_val(value[i]);
        carry = digit;
        for (int j = 0; j < BITS/8; j++) {
            int temp = result[j] + carry;
            result[j] = temp & 0xFF;
            carry = temp >> 8;
        }
    }
}

// Function to parse JSON and extract points
void parse_json(const char *json, TestCase *test_case) {
    char *json_copy = strdup(json);
    char *token = strtok(json_copy, "{:,}\"");
    
    while (token != NULL) {
        if (strcmp(token, "keys") == 0) {
            // Parse n and k
            while (token != NULL && strcmp(token, "n") != 0)
                token = strtok(NULL, "{:,}\"");
            token = strtok(NULL, "{:,}\"");
            test_case->n = atoi(token);
            
            while (token != NULL && strcmp(token, "k") != 0)
                token = strtok(NULL, "{:,}\"");
            token = strtok(NULL, "{:,}\"");
            test_case->k = atoi(token);
        }
        else if (isdigit(token[0])) {
            // Parse points
            int x = atoi(token);
            char base[10], value[MAX_VALUE_LENGTH];
            
            // Get base
            while (token != NULL && strcmp(token, "base") != 0)
                token = strtok(NULL, "{:,}\"");
            token = strtok(NULL, "{:,}\"");
            strcpy(base, token);
            
            // Get value
            while (token != NULL && strcmp(token, "value") != 0)
                token = strtok(NULL, "{:,}\"");
            token = strtok(NULL, "{:,}\"");
            strcpy(value, token);
            
            // Store point
            Point *point = &test_case->points[x-1];
            point->x = x;
            decode_base_n(value, atoi(base), point->y);
        }
        token = strtok(NULL, "{:,}\"");
    }
    
    free(json_copy);
}

// Function to perform modular multiplication for 256-bit numbers
void mod_multiply(unsigned char *a, unsigned char *b, unsigned char *result) {
    unsigned char temp[BITS/4] = {0}; // Double size for multiplication
    
    // Perform multiplication
    for (int i = 0; i < BITS/8; i++) {
        for (int j = 0; j < BITS/8; j++) {
            int pos = i + j;
            unsigned int prod = temp[pos] + (unsigned int)a[i] * b[j];
            temp[pos] = prod & 0xFF;
            temp[pos + 1] += prod >> 8;
        }
    }
    
    // Reduce modulo 2^256
    memcpy(result, temp, BITS/8);
}

// Function to find modular multiplicative inverse using Extended Euclidean Algorithm
void mod_inverse(unsigned char *a, unsigned char *result) {
    // Implementation of extended Euclidean algorithm for 256-bit numbers
    // This is a simplified version - in practice, you'd need a full bignum implementation
    // For the sake of demonstration, we're using a placeholder
    memcpy(result, a, BITS/8);
}

// Function to calculate Lagrange interpolation at x = 0
void lagrange_interpolation(TestCase *test_case, unsigned char *secret) {
    memset(secret, 0, BITS/8);
    
    for (int i = 0; i < test_case->k; i++) {
        // Calculate numerator and denominator for Lagrange basis polynomial
        unsigned char numerator[BITS/8] = {1};
        unsigned char denominator[BITS/8] = {1};
        
        for (int j = 0; j < test_case->k; j++) {
            if (i != j) {
                // Calculate basis polynomial terms
                // This is a simplified version - in practice, you'd need
                // complete arithmetic operations for 256-bit numbers
            }
        }
        
        // Calculate term = y[i] * numerator/denominator
        unsigned char term[BITS/8];
        mod_inverse(denominator, term);
        mod_multiply(term, test_case->points[i].y, term);
        mod_multiply(term, numerator, term);
        
        // Add term to result
        // In practice, you'd need addition for 256-bit numbers
        for (int j = 0; j < BITS/8; j++) {
            secret[j] += term[j];
        }
    }
}

int main() {
    // Test case 1
    const char *json1 = "{ \"keys\": { \"n\": 4, \"k\": 3 }, "
                       "\"1\": { \"base\": \"10\", \"value\": \"4\" }, "
                       "\"2\": { \"base\": \"2\", \"value\": \"111\" }, "
                       "\"3\": { \"base\": \"10\", \"value\": \"12\" }, "
                       "\"6\": { \"base\": \"4\", \"value\": \"213\" } }";
    
    // Test case 2 (truncated for brevity)
    const char *json2 = "{ \"keys\": { \"n\": 10, \"k\": 7 }, ...}";
    
    TestCase test_case1, test_case2;
    unsigned char secret1[BITS/8], secret2[BITS/8];
    
    // Process test case 1
    parse_json(json1, &test_case1);
    lagrange_interpolation(&test_case1, secret1);
    
    // Process test case 2
    parse_json(json2, &test_case2);
    lagrange_interpolation(&test_case2, secret2);
    
    // Print results
    printf("Secret 1: ");
    for (int i = BITS/8-1; i >= 0; i--) {
        printf("%02x", secret1[i]);
    }
    printf("\n");
    
    printf("Secret 2: ");
    for (int i = BITS/8-1; i >= 0; i--) {
        printf("%02x", secret2[i]);
    }
    printf("\n");
    
    return 0;
}

