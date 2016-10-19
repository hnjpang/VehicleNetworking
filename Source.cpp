#include <stdio.h>

#define RxFrame 3

void main(void){
	int ID[8] = {};
	int binID = 0;
	int c[8] = {};
	int BusData[8] = { 0, 0, 0, 0, 1, 0, 1, 0 };
	int Frame[RxFrame] = {};
	int fullData=0;
	for (int j = 0; j < RxFrame; j++){
		for (int i = 0; i<8; i++){
				if (BusData[i] == 0){
					ID[i] = 0;
				}
				else if (BusData[i] == 1){
					ID[i] = 1;
				}
			if (i == 0){
				binID = ID[0];
				c[i] = binID;
			}
			else{
				binID = (binID << 1) + ID[i];
				c[i] = binID;
			}
		}
		Frame[j] = binID;
	}
	for (int h = RxFrame - 1; h >= 0; h--){
		fullData = fullData + (Frame[h] << (8 * h));
	}
	for (int i = 0; i < 8; i++){
		printf("RxFrame=%d, fullData=%d and binID=%d, Frame=%d\n", binID, fullData, c[i], Frame[i]);
	}
}