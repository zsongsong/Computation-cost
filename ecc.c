#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "miracl.h"
#pragma comment (lib, "miracl.lib")
#define SM2_PAD_ZERO TRUE
#define SM2_DEBUG   0
//#define MR_OS_THREADS
#include <windows.h>
struct FPECC {
	char *p;
	char *a;
	char *b;
	char *n;
	char *x;
	char *y;
};

struct FPECC Ecc160r1 = {
"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFF",//p
"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFC",//a
"1C97BEFC54BD7A8B65ACF89F81D4D4ADC565FA45",//b
"0100000000000000000001F4C8F927AED3CA752257",//n
"4A96B5688EF573284664698968C38BB913CBFC82",//Gx
"23A628553168947D59DCC912042351377AC5FB32",//Gy
};
#define SEED_CONST 0x1BD8C95A
void main()
{
	int k = 1000;
	double dTimeTake1=0, dTimeTake2 = 0, dTimeTake3 = 0;
	LARGE_INTEGER fre = { 0 };//���汾��CPUʱ��Ƶ��
	LARGE_INTEGER startCount = { 0 };
	LARGE_INTEGER endCount = { 0 };
	QueryPerformanceFrequency(&fre);//��ȡ����cpuƵ��

	struct FPECC *cfig = &Ecc160r1;// 160
	epoint *g,*h;
	big a, b, p, n, x,y, key1;
	miracl *mip = mirsys(20, 0);

	mip->IOBASE = 16;

	p = mirvar(0);
	a = mirvar(0);
	b = mirvar(0);
	n = mirvar(0);
	x = mirvar(0);
	y = mirvar(0);
	key1 = mirvar(0);
	//���������ݸ�ֵ����Ӧ����
	cinstr(p, cfig->p);
	cinstr(a, cfig->a);
	cinstr(b, cfig->b);
	cinstr(n, cfig->n);
	cinstr(x, cfig->x);
	cinstr(y, cfig->y);
	//��ʼ����Բ����
	ecurve_init(a, b, p, MR_PROJECTIVE);
	g = epoint_init();
	h = epoint_init();
	epoint_set(x, y, 0, g);
	epoint_set(x, y, 0, h);
	//��ʼ���������
	irand(SEED_CONST);
	for (int i = 0; i < k; i++) {
		bigbits(10, key1);
		epoint_set(x, y, 0, g);
		epoint_set(x, y, 0, h);
		QueryPerformanceCounter(&startCount);//��ʱ��ʼ
		ecurve_mult(key1, g, h);//С��
		QueryPerformanceCounter(&endCount);;//��ʱ����
		QueryPerformanceFrequency(&fre);
		dTimeTake1 += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;

		bigrand(n, key1);
		epoint_set(x, y, 0, g);
		epoint_set(x, y, 0, h);
		QueryPerformanceCounter(&startCount);//��ʱ��ʼ
		ecurve_mult(key1, g, h);//��
		QueryPerformanceCounter(&endCount);;//��ʱ����
		QueryPerformanceFrequency(&fre);
		dTimeTake2 += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;

		epoint_set(x, y, 0, g);
		epoint_set(x, y, 0, h);
		QueryPerformanceCounter(&startCount);//��ʱ��ʼ
		ecurve_add(g, h);//�ӷ�
		QueryPerformanceCounter(&endCount);;//��ʱ����
		QueryPerformanceFrequency(&fre);
		dTimeTake3 += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;

	}

	printf("small mult %f ms��\n", dTimeTake1*1000/k);
	printf("mult %f ms��\n", dTimeTake2 * 1000 / k);
	printf("add %f ms��\n", dTimeTake3 * 1000/k);

	mirkill(key1);
	mirkill(p);
	mirkill(a);
	mirkill(b);
	mirkill(n);
	mirkill(x);
	epoint_free(g);
	mirexit();
}