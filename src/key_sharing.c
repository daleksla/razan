#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "key_sharing.h"

/** @brief Implementations of encryption functionalities (&helpful local inlines) for razan terminal messaging
  * @author Salih Ahmed
  * @date 16 Oct 2021 **/

static inline long long int power(long long int a, long long int b, long long int P) // Power function to return value of a ^ b mod P
{
	if(b == 1)
	{
		return a ;
	}
	else {
		return (((long long int)pow(a, b)) % P) ;
	}
}

two_keys generate_public_keys(void) // program run by server, just do it inline now
{
	/* create public keys - created&distributed by server */
	const long long int P = 23 ;
	const long long int G = 9 ;
	
	two_keys tmp ;
	tmp.key_a = P ;
	tmp.key_b = G ;
	return tmp ;
}

two_keys generate_secret_keys(const two_keys* public_keys) // ran by each client, takes public keys
{
	int f ;
	srand(f) ;
	const long long int private_key = rand() ;	
	const long long int mashed_key = power(public_keys->key_b, private_key, public_keys->key_a) ; // mash up private key to create a secret key

	two_keys tmp ;
	tmp.key_a = private_key ;
	tmp.key_b = mashed_key ;
	return tmp ;
}

long long int generate_symmetric_key(const long long int her_mashed_key, const long long int my_private_key, const long long int prime_public) // ran by each client, using others secret, their private keys and the prime public
{
	return power(her_mashed_key, my_private_key, prime_public) ;
}
