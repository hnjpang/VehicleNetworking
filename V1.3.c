#include <can_driver_mac_types.h>
#include <can_phy.h>

//V1.1

static void hw_can_mac_driver(volatile CAN_PORT *can_port_id,
		       CAN_FRAME * volatile *TxFrameFromSensor,
		       CAN_FRAME * volatile *RxFrameForActuator,
		       int *rxPrioFilters, uint rxPrioFiltersLen)
{
  CAN_FRAME TxFrame, RxFrame;
  bool newFrameFromSensor;
  CAN_SYMBOL TxSymbol, RxSymbol;
  
  int p=0;
  bool BusIdle=true;
  bool SOF=false;
  bool SelfSend=false;
  bool Collision=false;
  bool NotSend=false;
  bool Received=true;
  
    
  while (1) {
/* 	//Read from bus
	if (BusIdle==false){
	can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);

// Scan for idle bus
	if (RxSymbol==RECESSIVE){
		//Start idle bus counter
		p++;
		if(p>=10){
			BusIdle=true;
			p=0;
		}
	}
	else if (RxSymbol==DOMINANT){  // Find start of file
		if (p>=11 || BusIdle==true){
			SOF=true;
			p=0;
		}
		else {
			p=0;
			BusIdle=false;
		}
	}
	 */
	
	
// sender code
	newFrameFromSensor = can_mac_rx_next_frame(TxFrameFromSensor, &TxFrame);
	if (newFrameFromSensor==true && BusIdle==true){
		
		can_phy_tx_symbol(can_port_id, DOMINANT);
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);			//Start of Frame
		BusIdle=false;
		
		
		for(int i=1;i<12;i++){								//Transmit ID
		if(TxFrame.ID==1 && i==11){
			TxSymbol=RECESSIVE;
		}
		else if(TxFrame.ID==2 && i==10){
			TxSymbol=RECESSIVE;
		}
		else if(TxFrame.ID==3 && (i==10 || i==11)){
			TxSymbol=RECESSIVE;
		}
		else{
			TxSymbol=DOMINANT;
		}
		can_phy_tx_symbol(can_port_id, TxSymbol);
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
					//Transmit ID

		
		
		if(TxSymbol!=RxSymbol){								//Scan for Collision
			Collision=true;
			BusIdle=false;
			break;
		}
		else if (i==11){
			Collision=false;
		}
		}													//Close for loop
		
		
		if (Collision==false){								//break after collision
		
		
		for(int i=0; i<3; i++){								//Three dominant
		can_phy_tx_symbol(can_port_id, DOMINANT);
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
		}
		
		
		for(int i=0; i<4; i++){								//Transmit DLC
		if(TxFrame.DLC==1 && i==3){
			TxSymbol=RECESSIVE;
		}
		else if(TxFrame.DLC==2 && i==2){
			TxSymbol=RECESSIVE;
		}
		else if(TxFrame.DLC==3 && (i==2 || i==3)){
			TxSymbol=RECESSIVE;
		}
		else{
			TxSymbol=DOMINANT;
		}
	can_phy_tx_symbol(can_port_id, TxSymbol);
	can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
	}														//DLC transmitted
	
	
	int Framesize=TxFrame.DLC*8;						//Transmit message
	int b[64]={};
	b[0]=TxFrame.Data;
	for(int i=0; i<Framesize;i++){						//
		b[i+1]=b[i]>>1;									// Shift 1 digit to right
		if(b[i]==0){									//
			TxSymbol=DOMINANT;							//				
		}												//
		else if((b[i] - b[i + 1]) != b[i+1]){			// Determine if last digit was dominant or recessive
			TxSymbol=RECESSIVE;							//
		}												//
		else{											//
			TxSymbol=DOMINANT;							//
		}												//
		can_phy_tx_symbol(can_port_id, TxSymbol);		// Write symbol on bus
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
	}
	
	/* for (int i=0; i<TxFrame.DLC*8;i++){
		TxSymbol=RECESSIVE;
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
		can_phy_tx_symbol(can_port_id, TxSymbol);		//Data Test transmitted
	} */
	
	
	/* for (int i=0; i<15;i++){							//CRC
		if ((i % 2)==0){
			TxSymbol=DOMINANT;
		}
		else{
			TxSymbol=RECESSIVE;
		}
	can_phy_tx_symbol(can_port_id, TxSymbol);
	can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
				//CRC transmitted
	} */
	
	
	
	
	
	
	int x;
	int Gx=65;
	for (int i = 0; i <= 7; i++){
		x = pow(2.0, i);
		if (Gx <2*x){
			break;
		}
	}
	int F = TxFrame.Data * x;
	int R = F % Gx;

	int CRC[15];
	for (int i = 14; i >= 0; i--){
		CRC[i] = R % 2;
		R /= 2;
	}
	for (int i = 0; i < 15; i++){
		if (CRC[i]==0){
			TxSymbol=DOMINANT;
		}
		else{
			TxSymbol=RECESSIVE;
		}
		can_phy_tx_symbol(can_port_id, TxSymbol);
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
	}
	
	
	
	
	
	
	for (int i=0; i<1;i++){
		can_phy_tx_symbol(can_port_id, RECESSIVE);
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
				//1 recessive
	}
	
	
	can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
	if(RxSymbol==DOMINANT){
		NotSend=false;
	}
	else if (RxSymbol==RECESSIVE){
		NotSend=false;									//Change this
	}
	
	for (int i=0; i<1;i++){
		can_phy_tx_symbol(can_port_id, DOMINANT);
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
				//1 Dominant
	}
	
		for(int i=0; i<10; i++){						//Last 11 recessives
	can_phy_tx_symbol(can_port_id, RECESSIVE);
	can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
	}
	BusIdle=true;
	}
	}
	
// Receiver code
//	while (newFrameFromSensor==false && SelfSend==false){
else if (p==1000){
		
		
	int ID[11]={};										//Receive ID
	for (int i=0;i<12;i++){
	can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
	if (i>0){
		if (RxSymbol==DOMINANT){
			ID[i-1]=0;
		}
		else if(RxSymbol==RECESSIVE){
			ID[i-1]=1;
		}
	}
	if (i==1){
		RxFrame.ID=ID[0];
	}
	else{
		RxFrame.ID=(RxFrame.ID<<1)+ID[i-1];
	}
	}													//ID received
	
	
	int DLC[4]={};										//Receive DLC
	int decDLC=0;
	for (int i=0;i<7;i++){
	can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
	if(i>2){
		if(RxSymbol==DOMINANT){
			DLC[i-3]=0;
		}
		else if(RxSymbol==RECESSIVE){
			DLC[i-3]=1;
		}
	}
	if (i==3){
		RxFrame.DLC=DLC[0];
	}
	else{
		RxFrame.DLC=(RxFrame.DLC<<1)+DLC[i-3];
	}
	}													//DLC received
	
	
	int Data[8]={};										//Receive Data
	int decData=0;
	int Frame[1]={};
	RxFrame.Data=0;
	for(int j=0;j<RxFrame.DLC;j++){
		for (int i=0;i<8;i++){
			can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
			if(RxSymbol==DOMINANT){
				Data[i]=0;
			}
			else if(RxSymbol==RECESSIVE){
				Data[i]=1;
			}
			if (i==0){
				decData=Data[0];
			}
			else{
				decData=(decData<<1)+Data[i];
			}
		}
		Frame[j]=decData;
	}
	for (int h=RxFrame.DLC-1; h>=0;h--){
		RxFrame.Data=RxFrame.Data+(Frame[h]<<(8*h));
	}													//Data Received
	
	
	for (int i=0;i<15;i++){
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
		if(((i%2)==0 && RxSymbol==RECESSIVE) || ((i%2)==1 && RxSymbol==DOMINANT)){
			Received=false;
			break;
		}
		else if(i==14){
			Received=true;
			TxSymbol=DOMINANT;
			can_phy_tx_symbol(can_port_id, TxSymbol);
		}
	}
	can_mac_tx_next_frame(&RxFrame, RxFrameForActuator);
	}
	
// Set some counters and constants
	
  }
}  