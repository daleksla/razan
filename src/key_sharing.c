#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "key_sharing.h"

/** @brief Implementations of encryption functionalities (&helpful local inlines) for razan terminal messaging **/

static inline unsigned long int modpow(long long int a, long long int b, long long int P) // Power function to return value of a ^ b mod P
{
	if(b == 1)
	{
		return a ;
	}
	else {
		return (((unsigned long int)pow(a, b)) % P) ;
	}
}

struct two_keys generate_public_keys(void) // program run by server, just do it inline now
{
	/* create public keys - created&distributed by server */
	const unsigned long int P = 23 ;
	const unsigned long int G = 9 ;
	
	struct two_keys tmp = {P, G} ;
	return tmp ;
}

struct two_keys generate_secret_keys(const struct two_keys* public_keys) // ran by each client, takes public keys
{
    srand(time(NULL) ^ getpid() ^ pthread_self()) ; // try to generate a very random speed
	const unsigned long int private_key = rand() ;	
	const unsigned long int mashed_key = modpow(public_keys->key_b, private_key, public_keys->key_a) ; // mash up private key to create a secret key

	struct two_keys tmp = {private_key, mashed_key} ;
	return tmp ;
}

unsigned long int generate_symmetric_key(const unsigned long int her_mashed_key, const unsigned long int my_private_key, const unsigned long int prime_public) // ran by each client, using others secret, their private keys and the prime public
{
	return modpow(her_mashed_key, my_private_key, prime_public) ;
}
