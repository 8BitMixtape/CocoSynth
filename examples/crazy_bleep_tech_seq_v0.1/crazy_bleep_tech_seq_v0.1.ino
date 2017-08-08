#include <util/delay.h>
#include <NeoSynth.h>

NeoSynth synth;

#define DEBUG PB1

#define numVoices 4
#define numTracks 4

int steps = 8;     // 8 steps
int triggers = 32; // 32 triggers (notes)

const unsigned int NUM_NOTES = 7;
float notes[NUM_NOTES] = {
  523.2511306012,
  622.25396744373,
  739.98884542224,
  987.76660250974,
  1046.5022612024,
  1174.6590716691,
  1567.9817439245
};

//static boolean initTriggers[] =    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static boolean noiseTriggers[] =  {1,5,5,5,100,5,5,5,1,0,5,0,100,5,5,0,1,0,5,5,100,0,5,0,1,0,5,0,100,0,5,0};
static boolean sineTriggers[] =  {1,0,0,0,1,0,300,0,1,0,0,0,1,0,0,0,1,0,300,0,1,0,0,0,1,0,0,0,1,0,0,0}; // bassdrum

static boolean hatsTriggers[] = {0,0,100,0,0,0,1,0,0,0,100,0,0,0,1,0,0,0,100,0,0,0,1,0,0,0,100,0,0,0,1,0}; // hats

static boolean squareTriggers[] = {0,0,512,0,0,0,0,0,512,0,0,0,512,0,0,0,0,600,800,20,40,0,0,0,512,0,0,0,700,800,900,1023};

static boolean clearTriggers[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

long int timeStep;
long int timeTrigger;
int bpm = 140;      // tempo in beats per minute
int tempoTrigger = 1000.0 / (bpm / 60.0) / 4.0;      // delay for 32nd trigger

int swing = 5;          // swing

typedef struct {
  int voice;
  boolean triggers[32]; // 32 triggers
  //boolean mod[32]; // 32 triggers
} Track;

Track tracks[numTracks];

void setup()
{

  synth.begin();
  synth.setupVoice(2,SQUARE,64,ENVELOPE1,40,64);
  synth.setupVoice(3,NOISE,64,ENVELOPE3,60,64);
  //synth.setupVoice(3,SINE,127,ENVELOPE2,50,64);
  //synth.setupVoice(0,SINE,127,ENVELOPE3,40,64);
  
  pinMode(DEBUG, OUTPUT); // set debug LED to output
  
  for (int i = 0; i < numVoices; i++) {
    Track tr = {i,0};
    tracks[i] = tr;
    memcpy(tracks[i].triggers, clearTriggers , sizeof tracks[i].triggers); // init triggers to 0.
  }
  memcpy(tracks[1].triggers, noiseTriggers , sizeof tracks[0].triggers); // init track 0 
  memcpy(tracks[2].triggers, sineTriggers , sizeof tracks[1].triggers); // 
  //memcpy(tracks[3].triggers, squareTriggers , sizeof tracks[2].triggers); // 
  //memcpy(tracks[0].triggers, hatsTriggers , sizeof tracks[3].triggers); // 
  //Serial.begin(9600);
  synth.delay(100);  
}

void trigger(int v, int env) {
  if (env > 0) {
    synth.setMod(v, env);
    synth.trigger(v);
  }

}


void loop() {
  unsigned static int phrase_ct = 0;
  
  for (int t = 0; t < triggers ; t++) {  // 32th notes
      
    timeTrigger = synth.millis();

    for (int tr = 0; tr < numTracks; tr++) {
      trigger( tr , tracks[tr].triggers[t] );
    }
      
    long int elapsedTime = synth.millis() - timeTrigger;
    int d = tempoTrigger - elapsedTime;
    
    if (d > 0) {
      
      if (t % 2 == 0) {        
        synth.delay(d + (int)(float(d)*(float)swing/100.0));  // swing delay
      }
      else {    
        synth.delay(d - (int)(float(d)*(float)swing/100.0));
      } 
    }
    
  }

 
  phrase_ct++;

  if (phrase_ct % 4 == 0) {
    synth.setupVoice(1,SQUARE,64,ENVELOPE1,60,64);
    synth.setupVoice(2,SINE,64,ENVELOPE0,70,64);
  }

  if (phrase_ct  % 7 == 0) {
    synth.setupVoice(1,SQUARE,64,ENVELOPE0,60,64);
    synth.setupVoice(2,NOISE,64,ENVELOPE1,150,64);
  }

  if (phrase_ct  % 3 == 0) {
    memcpy(tracks[1].triggers, noiseTriggers , sizeof tracks[1].triggers); // init track 0 
    memcpy(tracks[2].triggers, sineTriggers , sizeof tracks[2].triggers); // 
  }
  if (phrase_ct  % 8 == 0) {
    memcpy(tracks[0].triggers, squareTriggers , sizeof tracks[1].triggers); // init track 0 
    memcpy(tracks[1].triggers, clearTriggers , sizeof tracks[2].triggers); // 
  }
  
  
  
}


//disable millis
//make timer available for NeoSynth
int main(void)
{
    //init();
    setup();
    for (;;)
        loop();
    return 0;
} 

