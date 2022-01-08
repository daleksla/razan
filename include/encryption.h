#ifndef ENCRYPTION_H
#define ENCRYPTION_H
#pragma once

/** @brief File contains declarations of supporting functionality to encrypt and decrypt data **/

struct TwoKeys {
    /** @brief struct TwoKeys - struct to easily transport a set of keys at a time **/
    unsigned long long int key_a ;

    unsigned long long int key_b ;
} ;

struct TextData {
    /** @brief struct TextData - stores array containing plaintext, ciphertext and their respective sizes **/
    unsigned char* plain_text ;

    int plain_text_len ;

    unsigned char* cipher_text ;

    int cipher_text_len ;

    unsigned long long max_len ;

} ;

struct EncryptionDetails {
    /** @brief struct EncryptionDetails - stores immutable key and IV for encryption / decryption purposes **/
    unsigned int enc_len ;

    unsigned char* key ;

    unsigned char* iv ;

} ;

enum USER_t {
    /** @brief enum USER - const enum for caller to reflect their status **/
    SERVER,

    CLIENT
} ;

/** @brief text_data_init - initialises text buffers
  * @param struct TextData* - pointer to TextData struct to which it's values are then initialised **/
void text_data_init(struct TextData*) ;

/** @brief text_data_grow - increases size of internal text buffers and relevant properties
  * @param struct TextData* - pointer to TextData struct to which it's values within are the grown **/
void text_data_grow(struct TextData*) ;

/** @brief text_data_fini - release memory held by text buffers
  * @param struct TextData* - pointer to TextData struct to which it's values are released **/
void text_data_fini(struct TextData*) ;

/** @brief encryption_details_init - release memory held by encryption buffers
  * @param struct EncryptionDetails* - pointer to EncryptionDetails struct containing pointers to initialises **/
void encryption_details_init(struct EncryptionDetails*) ;

/** @brief encryption_details_fini - release memory held by encryption buffers
  * @param struct EncryptionDetails* - pointer to EncryptionDetails struct to which it's values are released **/
void encryption_details_fini(struct EncryptionDetails*) ;

/** @brief generate_random_number - uses openssl library to perform a hardware-based random number generation
  * @return unsigned long long int - number containing randomly generated number between 0 & 18446744073709551615 **/
unsigned long long int generate_random_number(void) ;

/** @brief generate_public_keys - function generates public keys P & G
  * @return struct TwoKeys - struct contains generated and verified public key values **/
struct TwoKeys generate_public_keys(void) ;

/** @brief generate_secret_keys - function generates secret keys, one being private and the other a mashed version allowed to be sent over the network
  * @param const struct TwoKeys* - immutable pointer to existing struct contains generated and verified public key values
  * @return struct TwoKeys - struct contains generated and verified private and mashed edition of key values **/
struct TwoKeys generate_secret_keys(const struct TwoKeys*) ;

/** @brief generate_symmetric_keys - function generates shared (symmetric) key using ones private and main public key, and anothers mashed version of their private key
  * @param const unsigned long long - const number storing the other persons mashed version of their private key
  * @param const unsigned long long - const number storing the callers private key
  * @param const unsigned long long - const number storing the callers main public key
  * @return unsigned long long - number storing the symmetric key value (which will be calculated the same by both the caller and the 2nd party) **/
unsigned long long generate_symmetric_key(const unsigned long long, const unsigned long long, const unsigned long long) ;

/** @brief encryption_setup - meta function to setup and determine symmetric key between caller and endpoint client
  * @param const int - number storing socket number
  * @param const enum USER_t - number storing user level
  * @return struct EncryptionDetails - struct containing final encryption details **/
struct EncryptionDetails encryption_setup(const int, const enum USER_t) ;

/** @brief encrypt - encrypts message data
  * @param const struct* EncryptionDetails - const pointer to EncryptionDetails struct containing encryption details
  * @param struct TextData* - pointer to TextData struct containing plaintext and ciphertext buffers **/
void encrypt(const struct EncryptionDetails*, struct TextData*) ;

/** @brief decrypt - decrypts message data
  * @param const struct* EncryptionDetails - const pointer to EncryptionDetails struct containing encryption details
  * @param struct TextData* - pointer to TextData struct containing plaintext and ciphertext buffers **/
void decrypt(const struct EncryptionDetails*, struct TextData*) ;

#endif // ENCRYPTION_H
