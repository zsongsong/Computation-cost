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
    LARGE_INTEGER fre = { 0 };//储存本机CPU时钟频率
    LARGE_INTEGER startCount = { 0 };
    LARGE_INTEGER endCount = { 0 };
    QueryPerformanceFrequency(&fre);//获取本机cpu频率
	miracl* mip = mirsys(1, 0); //初始化miracl系统
    mip->IOBASE = 16;
    PFC pfc(80);//big n = mirvar(8); //初始化n,必须有
    Big a;//定义一个Big类型变量，默认初始化为0
    big aa;
    G1 P, Q;//乘法
    G1 P1, Q1;//配对
    GT ZT;
    aa = mirvar(0);
    for (int i = 0; i < k; i++) {


        pfc.random(a);
        pfc.random(P);
        pfc.random(Q);
        //pfc.precomp_for_mult(P);
        QueryPerformanceCounter(&startCount);//计时开始
        Q = pfc.mult(P, a); //Q=aP
        QueryPerformanceCounter(&endCount);//计时结束
        QueryPerformanceFrequency(&fre);//获取本机cpu频率
        dTimeTake1 += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;

        pfc.random(P1);
        pfc.random(Q1);
        //pfc.precomp_for_pairing(P1);
        //pfc.precomp_for_pairing(Q1);
        QueryPerformanceCounter(&startCount);//计时开始
        ZT = pfc.pairing(P1, Q1); //Z=e(P,Q);
        QueryPerformanceCounter(&endCount);//计时结束
        QueryPerformanceFrequency(&fre);//获取本机cpu频率
        dTimeTake2 += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;

        QueryPerformanceCounter(&startCount);//计时开始
        pfc.hash_and_map(P1, (char*)"Robert");
        QueryPerformanceCounter(&endCount);//计时结束
        QueryPerformanceFrequency(&fre);//获取本机cpu频率
        dTimeTake3 += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;
    }

    printf("mult %f ms！\n", dTimeTake1 * 1000 / k);
    printf("pairing %f ms！\n", dTimeTake2 * 1000/k);
    printf("hash_and_map %f ms！\n", dTimeTake3 * 1000/k);

	return 0;
}