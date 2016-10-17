#include <can_driver_mac_types.h>
#include <can_phy.h>

bool NextSymbol(bool BusData, bool BusDataOld){
	bool Status=false;
	if(BusData!=BusDataOld){
		Status = true;
	}
	else{
		Status = false;
	}
	return Status;
}

bool MsgTransmitID(CAN_FRAME TxFrame){
	can_phy_tx_symbol(can_port_id, DOMINANT);
	waitSymbolTime();
	for(i=1;i<12;i++){
		else if(TxFrame.ID==1 && i==11){
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
		BusData=can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);
		waitSymbolTime();
		if(TxSymbol!=BusData){
			return true;
			break;
		}
		else if (i==11){
			return false;
		}
	}
}

void MsgTransmitDLC(CAN_FRAME TxFrame){
	for(int i=0; i<3; i++){
	can_phy_tx_symbol(can_port_id, DOMINANT);
	waitSymbolTime();
	}
	for(int i=0; i<4; i++){
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
	waitSymbolTime();
	}
}

void MsgTransmitFrame(CAN_FRAME TxFrame){
	
}

/* bool CollisionDetect(bool SelfSend, bool BusData, bool Transmit, int Symbolnr){
	bool Collision=false;
	if(Symbolnr<12){
	//Polling to see if send message corresponds with read message.
		if (BusData==Transmit ){
			Collision=false;
		}
		else{
			Collision=true;
		}
	}
	else{
		Collision=false;
	}
	return Collision;
	}
} */

static void hw_can_mac_driver(
		       volatile CAN_PORT *can_port_id,
		       CAN_FRAME * volatile *TxFrameFromSensor,
		       CAN_FRAME * volatile *RxFrameForActuator,
		       int *rxPrioFilters, uint rxPrioFiltersLen)
{
  CAN_FRAME TxFrame, RxFrame;
  bool newFrameFromSensor;
  CAN_SYMBOL TxSymbol, RxSymbol;
  
  int i=0;
  bool BusData=DOMINANT;
  bool BusDataOld=DOMINANT;
  bool BusIdle=true;
  bool SOF=false;
  bool SelfSend=false;
  bool Collision=false;
  int Symbolnr=0;
  
    
  while (1) {
	//Read from bus
	BusData=can_phy_rx_symbol_blocking(can_port_id,&RxSymbol);

// Scan for idle bus
	if (BusData==RECESSIVE && NextSymbol(BusData,BusDataOld)==true){
		//Start idle bus counter
		i++
		if(i>=11){
			BusIdle=true;
			i=0;
		}
	}
	else if (BusData==DOMINANT && NextSymbol(BusData,BusDataOld)==true){  // Find start of file
		if (i>=11 || BusIdle==true){
			SOF=true;
			i=0;
		}
		else {
			i=0;
			BusIdle=false;
		}
	}
	else {
	}
	
// sender code
	newFrameFromSensor = can_mac_rx_next_frame(TxFrameFromSensor, &TxFrame);
	while (BusIdle==true && newFrameFromSensor==true){
		SelfSend=true;
		Collision=MsgTransmitID(TxFrame);
		if (Collision==true){
			BusIdle=false;
			break;
		}
		MsgTransmitDLC(TxFrame);
		MsgTransmitFrame(TxFrame);
		MsgTransmitCRC();
		MsgTransmitShit();
	}
	
// Receiver code
	while (){
		
	}
	
// Set some counters and constants
	BusDataOld=BusData;
	Symbolnr++;
  }
	  