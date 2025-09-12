#include "mbedtls/bignum.h"
#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"
#include "mbedtls/pem.h"
#include <stdio.h>
#include <time.h> 

#include "pmparser.h"
#include "parse_pagemap.h"
#include "introspection.h"
#include <sys/mman.h>

int long_key = 1;
const char *key_file = "./secret/private_key_512.pem";
const char *password = NULL;             

#define RSA_Message "2CF24DBA5FB0A30E26E83B2AC5B9E29E1B161E5C1FA7425E73043362938B9824"

#define RSA_N_8 "8A1F4B"
#define RSA_D_8 "F0"

int main() {
	int ret;
	int k;

	const uint64_t ida[CodeLocationLength] = {
		0x00023000, // mpi_select
		0x00027000, // mbedtls_mpi_core_montmul			    
		0x00025000, // mbedtls_mpi_exp_mod

	};

	const int total_count = 300; 

	clock_t start_time = clock();  // Start time

	for (int count = 0; count < total_count; count++) { 
		mbedtls_mpi M, D, N, R;

		mbedtls_mpi_init(&M); 
		mbedtls_mpi_init(&D); 
		mbedtls_mpi_init(&N); 
		mbedtls_mpi_init(&R); 

		mbedtls_mpi_read_string(&M, 16, RSA_Message);
		mbedtls_pk_context pk;

		mbedtls_pk_init(&pk);

		ret = mbedtls_pk_parse_keyfile(&pk, key_file, password, NULL, NULL);
		if (ret != 0) {
			printf("Failed to parse private key file, error code: -0x%04x\n", -ret);
			return -1;
		}

		if (mbedtls_pk_get_type(&pk) != MBEDTLS_PK_RSA) {
			printf("Not an RSA private key!\n");
			mbedtls_pk_free(&pk);
			return -1;
		}

		mbedtls_rsa_context *rsa = mbedtls_pk_rsa(pk);

		ret = mbedtls_rsa_export(rsa, &N, NULL, NULL, &D, NULL);

		if (ret != 0) {
			printf("Failed to export RSA key components, error code: -0x%04x\n", -ret);
			return -1;
		}
		mbedtls_pk_free(&pk);

		ret = mbedtls_mpi_exp_mod(&R, &M, &D, &N, NULL);

		mbedtls_mpi_free(&M);
		mbedtls_mpi_free(&D);
		mbedtls_mpi_free(&N);
		mbedtls_mpi_free(&R);
	}
	clock_t end_time = clock();
	double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
	printf("RSA exponent calculation !\nExecution Time: %.2f seconds\n", elapsed_time);
	return 0;
}

