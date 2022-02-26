#include <iostream>  
#include <openssl/sha.h>  
using namespace std;
void prf(const unsigned char* md)
{
	int i = 0;
	char buf[65] = { 0 };
	char tmp[3] = { 0 };
	for (i = 0; i < 32; i++)
	{
		sprintf(tmp, "%02X", md[i]);
		strcat(buf, tmp);
	}

	cout << buf << endl;
}
int main()
{
	unsigned char md[33] = { 0 };
	clock_t begin, end;
	double tt = 0;
	int iter = 1000000;
	for (int i = 0; i < iter; i++)
	{
		begin = clock();
		SHA256((const unsigned char*)"hello", strlen("hello"), md);
		end = clock();
		tt = tt + (double)(end - begin);
	}
	printf("SHA256 %8.10fms ", tt / iter);
	// prf(md);
	return 0;
}
