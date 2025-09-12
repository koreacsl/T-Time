#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"
#include "mbedtls/pem.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
	int ret;

	mbedtls_pk_context pk;
	const char *key_file = "private_key_512.pem"; 
	const char *password = NULL;            

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

	mbedtls_mpi N, P, Q, D, E;
	mbedtls_mpi_init(&N);
	mbedtls_mpi_init(&P);
	mbedtls_mpi_init(&Q);
	mbedtls_mpi_init(&D);
	mbedtls_mpi_init(&E);

	ret = mbedtls_rsa_export(rsa, &N, &P, &Q, &D, &E);
	if (ret != 0) {
		printf("Failed to export RSA key components, error code: -0x%04x\n", -ret);
		return -1;
	}

	mbedtls_mpi_write_file("N:  ",  &N,  16, NULL);
	mbedtls_mpi_write_file("E:  ",  &E,  16, NULL);
	mbedtls_mpi_write_file("D:  ",  &D,  16, NULL);
	mbedtls_mpi_write_file("P:  ",  &P,  16, NULL);
	mbedtls_mpi_write_file("Q:  ",  &Q,  16, NULL);

	mbedtls_mpi_free(&N);
	mbedtls_mpi_free(&P);
	mbedtls_mpi_free(&Q);
	mbedtls_mpi_free(&D);
	mbedtls_mpi_free(&E);
	mbedtls_pk_free(&pk);

	return 0;
}

