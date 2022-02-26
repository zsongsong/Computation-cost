#include <iostream>
//security define
#define AES_SECURITY 80
#define MR_PAIRING_SSP
//#define MR_PAIRING_SS2
#include "pairing_1.h"
#include <windows.h>
#pragma comment (lib, "miracl.lib")
using namespace std;
extern "C"
{
#include "miracl.h"
#include "mirdef.h"
#include <stdio.h>
}

int main_pair() {
    int k = 100;
    double dTimeTake1=0, dTimeTake2=0, dTimeTake3=0;
    LARGE_INTEGER fre = { 0 };//���汾��CPUʱ��Ƶ��
    LARGE_INTEGER startCount = { 0 };
    LARGE_INTEGER endCount = { 0 };
    QueryPerformanceFrequency(&fre);//��ȡ����cpuƵ��
	miracl* mip = mirsys(1, 0); //��ʼ��miraclϵͳ
    mip->IOBASE = 16;
    PFC pfc(80);//big n = mirvar(8); //��ʼ��n,������
    Big a;//����һ��Big���ͱ�����Ĭ�ϳ�ʼ��Ϊ0
    big aa;
    G1 P, Q;//�˷�
    G1 P1, Q1;//���
    GT ZT;
    aa = mirvar(0);
    for (int i = 0; i < k; i++) {


        pfc.random(a);
        pfc.random(P);
        pfc.random(Q);
        //pfc.precomp_for_mult(P);
        QueryPerformanceCounter(&startCount);//��ʱ��ʼ
        Q = pfc.mult(P, a); //Q=aP
        QueryPerformanceCounter(&endCount);//��ʱ����
        QueryPerformanceFrequency(&fre);//��ȡ����cpuƵ��
        dTimeTake1 += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;

        pfc.random(P1);
        pfc.random(Q1);
        //pfc.precomp_for_pairing(P1);
        //pfc.precomp_for_pairing(Q1);
        QueryPerformanceCounter(&startCount);//��ʱ��ʼ
        ZT = pfc.pairing(P1, Q1); //Z=e(P,Q);
        QueryPerformanceCounter(&endCount);//��ʱ����
        QueryPerformanceFrequency(&fre);//��ȡ����cpuƵ��
        dTimeTake2 += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;

        QueryPerformanceCounter(&startCount);//��ʱ��ʼ
        pfc.hash_and_map(P1, (char*)"Robert");
        QueryPerformanceCounter(&endCount);//��ʱ����
        QueryPerformanceFrequency(&fre);//��ȡ����cpuƵ��
        dTimeTake3 += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;
    }

    printf("mult %f ms��\n", dTimeTake1 * 1000 / k);
    printf("pairing %f ms��\n", dTimeTake2 * 1000/k);
    printf("hash_and_map %f ms��\n", dTimeTake3 * 1000/k);

	return 0;
}