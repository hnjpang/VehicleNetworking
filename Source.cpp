#include <stdio.h>

bool NextSymbol(bool BusData, int BusDataOld){
	bool Status;
	if (BusData != BusDataOld){
		Status = true;
	}
	else{
		Status = false;
	}
	return Status;
}

void main(void){
	bool BusData=true;
	bool BusDataOld=true;
	bool Check;
	int j=0, i=0;
	for (i = 0; i<5; i++){
		if (i < 3){
			BusData = true;
		}
		else if (i >= 3){
			BusData = false;
		}
		Check = NextSymbol(BusData, BusDataOld);
		if (Check == true){
			j = 1;
		}
		else if (Check == false){
			j = 0;
		}
		printf("i=%d		Check=%d\n", i, j);
	}
	BusDataOld = BusData;
}