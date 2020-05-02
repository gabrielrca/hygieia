#include<EEPROM.h>

#define retrospective_window_size 10
#define quantity_of_samples_in_EEPROM 499

#define node_id 0xF3 //Id of the node
#define tdma_window 2000 //time of tdma window in miliseconds
#define quantity_of_nodes_in_tdma 4

/*
 * tdma scheme: 3 CN and 1 FN (4 nodes in total)
 * Each node has the time in tdma_window to process Hygiea and send the message
 * IDs of the CN: 0xF1, 0xF2, 0XF3
 * ID of the FN: 0xF0
 * Broadcast ID: 0xFF
 */

int EEPROM_Address = 0;
int retrospective_index=0;
int RSW[retrospective_window_size];
int datum;

void swap(int *xp, int *yp){
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
 
// A function to implement bubble sort
void bubbleSort(int arr[], int n){
   int i, j;
   for (i = 0; i < n-1; i++)        
       for (j = 0; j < n-i-1; j++) 
           if (arr[j] > arr[j+1])
              swap(&arr[j], &arr[j+1]);
}

int percentil(int dataset[], float percentil, int dataSize){
  int index= dataSize * percentil;
  return dataset[index];
}

void setup() {
  //Serial.begin(9600);
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  }

void loop() {

  //This switch statement is to define TDMA window based on node ID
  //The TDMA sequence is 
  //CN1 (id 0xF1), CN2 (id 0xF2), CN3 (id 0xF3) and FN (id 0xF0)
  switch(node_id){
    case 0xF1 :
      break;
    case 0xF2 :
      delay(tdma_window*1);
      break;
    case 0xF3 :
      delay(tdma_window*2);
      break;
    }

   //Hygieia dont receive data from serial, hence it should clean the received data
   //if it does no clean the serial, the serial buffer will increase until fill up
   while(Serial.available()){Serial.read();}

  //Turn on the Builtin led to inform 
  //it is operating Hygieia in its desired TDMA slot  
  digitalWrite(LED_BUILTIN, HIGH);

  
  //Gets a datum from EEPROM and store it on RSW
  int Datum = EEPROM.read(EEPROM_Address);
  RSW[retrospective_index] = Datum;


    
  //Updates the RSW index and the EEPROM index  
  retrospective_index=retrospective_index+1;
  EEPROM_Address = EEPROM_Address + 1;
  
   if(retrospective_index > 1){
    //Creates another array for sorting the data
    int data[retrospective_window_size];



    //Copy the RSW to data array
    for(int i=0; i<(retrospective_index); i++){
      data[i]= RSW[i];  
    }
    
    /*for(int i=0; i<(retrospective_index); i++){
      Serial.print(i);
      Serial.print(" ");
      Serial.println(data[i]);  
    }*/

    //Sort the data 
    bubbleSort(data, (retrospective_index+1));
  


    //Gets the p25 and p75 from data
    int p25 = percentil(data, 0.25, retrospective_index + 1 );
    int p75 = percentil(data, 0.75, retrospective_index + 1 );


    int iqr = p75 - p25;

    float boundary = iqr*1.75;

    if((Datum > p75 + boundary) or (Datum < p25 - boundary)){
      //In this case an abnormal data was detected
      retrospective_index = retrospective_index -1;  //This data is taken off the array
      Serial.write(node_id);
      Serial.write(255);
      Serial.write(Datum);
    }else{
      //Otherwise a normal data was found
      Serial.write(node_id);
      Serial.write(0);
      Serial.write(Datum);
    }

    if(retrospective_index == retrospective_window_size){
        //Taking off the first datum that arrived  
        for(int i=0; i<retrospective_window_size; i++){
    
          if(i + 1 < retrospective_window_size){
              RSW[i]=RSW[i+1];
          }else{
              RSW[i]= 0;
              retrospective_index = retrospective_index -1;
          }
         }
    }
  }

  //Turn off the buitin led to inform it is waiting
  digitalWrite(LED_BUILTIN, LOW);

  //This switch statement is to define TDMA window based on node ID
  //The TDMA sequence is 
  //CN1 (id 0xF1), CN2 (id 0xF2), CN3 (id 0xF3) and FN (id 0xF0)
  switch(node_id){
    case 0xF1 :
      delay(tdma_window*3);
      break;
    case 0xF2 :
      delay(tdma_window*2);
      break;
    case 0xF3 :
      delay(tdma_window*1);
      break;
    }




  if (EEPROM_Address == (quantity_of_samples_in_EEPROM + 1)) {
    while(true){}
  }

}
