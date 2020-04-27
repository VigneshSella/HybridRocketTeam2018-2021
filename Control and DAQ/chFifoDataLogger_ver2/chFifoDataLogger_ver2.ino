#include <SD.h>
#include <ChRt.h>

// Data logger based on a FIFO to decouple SD write latency from data
// acquisition timing.
//
// The FIFO uses two semaphores to synchronize between tasks.

// Interval between points in units of 1024 usec on AVR, usec on ARM.
const systime_t intervalTicks = TIME_US2I(1000);
//const systime_t intervalTicks = 1;

//------------------------------------------------------------------------------
// SD file definitions.
const uint8_t sdChipSelect = BUILTIN_SDCARD;
File file;
//------------------------------------------------------------------------------
// Fifo definitions.

// Size of fifo in records.
const size_t FIFO_SIZE = 10000; //assumption

// Counter for real time pressure displays
int i=0;

// Count of data records in fifo.
SEMAPHORE_DECL(fifoData, 0);

// Count of free buffers in fifo.
SEMAPHORE_DECL(fifoSpace, FIFO_SIZE);

// Data type for fifo item.
struct FifoItem_t {
  uint32_t usec;
  int value1;
  int value2;
  int value3;
  int error;
};
// Array of fifo items.
FifoItem_t fifoArray[FIFO_SIZE];
//------------------------------------------------------------------------------
// Declare a stack with 32 bytes beyond task switch and interrupt needs.
static THD_WORKING_AREA(waThread1, 32);

static THD_FUNCTION(Thread1, arg) {
  (void)arg;
  // Index of record to be filled.
  size_t fifoHead = 0;

  // Count of overrun errors.
  int error = 0;
  
  systime_t logTimeTicks = chVTGetSystemTime();
  while (true) {
    logTimeTicks += intervalTicks;
    chThdSleepUntil(logTimeTicks);
    // Get a buffer.
    if (chSemWaitTimeout(&fifoSpace, TIME_IMMEDIATE) != MSG_OK) {
      // Fifo full, indicate missed point.
      error++;
      continue;
    }
    FifoItem_t* p = &fifoArray[fifoHead];
    p->usec = micros();
    p->value1 = analogRead(A7);  
    p->value2 = analogRead(A6); 
    p->value3 = analogRead(A5); 
    p->error = error;
    error = 0;
    
    // Counting up at every data acquisition
    i++;
    
    // Signal new data.
    chSemSignal(&fifoData);
    
    // Advance FIFO index.
    fifoHead = fifoHead < (FIFO_SIZE - 1) ? fifoHead + 1 : 0;
  }
}
//------------------------------------------------------------------------------
void setup() {
  
  analogReadResolution(13);
  Serial.begin(9600);
  
  // Wait for USB Serial.
  while (!Serial) {}
  
  // Start kernel
  chBegin(mainThread);
  // Start input pin
  pinMode(A7, INPUT);
  pinMode(A6, INPUT);
  pinMode(A5, INPUT);
  
  // chBegin() resets stacks and should never return.
  while (true) {}  
}
//------------------------------------------------------------------------------

void mainThread() {

  // FIFO index for record to be written.
  size_t fifoTail = 0;  

  // Time in micros of last point.
  uint32_t last = 0;

  // Remember errors.
  bool overrunError = false;  

  // start producer thread
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO + 1, Thread1, NULL);
  
  Serial.println(F("type any character to begin"));
  while(!Serial.available()); 
  
  // Open file.
  if (!SD.begin(sdChipSelect)) {
    Serial.println(F("SD begin failed."));
    while (true) {}
  }
  file = SD.open("0218B.CSV", O_CREAT | O_WRITE | O_TRUNC);
  if (!file) {
    Serial.println(F("file open  failed."));
    while (true) {} 
  }
  // Throw away input.
  while (Serial.read() >= 0);
  Serial.println(F("type any character to end"));   
  
  // SD write loop.
  while (!Serial.available()) {
    // Wait for next data point.
    chSemWait(&fifoData);

    FifoItem_t* p = &fifoArray[fifoTail];
    if (fifoTail >= FIFO_SIZE) fifoTail = 0;

    // Display real time pressure every 1000 data acquisition
    if(i%5000==1){
      i=0;
      //Serial.println(p->value*0.016337764-0.300943842);
      Serial.print(p->value1);
      Serial.print("\t");
      Serial.print(p->value2);
      Serial.print("\t");
      Serial.println(p->value3);
    }

    // Print interval between points.
    if (last) {
      file.print(p->usec);
    } else {
      file.write("NA");
    }
    last = p->usec;
    file.write(',');
    file.print(p->value1);
    file.write(',');
    file.print(p->value2);
    file.write(',');
    file.print(p->value3);
    //file.write(',');
    //file.println(p->error);

    // Remember error.
    if (p->error) overrunError = true;

    // Release record.
    chSemSignal(&fifoSpace);
    
    // Advance FIFO index.
    fifoTail = fifoTail < (FIFO_SIZE - 1) ? fifoTail + 1 : 0;
  }
  // Close file, print stats and stop.
  file.close();
  Serial.println(F("Done"));
  Serial.print(F("Unused Thread1 stack: "));
  Serial.println(chUnusedThreadStack(waThread1, sizeof(waThread1)));
  Serial.print(F("Unused main stack: "));
  Serial.println(chUnusedMainStack());
  if (overrunError) {
    Serial.println();
    Serial.println(F("** overrun errors **"));
    Serial.println(F("Increase intervalTicks and/or FIFO_SIZE"));
  }
  while (true) {}
}

void loop() {
  // not used
}
