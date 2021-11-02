#ifndef CRYPT_H
#define CRYPT_H
#pragma once

/** @brief Definitions of encryption functionalities for razan terminal messaging **/

struct two_keys {

	unsigned long int key_a ;
	
	unsigned long int key_b ;
	
} ;

/** @brief generate_public_keys - function ran by server to creates public keys (P&G) used in Duffie-Hellman key-sharing implementation
  * @return struct two_keys - structure containing public keys **/
struct two_keys generate_public_keys(void) ;

/** @brief generate_secret_keys - ran by each client, takes public keys, returns private (to keep) and mashed key (to share)
  * @param const struct two_keys* - pointer to public keys
  * @return struct two_keys - pointer containing private-esque keys **/
struct two_keys generate_secret_keys(const struct two_keys*) ;

/** @brief generate_symmetric_key - ran by each client, using others secret, their private keys and the prime public to create symmetric key for message encryption
  * @param const unsigned long int- other clients mashed key
  * @param const unsigned long int - my private key
  * @param const unsigned long int - prime public key
  * @return unsigned long int - symmetric key **/
unsigned long int generate_symmetric_key(const unsigned long int, const unsigned long int, const unsigned long int) ; 

#endif // CRYPT_H
