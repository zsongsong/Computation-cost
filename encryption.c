#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "rng.h"
#include "api.h"
#include "cmov.h"
#include "kem.h"
#include "owcpa.h"
#include "params.h"
#include "rng.h"
#include "sample.h"
#include "fips202.h"
#include <time.h>
#include <windows.h>
void
keypair(poly* f,
	poly* invf_mod3,
	poly* h,
	const unsigned char seed[NTRU_SAMPLE_FG_BYTES])
{
	int i;
	poly x1, x2, x3, x4, x5;
	poly* g = &x2;
	poly* gf = &x3, * invgf = &x4, * tmp = &x5;
	sample_fg(f, g, seed);
	poly_S3_inv(invf_mod3, f);
	/* Lift coeffs of f and g from Z_p to Z_q */
	poly_Z3_to_Zq(f);
	poly_Z3_to_Zq(g);
	for (i = 0; i < NTRU_N; i++)
		g->coeffs[i] = 3 * g->coeffs[i];
	poly_Rq_mul(gf, g, f);
	poly_Rq_inv(invgf, gf);
	poly_Rq_mul(tmp, invgf, g);
	poly_Rq_mul(h, tmp, g);
	//poly_trinary_Zq_to_Z3(&f);
}

void enc(poly* c,
	const poly* r,
	const poly* m,
	const poly* h)
{
	int i;
	poly x1;
	poly* liftm = &x1;
	poly* ct = c;
	// poly_lift(liftm, &m);
	poly_Z3_to_Zq(m);
	poly_Rq_mul(ct, r, h);
	for (i = 0; i < NTRU_N; i++) {
		ct->coeffs[i] = ct->coeffs[i] + m->coeffs[i];
		ct->coeffs[i] = (ct->coeffs[i]) & (NTRU_Q - 1);
	}
}

int dec(poly* mm,
	poly* c,
	const  poly* f,
	const poly* invf_mod3)
{
	int i;
	poly x1, x2;
	poly* cf = &x1, * mf = &x2;
	poly_Z3_to_Zq(f);
	poly_Rq_mul(cf, c, f);
	poly_Rq_to_S3(mf, cf);
	poly_S3_mul(mm, mf, invf_mod3);
	return 0;
}
void
prt(poly* a)
{
	for (int i = 0; i < NTRU_N; i++)
		printf("%d ", a->coeffs[i]);
	printf("\n");
}
int
main()
{
	poly r, m, c, mm, f, invf_mod3, h, cf;
	int i;
	unsigned char       entropy_input[48];
	unsigned char rm_seed[NTRU_SAMPLE_RM_BYTES];

	for (i = 0; i < 48; i++)
		entropy_input[i] = i;
	randombytes_init(entropy_input, NULL, 256);

	LARGE_INTEGER fre = { 0 };//储存本机CPU时钟频率
	LARGE_INTEGER startCount = { 0 };
	LARGE_INTEGER endCount = { 0 };
	QueryPerformanceFrequency(&fre);//获取本机cpu频率
	clock_t begin, end;
	double ttkeypair = 0, ttenc = 0, ttdec = 0, ttmul=0;
	int iter = 100;

	for (i = 0; i < iter; i++)
	{
		// Generate the public/private keypair
		
		randombytes(rm_seed, NTRU_SAMPLE_RM_BYTES);
		QueryPerformanceCounter(&startCount);//计时开始
		keypair(&f, &invf_mod3, &h, rm_seed);
		QueryPerformanceCounter(&endCount);//计时结束
		ttkeypair += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;


		randombytes(rm_seed, NTRU_SAMPLE_RM_BYTES);
		sample_rm(&r, &m, rm_seed);
		//prt(&m);

		QueryPerformanceCounter(&startCount);//计时开始
		poly_Rq_mul(&cf, &c, &h);
		QueryPerformanceCounter(&endCount);//计时结束
		ttmul += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;

		//enc
		QueryPerformanceCounter(&startCount);//计时开始
		enc(&c, &r, &m, &h);
		QueryPerformanceCounter(&endCount);;//计时结束
		ttenc += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;
		//  prt(&c);
		
		 //dec
		QueryPerformanceCounter(&startCount);//计时开始
		dec(&mm, &c, &f, &invf_mod3);
		QueryPerformanceCounter(&endCount);//计时结束
		ttdec += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;
		//  prt(&mm);
	}
	printf("keypair %8.4fms \n ", ttkeypair*1000 / iter);
	printf("enc %8.4fms \n", ttenc * 1000 / iter);
	printf("dec %8.4fms\n ", ttdec * 1000 / iter);
	printf("mul %8.4fms \n", ttmul * 1000 / iter);
	return 0;
}
