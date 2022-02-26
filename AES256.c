// main.c
#include <openssl/aes.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
unsigned char* str2hex(char* str) {
    unsigned char* ret = NULL;
    int str_len = strlen(str);
    int i = 0;
    assert((str_len % 2) == 0);
    ret = (char*)malloc(str_len / 2);
    for (i = 0; i < str_len; i = i + 2) {
        sscanf(str + i, "%2hhx", &ret[i / 2]);
    }
    return ret;
}

char* padding_buf(char* buf, int size, int* final_size) {
    char* ret = NULL;
    int pidding_size = AES_BLOCK_SIZE - (size % AES_BLOCK_SIZE);
    int i;

    *final_size = size + pidding_size;
    ret = (char*)malloc(size + pidding_size);
    memcpy(ret, buf, size);
    if (pidding_size != 0) {
        for (i = size; i < (size + pidding_size); i++) {
            ret[i] = 0;
        }
    }

    return ret;
}

void printf_buff(char* buff, int size) {
    int i = 0;
    for (i = 0; i < size; i++) {
        printf("%02X ", (unsigned char)buff[i]);
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }

    printf("\n\n\n\n");
}

void encrpyt_buf(char* raw_buf, char** encrpy_buf, int len) {
    AES_KEY aes;
    unsigned char* key = str2hex("8cc72b05705d5c46f412af8cbed55aad8cc72b05705d5c46f412af8cbed55aad");
    //unsigned char* key = str2hex("8cc72b0234567835");
    unsigned char* iv = str2hex("667b02a85c61c786def4521b060265e8");
    AES_set_encrypt_key(key, 256, &aes);
    AES_cbc_encrypt(raw_buf, *encrpy_buf, len, &aes, iv, AES_ENCRYPT);

    free(key);
    free(iv);
}

void decrpyt_buf(char* raw_buf, char** encrpy_buf, int len) {
    AES_KEY aes;
    unsigned char* key = str2hex("8cc72b05705d5c46f412af8cbed55aad8cc72b05705d5c46f412af8cbed55aad");
    //unsigned char* key = str2hex("8cc72b0234567835");
    unsigned char* iv = str2hex("667b02a85c61c786def4521b060265e8");
    AES_set_decrypt_key(key, 256, &aes);
    AES_cbc_encrypt(raw_buf, *encrpy_buf, len, &aes, iv, AES_DECRYPT);

    free(key);
    free(iv);
}

int main(int argn, char* argv[]) {
    char* raw_buf = NULL;
    char* after_padding_buf = NULL;
    int padding_size = 0;
    char* encrypt_buf = NULL;
    char* decrypt_buf = NULL;

    LARGE_INTEGER fre = { 0 };//储存本机CPU时钟频率
    LARGE_INTEGER startCount = { 0 };
    LARGE_INTEGER endCount = { 0 };
    QueryPerformanceFrequency(&fre);//获取本机cpu频率
    clock_t begin, end;
    double ttkeypair = 0, ttenc = 0, ttdec = 0;
    int iter = 10000;
    int len = 1281;
    // 1
    raw_buf = (char*)malloc(len);
    memcpy(raw_buf, "0l234567890123456789012345678901234567890 \\
        l2345678901234567890123456789012345678901234566789012345678901\\
        ", len);


        //       printf("------------------raw_buf %d\n",strlen(raw_buf));
         //      printf_buff(raw_buf, len);

               // 2
        after_padding_buf = padding_buf(raw_buf, len, &padding_size);
    //        printf("------------------after_padding_buf %d\n",strlen(after_padding_buf));
     //       printf_buff(after_padding_buf, padding_size);

    for (int i = 0; i < iter; i++)
    {
        // 3
        encrypt_buf = (char*)malloc(padding_size);
        QueryPerformanceCounter(&startCount);//计时开始
        encrpyt_buf(after_padding_buf, &encrypt_buf, padding_size);
        QueryPerformanceCounter(&endCount);;//计时结束
        //QueryPerformanceFrequency(&fre);
        ttenc += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;
        //   printf("------------------encrypt_buf %d\n",strlen(encrypt_buf));
        //   printf_buff(encrypt_buf, padding_size);

           // 4
        decrypt_buf = (char*)malloc(padding_size);
        QueryPerformanceCounter(&startCount);//计时开始
        decrpyt_buf(encrypt_buf, &decrypt_buf, padding_size);
        QueryPerformanceCounter(&endCount);;//计时结束
        //QueryPerformanceFrequency(&fre);
        ttdec += ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)fre.QuadPart;

        //   printf("------------------decrypt_buf %d\n",strlen(decrypt_buf));
        //   printf_buff(decrypt_buf, padding_size);
    }
    free(raw_buf);
    free(after_padding_buf);
    free(encrypt_buf);
    free(decrypt_buf);
    printf("enc %8.4fms \n ", ttenc*1000 / iter);
    printf("dec %8.4fms \n ", ttdec * 1000 / iter);

    return 0;
}
