#ifndef CRYPT_H
#define CRYPT_H

/** @brief Definitions of encryption functionalities for razan terminal messaging
  * @author Salih Ahmed
  * @date 16 Oct 2021 **/

typedef struct {

	long long int key_a ;
	
	long long int key_b ;
	
} two_keys ;

// ran by server, creates public keys
two_keys generate_public_keys(void) ;

// ran by each client, takes public keys, returns private (to keep) and mashed key (to share)
two_keys generate_secret_keys(const two_keys*) ;

// ran by each client, using others secret, their private keys and the prime public to create key for all messages
long long int generate_symmetric_key(const long long int, const long long int, const long long int) ; 

#endif // CRYPT_H
