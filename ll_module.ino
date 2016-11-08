#include "messages.h"
#include <SymphonyLink.h>

//Instantiate a SymphonyLink class using the SymphonyLink Arduino Library
SymphonyLink symlink; 

uint8_t txData[1];    //TX data buffer. Could be up to 256 bytes.
uint8_t rxData[128];   //RX data buffer. Could be up to 128 bytes.
uint8_t rxDataLength;
uint8_t radioPath = 1; // set 1 for U.FL, 2 for integrated trace antenna

sym_module_state_t currentSymphonyState;
sym_module_state_t lastSymphonyState;

void setup() 
{
  int ii;
  
  //Arduino Due allows debug and status signals out of the Serial port. UART communications with the SymphonyLink module are done using Serial1.
  Serial.begin(115200);

  //Configure the following to match your network and application tokens
  //Set desired network token 
  uint32_t netToken = 0x02e73684;  //Insert your network token. For example, the OPEN network token is 0x4f50454e.
                                   //A module can only talk to gateways using the same network token.
  //Set desired application token
  uint8_t appToken[APP_TOKEN_LEN] = {0x5c,0x87,0x6e,0xfb,0xf9,0x59,0xa4,0xa1,0x7e,0xff}; //Generate an application token in your Conductor account use it here.
                                                                                          //The application token identifies this dataflow in Conductor.
  
  //Initialize the SymphonyLink object and open UART communications with the module on Serial1.
  symlink.begin(netToken, appToken, LL_DL_MAILBOX, 15);
  
  
  //Initialize the txData. This is the array of hex bytes to be sent over the air, as an example.
  for (ii = 0; ii < sizeof(txData); ii++)
  {
    txData[ii] = 0;
  }

  rxDataLength = sizeof(rxData);

  //Set RF path
  symlink.setAntenna(radioPath);

  uint8_t ret = symlink.systemTimeSync(1);
  Serial.print("return code after time sync request: ");
  Serial.println( ret );
  
  //Update the state of the SymphonyLink module (aka Modem)
  lastSymphonyState = symlink.updateModemState();

  
}

void loop()
{
  //Update the state of the SymphonyLink module (aka Modem)
  currentSymphonyState = symlink.updateModemState();
  switch (currentSymphonyState)
  {
    case SYMPHONY_READY:                            
      if (SYMPHONY_TRANSMITTING != lastSymphonyState) //When SymphonyLink module is ready, send txData
      {
        txData[0]++;                                 //Increment payload
        symlink.write(txData, sizeof(txData), true); //Uplink the payload to Conductor
      
        //Print the payload to the Serial monitor for debug.
        Serial.print("\t... Outbound payload is ");
        symlink.printPayload(txData, sizeof(txData));

        getAndPrintTime();
      }
      else 
      {
         // If last uplink failed, do not increment payload.
         if (LL_TX_STATE_SUCCESS != symlink.getTransmitState())
         {
          txData[0]--;
         }

         //Check for downlink data
         symlink.read(rxData, rxDataLength);
      }
      break;
    default:
      break;
  }
  lastSymphonyState = currentSymphonyState;
}

void getAndPrintTime()
{
                llabs_time_info_t  time_info;
        symlink.getSystemTime( &time_info );
        Serial.println();

    Serial.print("Time sync mode is : ");
    if ( time_info.sync_mode == 0 )
      Serial.println(" Time sync only when requested");
    else
      Serial.println(" Time sync opportunistically");

    Serial.print("Last sync was at : ");
    Serial.println(time_info.last_sync.seconds );

        Serial.print("UTC epoch time is : ");
        Serial.print(time_info.curr.seconds );
        Serial.println(" seconds." );
}
