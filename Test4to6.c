#include <can_driver_mac_types.h>
#include <can_phy.h>

//V1.1

Dominant=0;
Recessive=0;
DominantR=0;
RecessiveR=0;

CAN_SYMBOL sendSymbol(CAN_SYMBOL Symbol, volatile CAN_PORT *can_port_id){
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
	/* if(Recessive>=5){
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
	} */
	return RxSymbol;
}











CAN_SYMBOL readSymbol(volatile CAN_PORT *can_port_id, bool IDsend){
		CAN_SYMBOL RxSymbol, BitStuffR;
		can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
		return RxSymbol;
		/* if (IDsend==false){
		if (RxSymbol==DOMINANT){
			DominantR++;
			RecessiveR=0;
			if(DominantR>=5){
				can_phy_rx_symbol_blocking(can_port_id,&BitStuffR);
				return RxSymbol;
				DominantR=0;
			}
			else{
				return RxSymbol;
			}
		}
		else if(RxSymbol==RECESSIVE){
			RecessiveR++;
			DominantR=0;
			if(RecessiveR>=5){
				can_phy_rx_symbol_blocking(can_port_id,&BitStuffR);
				return RxSymbol;
				RecessiveR=0;
			}
			else{
				return RxSymbol;
			}
		}
		}
		else{
			if (RxSymbol==DOMINANT){
			DominantR++;
			RecessiveR=0;
			if(DominantR>=3){
				can_phy_rx_symbol_blocking(can_port_id,&BitStuffR);
				return RxSymbol;
				DominantR=0;
			}
			else{
				return RxSymbol;
			}
		}
		else if(RxSymbol==RECESSIVE){
			RecessiveR++;
			DominantR=0;
			if(RecessiveR>=3){
				can_phy_rx_symbol_blocking(can_port_id,&BitStuffR);
				return RxSymbol;
				RecessiveR=0;
			}
			else{
				return RxSymbol;
			}
		}
		} */
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
  int RR=0;
  int DD=0;
  bool BusIdle=true;
  bool SOF=false;
  bool SelfSend=true;
  bool Collision=false;
  bool NotSend=false;
  bool Received=true;
  bool AllowSend=false;
  bool CRCerror=false;
  bool NoSender=true;
  bool Receiving=true;
  bool IDsend=false;
  bool NewMessage=true;
  
    
  while (1) {
	//Read from bus	
	
// sender code

	if(*rxPrioFilters==-1){
	newFrameFromSensor = can_mac_rx_next_frame(TxFrameFromSensor, &TxFrame);
	if ((newFrameFromSensor==true && BusIdle==true) || (BusIdle==true && Collision==true) || (newFrameFromSensor==true && NoSender==true) || NotSend==true){
		
		sendSymbol(DOMINANT, can_port_id);
			//Start of Frame
		BusIdle=false;
		NoSender=false;
		Collision=false;
		Dominant=1;
		Recessive=0;
		
		
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
		RxSymbol=sendSymbol(TxSymbol, can_port_id);
					//Transmit ID

		
		
		if(TxSymbol!=RxSymbol){								//Scan for Collision
			Collision=true;
			SelfSend=false;
			BusIdle=false;
			p=0;
			R=0;
			break;
		}
		else if (i==11){
			Collision=false;
		}
		}													//Close for loop
		
		
		if (Collision==false){								//break after collision
		
		
		for(int i=0; i<3; i++){								//Three dominant
		sendSymbol(DOMINANT, can_port_id);
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
	sendSymbol(TxSymbol, can_port_id);
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
	sendSymbol(TxSymbol, can_port_id);		// Write symbol on bus
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
		sendSymbol(TxSymbol, can_port_id);
	}
	
	
	
	
	
	
	for (int i=0; i<1;i++){
		sendSymbol(RECESSIVE, can_port_id);
				//1 recessive
	}
	
	sendSymbol(RECESSIVE, can_port_id);
	if(RxSymbol==DOMINANT){
		NotSend=false;
	}
	else if (RxSymbol==RECESSIVE){
		NotSend=true;									//Change this
	}
	
	for (int i=0; i<1;i++){
		sendSymbol(DOMINANT, can_port_id);
				//1 Dominant
	}
	
		for(int i=0; i<10; i++){						//Last 11 recessives
	sendSymbol(RECESSIVE, can_port_id);
	}
	p=0;
	R=0;
	Dominant=0;
	Recessive=0;
	SelfSend=false;
	}
	}
	
	
	
	
	else if(newFrameFromSensor==false){
		SelfSend=false;
	}
	while (SelfSend==false){
		if(R>=10){
			R=0;
			D=1;
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
	else if (RxSymbol==DOMINANT && (R==5 || R==10 || D==1)){
		R=R;
		D=0;
	}
	else{
		D=0;
		R=0;
		NoSender=false;
		BusIdle=false;
	}
	if (NoSender==true && BusIdle==false){
		SelfSend=true;
		break;
	}
	
  }
	}
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  //Receiver
  
  	if (*rxPrioFilters!=-1){
		if(NewMessage==true){										//After EOF
			Receiving==false; NewMessage=false;
			while(1){												//Scan for SOF
				RxSymbol=readSymbol(can_port_id, false);
				if(RxSymbol==DOMINANT){
					RxFrame.ID=0;
					RxFrame.DLC=0;
					RxFrame.Data=0;
					RxFrame.CRC=0;
					break;											//SOF found, start read
				}
			}
		
	int ID[11]={};										//Receive ID
	int decID=10;
	for (int i=0;i<11;i++){
	RxSymbol=readSymbol(can_port_id, true);
	if (i>=0){
		if (RxSymbol==DOMINANT){
			ID[i]=0;
		}
		else if(RxSymbol==RECESSIVE){
			ID[i]=1;
		}
		if(i==0){
			decID=0;
		}
		else{
			decID=(decID<<1)+ID[i];	
		}
	}
	}
	RxFrame.ID=decID;										//ID received
	if(RxFrame.ID==*rxPrioFilters){
	for(int i=0;i<3;i++){
	RxSymbol=readSymbol(can_port_id, false);
	}
	
	int DLC[4]={};										//Receive DLC
	int decDLC=0;
	for (int i=0;i<4;i++){
	RxSymbol=readSymbol(can_port_id, false);
	if(i>=0){
		if(RxSymbol==DOMINANT){
			DLC[i]=0;
		}
		else if(RxSymbol==RECESSIVE){
			DLC[i]=1;
		}
	}
	if (i==0){
		RxFrame.DLC=DLC[0];
	}
	else{
		RxFrame.DLC=(RxFrame.DLC<<1)+DLC[i];
	}
	}													//DLC received
	
	
	int Data=0;									//Receive Data
	int decData=0;
	for (int i=RxFrame.DLC*8-1;i>=0;i--){
			RxSymbol=readSymbol(can_port_id, false);
			if(RxSymbol==DOMINANT){
				Data=0;
			}
			else if(RxSymbol==RECESSIVE){
				Data=1;
			}
			if (i==RxFrame.DLC*8-1){
				RxFrame.Data=Data;
			}
			else{
				RxFrame.Data=(RxFrame.Data<<1)+Data;
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
		RxSymbol=readSymbol(can_port_id, false);
		if(RxSymbol==RECESSIVE && CRC[i]==0){
			break;
		}
		else if(RxSymbol==DOMINANT && CRC[i]==1){
			break;
		}
		else if(RxSymbol==RECESSIVE){
			CRCread[i]=1;
		}
		else{
			CRCread[i]=0;
		}
		if(i==0){
			RxFrame.CRC==CRCread[i];
		}
		else{
			RxFrame.CRC=(RxFrame.CRC<<1)+CRCread[i];
		}
		
		if(i==14){							//&& CRCerror==false
			RxSymbol=readSymbol(can_port_id, false);
			can_phy_tx_symbol(can_port_id, DOMINANT);
			can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
		}
	}
		
	RxSymbol=readSymbol(can_port_id, false);
	
	for (int i=0;i<10;i++){
		RxSymbol=readSymbol(can_port_id, false);
		if (RxSymbol==RECESSIVE){
			RR++;
		}
		if(RR=10){
			DominantR=0;
			RecessiveR=0;
		}
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
	else{
		NewMessage=false;
	}
		}
	else{
		RxSymbol=readSymbol(can_port_id, false);
		if(RxSymbol==RECESSIVE){
			RR++;
			NewMessage=false;
		}
		else if(RxSymbol==DOMINANT){
			NewMessage=false;
			RR=0;
		}
		else{
			RR=0;
			NewMessage=false;
		}
		if(RR>=10){
			RR=10;
			NewMessage=true;
		}
	}
	
	
	
	
	
	
	
	}
  
  
  }
}  