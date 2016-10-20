#include <stdio.h>
#include <math.h>
#include <stdint.h>

void main(){
	uint64_t frame = 83;
	uint64_t generator = 65;
	int x;
	for (int i = 0; i <= 7; i++){
		x = pow(2.0, i);
		if (generator <2*x){
			break;
		}
	}
	uint64_t F = frame * x;
	uint64_t R = F % generator;

	printf("%d \n", R);

	uint64_t CRC[15];
	for (int i = 14; i >= 0; i--){
		CRC[i] = R % 2;
		R /= 2;
	}
	for (int i = 0; i < 15; i++){
		printf("%d", CRC[i]);
	}
}