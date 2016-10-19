#include <stdio.h>
#include <math.h>

#define size 8

void main(void){
	int Binary[size] = {};
	int b[size] = {};
	scanf_s("%d", &b[0]);
	printf("Scanned number: %d\n", b[0]);
	for (int i = 0; i < size;i++){
		b[i+1] = b[i] >> 1;
		if (b[i] == 0){
			Binary[i] = 0;
		}
		else if((b[i] - b[i + 1]) != b[i+1]){
			Binary[i] = 1;
		}
		else{
			Binary[i] = 0;
		}		
	}
	printf("Binary number: ");
	for (int i = 0; i < size; i++){
		printf("%d", Binary[(size-1)-i]);
	}
}