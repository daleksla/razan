#ifndef ENCRYPTION_HPP
#define ENCRYPTION_HPP
#pragma once


#include <cstddef>
#include <string>
#include <array>
#include <cstdint>
#include <tuple>

#include "types.hpp"

/**
 * @brief File contains declarations of supporting functionality to encrypt and decrypt data
 * @author Salih MSA
 */


#define AES_BLOCK_SIZE 16

namespace razan {

	/**
	 * @brief struct EncryptionDetails - stores immutable key and IV for encryption / decryption purposes
	 */
	struct EncryptionDetails {
		key_t key ;

		key_t iv ;
	} ;


	/**
	 * @brief generate_random_number - uses openssl library to perform a hardware-based random number generation
	 * @param key_t - number containing randomly generated number between 0 & 18446744073709551615
	 */
	key_t generate_random_number() noexcept ;


	/**
	 * @brief generate_public_keys - function generates public keys P & G
	 * @return std::tuple<key_t, key_t> - public key P and G
	 */
	std::tuple<key_t, key_t> generate_public_keys() noexcept ;

	/**
	 * @brief generate_secret_keys - function generates secret keys, one being private and the other a mashed version allowed to be sent over the network
	 * @param const key_t& p_key - P key
	 * @param const key_t& g_key - G key
	 * @param std::tuple<key_t, key_t> - private and mashed keys
	 */
	std::tuple<key_t, key_t> generate_secret_keys(const key_t& p_key, const key_t& g_key) noexcept ;


	/**
	 * @brief generate_symmetric_keys - function generates shared (symmetric) key using ones private and main public key, and anothers mashed version of their private key
	 * @param const key_t& their_mashed_key - const number storing the other persons mashed version of their private key
	 * @param const key_t& my_private_key - const number storing the callers private key
	 * @param const key_t& prime_public - const number storing the callers main public key
	 * @return key_t - number storing the symmetric key value (which will be calculated the same by both the caller and the 2nd party)
	 */
	key_t generate_symmetric_key(const key_t& their_mashed_key, const key_t& my_private_key, const key_t& prime_public) noexcept ;


	/**
	 * @brief encrypt - encrypts message data
	 * @param const EncryptionDetails& enc_details - EncryptionDetails struct containing encryption details
	 * @param const std::u8string& - string containing to-be-encrypted data as serialised unsigned characters
	 * @return std::u8string - string containing encrypted data as serialised unsigned characters
	 */
	std::u8string encrypt(const EncryptionDetails& enc_details, const std::u8string& txt) noexcept ;


	/**
	 * @brief decrypt - decrypts message data
	 * @param const EncryptionDetails& enc_details - EncryptionDetails struct containing encryption details
	 * @param const std::u8string& - string containing encrypted data as serialised unsigned characters
	 * @return std::u8string - string containing decrypted data as serialised unsigned characters
	 */
	std::u8string decrypt(const EncryptionDetails& enc_details, const std::u8string& txt) noexcept ;

} // namespace razan

#endif // ENCRYPTION_HPP
