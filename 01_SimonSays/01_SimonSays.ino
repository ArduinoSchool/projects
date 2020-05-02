#include <LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>

#define RS 13
#define EN 10
#define D4 8
#define D5 6
#define D6 5
#define D7 4


#define PIN_PIXELS 7
#define NUMPIXELS 4
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800);

#define MAX_SEQUENCE 5
#define INITIAL_SPEED_MS 700

#define BUZZER 9

enum simonStates {
	RESET=0,
  	SHOWING,
  	WAITING,
  	WIN,
  	LOSE
};
simonStates simonState=RESET;
simonStates prevSimonState=RESET;

int sequence[MAX_SEQUENCE];
int colors[NUMPIXELS][3];
int freq[]={262,330,392,494};

int prevButtonState[]={false,false,false,false};
int currentButtonState[]={false,false,false,false};
int buttonsPins[]={12,11,2,3};

int currentSequenceIndex=0;
int waitingIndex=0;

unsigned int speedMs = INITIAL_SPEED_MS;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void setup(){
  lcd.begin(16,2);
  Serial.begin(115200);
  pixels.begin();

  pinMode(BUZZER,OUTPUT);
  for(int i=0;i<NUMPIXELS;i++){
    pinMode(buttonsPins[i],INPUT_PULLUP);
  }
  
  randomSeed(analogRead(A0));
  lcd.print("By@arduino.shool");
  lcd.setCursor(0,1);
  lcd.print("SIMON:");
}

unsigned long prevTime=0;

void loop()
{

  switch(simonState){
    case RESET:{
      stateRESET();
    }break;
    case SHOWING:{
      stateSHOWING();
    }break;
    case WAITING:{
      stateWAITING(); 
    }break;
    case LOSE:{
      stateLOSE(); 
    }break;
    case WIN:{
      stateWIN(); 
    }break;
    default:{
    }break;
  }
  delay(100);
}


void stateRESET(){
  
  	currentSequenceIndex=0;
    for(int i=0;i<MAX_SEQUENCE;i++){
	  sequence[i]=-1; //Inicializamos la secuencia en -1
  	}
  
  	colors[0][0]=0;
    colors[0][1]=0;
    colors[0][2]=255;
  
  	colors[1][0]=0;
    colors[1][1]=255;
    colors[1][2]=0;
  
    colors[2][0]=255;
    colors[2][1]=0;
    colors[2][2]=0;
  
 
    colors[3][0]=233;
    colors[3][1]=255;
    colors[3][2]=0;
  
    //SIMON:(HERE)
  	lcd.setCursor(6,1);
  	lcd.print("Go!       ");
	lcd.setCursor(6,1);
  	delay(speedMs);
    lcd.print("   ");
	lcd.setCursor(6,1);

  	prevSimonState=RESET;
	goToSHOWING();
  	
}

void stateWIN(){
  
    for(int i=0;i<50;i++){
      
      for(int j=0;j<NUMPIXELS;j++){
        int r = random(0,256);
        int g = random(0,256);
        int b = random(0,256);
      	pixels.setPixelColor(j, pixels.Color(r,g,b));
      }
      pixels.show();
      delay(50);
  	}
  
  	prevSimonState=LOSE;
	goToRESET();
  	
}

void stateLOSE(){
  
    for(int i=0;i<6;i++){
      
      for(int j=0;j<NUMPIXELS;j++){
      	pixels.setPixelColor(j, pixels.Color(255,0,0));
      }
      pixels.show();
      delay(250);
	  pixels.clear();
      pixels.show();
      delay(250);

  	}
  
  	prevSimonState=LOSE;
	goToRESET();
  	
}

void stateSHOWING(){
  for(int i=0;i<=currentSequenceIndex;i++){
    if(currentSequenceIndex>1&&(sequence[i]==sequence[i-1])){
    	delay(speedMs/2);
    }
    Serial.print("[SHOWING]:Showing ");
    Serial.println(sequence[i]);
    
    lcd.print(sequence[i]);
 	lcd.setCursor(6,1);

  	pixels.setPixelColor(sequence[i], pixels.Color(colors[sequence[i]][0],colors[sequence[i]][1],colors[sequence[i]][2]));
    pixels.show();
    tone(BUZZER,freq[sequence[i]]);
    delay(speedMs);
    noTone(BUZZER);
    pixels.clear();
    pixels.show();
  }
  prevSimonState=SHOWING;
  goToWAITING();
}

void stateWAITING(){
  
  for(int i=0;i<NUMPIXELS;i++){
    currentButtonState[i]=!digitalRead(buttonsPins[i]);
    
    if(prevButtonState[i]==false&&currentButtonState[i]==true){
      
      Serial.print("[WAITING]Button ");
      Serial.print(i);
      Serial.println(" pressed.");
      
      pixels.setPixelColor(i, pixels.Color(colors[i][0],colors[i][1],colors[i][2]));
      pixels.show();
      tone(BUZZER,freq[i]);
      
      if(i!=sequence[waitingIndex]){
      	Serial.println("[WAITING]WRONG");
        noTone(BUZZER);
        pixels.clear();
        pixels.show();
        goToLOSE();
      }

    }
    
    if(prevButtonState[i]==true&&currentButtonState[i]==false){
      
      Serial.print("[WAITING]Button ");
      Serial.print(i);
      Serial.println(" realesed.");    
      
      noTone(BUZZER);
      pixels.clear();
      pixels.show();
      
      if(i==sequence[waitingIndex]){
      	Serial.println("[WAITING]CORRECT");
        if(waitingIndex==currentSequenceIndex){
          Serial.println("[WAITING]Was the last");
          currentSequenceIndex++;
          goToSHOWING();
        }else{
          Serial.println("[WAITING]waiting next");
          waitingIndex++;
        }
      } 
    }
    
	prevButtonState[i]=currentButtonState[i];
    
  }
}


void goToSHOWING(){  
  Serial.println("goToSHOWING");
  if(currentSequenceIndex<MAX_SEQUENCE){
   int newPixel = random(0,NUMPIXELS);
   if(currentSequenceIndex>2){
     while(newPixel==sequence[currentSequenceIndex-1]&&newPixel==sequence[currentSequenceIndex-2]){
     	newPixel = random(0,NUMPIXELS);
     }
   }
   sequence[currentSequenceIndex]=newPixel;
   delay(1000);
   pixels.clear();
   pixels.show();
   noTone(BUZZER);
   lcd.print("     ");
   lcd.setCursor(6,1);
    
   simonState=SHOWING;
  }else{
   goToWIN();
  }	
}

void goToRESET(){
  Serial.println("goToRESET");
  simonState=RESET;
}

void goToWIN(){
  Serial.println("goToWIN");
  simonState=WIN;
  
  lcd.print("  WIN  ");
  lcd.setCursor(6,1);
}

void goToLOSE(){
  Serial.println("goToLOSE");
  simonState=LOSE;
  lcd.print("  LOSE  ");
  lcd.setCursor(6,1);
}

void goToWAITING(){
  waitingIndex=0;
  Serial.println("goToWAITING");
  
  lcd.print("You!");
  lcd.setCursor(6,1);
 
  simonState=WAITING;
}