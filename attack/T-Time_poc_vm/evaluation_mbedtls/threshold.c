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

	for (int count = 0; count < 15000; count++) {
		mbedtls_mpi A, E, N, X;                       

		mbedtls_mpi_init(&A);
		mbedtls_mpi_init(&E);
		mbedtls_mpi_init(&N);                         
		mbedtls_mpi_init(&X);

		mbedtls_mpi_read_string(&A, 16, RSA_Message);
		mbedtls_mpi_read_string(&E, 16, RSA_D_8); 
		mbedtls_mpi_read_string(&N, 16, RSA_N_8);

		ret = mbedtls_mpi_exp_mod(&X, &A, &E, &N, NULL);

		mbedtls_mpi_free(&A);
		mbedtls_mpi_free(&E);
		mbedtls_mpi_free(&N);
		mbedtls_mpi_free(&X);
	}

	printf("threshold done!\n");
	return 0;
}

