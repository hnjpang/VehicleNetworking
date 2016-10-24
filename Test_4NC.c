#include <can_driver_mac_types.h>
#include <can_phy.h>

//V1.1

int Recessive=0;
int Dominant=0;

CAN_SYMBOL sendSymbol(bool Symbol, volatile CAN_PORT *can_port_id, int IDsend){
	CAN_SYMBOL TxSymbol, RxSymbol,BitStuff;
	if (Symbol==DOMINANT){
		can_phy_tx_symbol(can_port_id, DOMINANT);
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
	}
	else{
		can_phy_tx_symbol(can_port_id, RECESSIVE);
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
	}
	if (RxSymbol==RECESSIVE){
		Recessive++;
		Dominant=0;
	}
	else if (RxSymbol==DOMINANT){
		Dominant++;
		Recessive=0;
	}
	if(Recessive>=5){
		can_phy_tx_symbol(can_port_id, DOMINANT);
		can_phy_rx_symbol_blocking(can_port_id,&BitStuff);
		Dominant++;
		Recessive=0;
	}
	if(Dominant>=5){
		can_phy_tx_symbol(can_port_id, RECESSIVE);
		can_phy_rx_symbol_blocking(can_port_id,&BitStuff);
		Recessive++;
		Dominant=0;
	}
	return RxSymbol;
}

CAN_SYMBOL ReceiveSymbol(bool Symbol, volatile CAN_PORT *can_port_id){
	
}

static void hw_can_mac_driver(volatile CAN_PORT *can_port_id,
		       CAN_FRAME * volatile *TxFrameFromSensor,
		       CAN_FRAME * volatile *RxFrameForActuator,
		       int *rxPrioFilters, uint rxPrioFiltersLen)
{
  CAN_FRAME TxFrame, RxFrame;
  bool newFrameFromSensor;
  CAN_SYMBOL TxSymbol, RxSymbol;
  
  int p=0;
  int R=0;
  int D=0;
  int IDsend=0;
  bool BusIdle=true;
  bool SOF=false;
  bool SelfSend=true;
  bool Collision=false;
  bool NotSend=false;
  bool Received=true;
  bool AllowSend=false;
  bool NoSender=false;
  bool CRCerror=false;
  
    
  while (1) {
	//Read from bus	
	Dominant=0;
	Recessive=0;
// sender code
	newFrameFromSensor = can_mac_rx_next_frame(TxFrameFromSensor, &TxFrame);
	if ((newFrameFromSensor==true && BusIdle==true) || (newFrameFromSensor==true && NoSender==true) || (BusIdle==true && Collision==true)){
		
		sendSymbol(DOMINANT, can_port_id, IDsend);			//Start of Frame
		BusIdle=false;
		Collision=false;
		
		
		for(int i=1;i<12;i++){								//Transmit ID
		IDsend=1;
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
		RxSymbol=sendSymbol(TxSymbol, can_port_id, IDsend);
					//Transmit ID

		
		
		if(TxSymbol!=RxSymbol){								//Scan for Collision
			Collision=true;
			SelfSend=false;
			BusIdle=false;
			IDsend=false;
			p=0;
			Dominant=0;
			Recessive=0;
			break;
		}
		else if (i==11){
			Collision=false;
			IDsend=0;
		}
		}													//Close for loop
		
		
		if (Collision==false){								//break after collision
		
		
		for(int i=0; i<3; i++){								//Three dominant
		sendSymbol(DOMINANT, can_port_id, IDsend);
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
	sendSymbol(TxSymbol, can_port_id,IDsend);
	}														//DLC transmitted
	
	
	int Framesize=TxFrame.DLC*8;						//Transmit message
	int b[64]={};
	int c[64]={};
	b[0]=TxFrame.Data;
	for(int i=0; i<Framesize;i++){						//
		b[i+1]=b[i]>>1;									// Shift 1 digit to right
		if(b[i]==0){									//						//				
			c[i]=0;
		}												//
		else if((b[i] - b[i + 1]) != b[i+1]){			// Determine if last digit was dominant or recessive						//
			c[i]=1;
		}												//
		else{											//							//
			c[i]=0;
		}												//
	}
	for (int g=Framesize-1; g>=0;g--){
		if(c[g]==0){
			TxSymbol=DOMINANT;
		}
		else{
			TxSymbol=RECESSIVE;
		}
	sendSymbol(TxSymbol, can_port_id,IDsend);     //Write symbol on bus
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
		sendSymbol(TxSymbol, can_port_id,IDsend);
	}
	
	
	
	
	
	
	for (int i=0; i<1;i++){
		sendSymbol(RECESSIVE, can_port_id,IDsend);
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
		sendSymbol(DOMINANT, can_port_id, IDsend);
				//1 Dominant
	}
	
		for(int i=0; i<10; i++){						//Last 10 recessives
	sendSymbol(RECESSIVE, can_port_id,IDsend);
	}
	p=0;
	SelfSend=false;
	}
	}
	else if(newFrameFromSensor==false){
		SelfSend=false;
	}
	while (SelfSend==false){
		if(R>=10){
			R=0;
			BusIdle=true;
			SelfSend=true;
			NoSender=true;
			break;
		}
	can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
	BusIdle=false;
	if (RxSymbol==RECESSIVE){
		R++;
		D=0;
	}
	else if (RxSymbol==DOMINANT && (R==5 || R==10)){
		R=R;
	}
	else if (RxSymbol==DOMINANT){
		D++;
		R=0;
		NoSender=false;
		BusIdle=false;
	}
	if (NoSender==true && BusIdle==false){
		SelfSend=true;
		break;
	}
	
  }
	
// Receiver code
//	while (newFrameFromSensor==false && SelfSend==false){
	if (rxPrioFilters>=0){
		
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
	}
	for (int g=12;g>=0;g--){
	if (g==12){
		RxFrame.ID=ID[g];
	}
	else{
		RxFrame.ID=(RxFrame.ID<<1)+ID[g];
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
	
	
	int Data[64]={};//Receive Data
	int c[64]={};
	int decData=0;
	int Frame[8]={};
	for (int i=RxFrame.DLC-1;i>=0;i--){
			can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
			if(RxSymbol==DOMINANT){
				Data[i]=0;
			}
			else if(RxSymbol==RECESSIVE){
				Data[i]=1;
			}
			if (i==RxFrame.DLC-1){
				RxFrame.Data=Data[0];
			}
			else{
				RxFrame.Data=(RxFrame.Data<<1)+Data[i];
			}
	}													//Data Received
	
	
	int x;
	int Gx=65;
	for (int i = 0; i <= 7; i++){
		x = pow(2.0, i);
		if (Gx <2*x){
			break;
		}
	}
	int F = RxFrame.Data * x;
	int R = F % Gx;

	int CRC[15]={};
	int CRCread[15]={};
	for (int i = 14; i >= 0; i--){
		CRC[i] = R % 2;
		R /= 2;
	}
	for (int i = 0; i < 15; i++){
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
		if (CRC[i]==0 && RxSymbol==RECESSIVE){
			CRCerror=true;
		}
		else if(CRC[i]==1 && TxSymbol==DOMINANT){
			CRCerror=true;
		}		
		else {
			CRCerror=false;
			if(RxSymbol==RECESSIVE){
				CRCread[i]=1;
			}
			else{
				CRCread[i]=0;
			}
		}
		if(i==14 && CRCerror==false){
			can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
			can_phy_tx_symbol(can_port_id, DOMINANT);
			can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
		}
	}
	
	for(int g=0; g<15; g++){
		if(g==0){
			RxFrame.CRC==CRCread[g];
		}
		else{
			RxFrame.CRC=RxFrame.CRC<<1+CRCread[g];
		}
	}
	
	
	for (int i=0;i<12;i++){
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
	}
	
	
	// for (int i=0;i<15;i++){
		// can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
		// if(((i%2)==0 && RxSymbol==RECESSIVE) || ((i%2)==1 && RxSymbol==DOMINANT)){
			// Received=false;
			// break;
		// }
		// else if(i==14){
			// Received=true;
			// TxSymbol=DOMINANT;
			// can_phy_tx_symbol(can_port_id, TxSymbol);
		// }
	// }
	can_mac_tx_next_frame(RxFrameForActuator,&RxFrame);
	}
	
// Set some counters and constants
	
  }
}  