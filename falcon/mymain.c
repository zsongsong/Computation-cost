
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "katrng.h"
#include "api.h"
#include <time.h>
#include <windows.h>
#define	MAX_MARKER_LEN		50

#define KAT_SUCCESS          0
#define KAT_FILE_OPEN_ERROR -1
#define KAT_DATA_ERROR      -3
#define KAT_CRYPTO_FAILURE  -4

int
main()
{
	LARGE_INTEGER fre = { 0 };//储存本机CPU时钟频率
	LARGE_INTEGER startCount = { 0 };
	LARGE_INTEGER endCount = { 0 };
	QueryPerformanceFrequency(&fre);//获取本机cpu频率

	unsigned char* m, * sm, * m1;
	unsigned long long  mlen, smlen, mlen1;
	int                 count;
	int                 done;
	int                 ret_val;
	
	static unsigned char       seed[48];
	static unsigned char       entropy_input[48];
	static unsigned char       msg[3300];
	static unsigned char       pk[CRYPTO_PUBLICKEYBYTES], sk[CRYPTO_SECRETKEYBYTES];
	for (int i = 0; i < 48; i++)
		entropy_input[i] = i;

	randombytes_init(entropy_input, NULL, 256);

	randombytes(seed, 48);
	randombytes_init(seed, NULL, 256);
	mlen = 330;
	m = (unsigned char*)calloc(mlen, sizeof(unsigned char));
	m1 = (unsigned char*)calloc(mlen, sizeof(unsigned char));
	sm = (unsigned char*)calloc(mlen + CRYPTO_BYTES, sizeof(unsigned char));

	clock_t begin, end;
	double ttkeypair=0, ttsign=0, ttopen=0;
	int iter = 100;
	for (int i = 0; i < iter; i++)
	{
		randombytes(m, mlen);

		// Generate the public/private keypair
		QueryPerformanceCounter(&startCount);//计时开始
		ret_val = crypto_sign_keypair(pk, sk);
		QueryPerformanceCounter(&endCount);;//计时结束
		//QueryPerformanceFrequency(&fre);
		ttkeypair += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;

		QueryPerformanceCounter(&startCount);//计时开始
		ret_val = crypto_sign(sm, &smlen, m, mlen, sk);
		QueryPerformanceCounter(&endCount);;//计时结束
		//QueryPerformanceFrequency(&fre);
		ttsign += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;

		QueryPerformanceCounter(&startCount);//计时开始
		ret_val = crypto_sign_open(m1, &mlen1, sm, smlen, pk);
		QueryPerformanceCounter(&endCount);;//计时结束
		//QueryPerformanceFrequency(&fre);
		ttopen += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;
	}
	free(m);
	free(m1);
	free(sm);
	printf("keypair %8.4fms \n ", ttkeypair*1000/iter);
	printf("sign %8.4fms \n", ttsign*1000/ iter);
	printf("open %8.4fms \n", ttopen*1000/ iter);

	return 0;
}