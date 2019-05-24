#include "mbed.h"

#include "ultrasonic.h"
#include "encoder.h"

#define RATE 0.1

PwmOut ba(PA_9);
DigitalOut bb(PA_6);
PwmOut aa(PA_7);
DigitalOut ab(PA_11);
Encoder motorright(PB_5,PB_7, true);
Encoder motorleft (PB_0, PA_12, true);

ultrasonic mu(PA_0, PA_1, .1, 1, &dist);
int distanceLimit=3000;

DigitalIn btn(PB_1);
DigitalOut myled(LED1);
DigitalOut ledmain(PA_10);
DigitalIn line(PA_8);




int vzdalenost = 0;

int casovacBtn = 0;

int state = 0;
bool running = false;

int speedleft;
int speedright;
int prumernaRychlost;
double firstspeedright;
double firstspeedleft;
int posright;
int posleft;
int firstposvalue;
int posvaluecount;
int summedpos;
int lastsummedpos;


bool barvaDefault = false;

int counterbliknutiWait;
int counterSkenu;
int counterUtoku=0;
int counterCuknuti;

int vzorkovacRychlosti;
int vzorekPrumerneRychlosti;
int cukTimer;

bool smerOtaceni = false;

Serial pc(USBTX, USBRX);




void speed() {
    
    posleft = motorleft.getPosition();
    posright = motorright.getPosition();
    summedpos = abs(posright)+abs(posleft);
    firstspeedright = -1*motorright.getSpeed();
    if (firstspeedright > 0.007 and firstspeedright < 0.008 or firstspeedright < -0.007 and firstspeedright > -0.008 ) {
        //pc.puts("SpatnaHodnota\r\n");
        
        } else {
        speedright = firstspeedright / 20;
        }
        
    firstspeedleft = motorleft.getSpeed();
    if (firstspeedleft > 0.007 and firstspeedleft < 0.008 or firstspeedleft < -0.007 and firstspeedleft > -0.008 ) {
        //pc.puts("SpatnaHodnota\r\n");
        
        } else {
        speedleft = firstspeedleft / 20;
        }
    }

void dist(int distance) {
    //code to execute when the distance has changed
    
    if (distance < distanceLimit) {
        vzdalenost = distance;
        }
    }
    


void motors(int smer, const char* rychlost) {
    //1 DOPREDU      2 DOZADU      3 DOLEVA        4Doprava        5STOP
    if (smer == 1) {
        ab = true;
        bb = true;
        ba=0.1;
        aa=0.1;
        //pc.puts("running dopredu\r\n");
    }else if(smer == 2) {
        ab = false;
        bb = false;
        ba=0.9;
        aa=0.9;
        //pc.puts("running dozadu\r\n");
    }else if(smer == 3) {
        ab = false;
        bb = true;
        if (rychlost=="rychla") {
            ba=0;
            aa=1;
        }else if(rychlost=="stredni"){
            ba=0.35;
            aa=0.65;
        }else if(rychlost=="pomala"){
            ba=0.5;
            aa=0.5;
            }
        //pc.puts("running doleva\r\n");
    }else if(smer == 4) {
        ab = true;
        bb = false;
        ba=0.8;
        aa=0.1;
        if (rychlost=="rychla") {
            ba=1;
            aa=0;
        }else if(rychlost=="stredni"){
            ba=0.65;
            aa=0.35;
        }else if(rychlost=="pomala"){
            ba=0.5;
            aa=0.5;
            }
        //pc.puts("running doprava\r\n");
    }else if(smer == 5) {
        ab = false;
        bb = false;
        ba=0;
        aa=0;
        //pc.puts("STOP\r\n"); 
        }
        
    }
    
void bliknutiWait(int quantity, float waitingtime){
    for(counterbliknutiWait=0; counterbliknutiWait < quantity; ++counterbliknutiWait){
        ledmain = true;
        wait(waitingtime);
        ledmain = false;
        wait(waitingtime);
        }
    }



void buttonWatch() {
    if (btn == false) {
            casovacBtn = casovacBtn+1;
    } else {
            if (casovacBtn >= 1 and casovacBtn < 5) {
                if (state == 0) {
                    bliknutiWait(5, 0.5);
                    mu.startUpdates(); //start measuring the distance
                    pc.puts("Tlacitko. running.. state je OTOCKASTART\r\n");
                    cukTimer=1;
                    counterSkenu=0;
                    counterSkenu=0;
                    counterUtoku=0;
                    vzorkovacRychlosti=0;
                    vzorekPrumerneRychlosti=0;
                    
                    //firstposvalue = 0;
                    motorright.setPosition(0);
                    motorleft.setPosition(0);
                    state = 1;
                    running = true;
                    smerOtaceni=false;
                    motors(4,"rychla"); //otoc se 90
                }else{
                    mu.pauseUpdates();//stops measuring the distance
                    pc.puts("Tlacitko. Nejedu..\r\n");   
                    state = 0;
                    running = false;
                    }
                casovacBtn = 0;    
            }else if (casovacBtn > 20) {
                pc.puts("Tlacitko. Dlouze. Nastateuju barvu kruhu\r\n");
                barvaDefault = line;
                bliknutiWait(3,0.5);
                
                casovacBtn = 0; 
                
                }
            }
    
    }

    


int main() {
    ba.period_ms(50);
    aa.period_ms(50);
    btn.mode(PullUp);
    state = 0;
    bliknutiWait(5,0.1);
    pc.printf("xd cau %d \r\n");
    //pc.printf("pwm set to %.2f %%\n", mypwm.read() * 100);
    
    while(true) {
        wait(RATE);
        if (running == true) {
            speed(); //funkce na kontrolovani enkoderuu
            mu.checkDistance();  //funkce na aktualizovani vzdalenosti ultrazvuku
            
            //pc.printf("Rychlost leveho moto je %d mm\r\n", speedleft);
            //pc.printf("Rychlost praveho moto je %d mm\r\n", posright);
            pc.printf("state je %d mm\r\n", state);
            
            if (line!=barvaDefault) {
                motors(2,"rychla");
                motorright.setPosition(0);
                motorleft.setPosition(0);
                //firstposvalue = 0;
                state=4;
                }
            
            if (state == 1) { //kdyz state se rovna OTOCKA na STARTU
                
                
                posvaluecount = summedpos-firstposvalue;
                if (posvaluecount>1350) {
                    counterSkenu=0;
                    state = 2;
                    motorright.setPosition(0);
                    motorleft.setPosition(0);
                    motors(1, "rychla");
                    motors(3,"rychla");
                    //motors(2,"rychla"); //jed dozadu
                    //firstposvalue = 0;
                    }
                
    
            } else if (state == 2) { //kdyz state se rovna RYCHLY SKEN
                //pc.printf("Ujel jsem %d %d %d zadnych jednotek\r\n", firstposvalue,summedpos,posvaluecount);
                //pc.printf("%d \r\n",posvaluecount);
                if (smerOtaceni==false){ //false znamena doleva
                    if (posvaluecount<summedpos){
                        posvaluecount = summedpos-firstposvalue;
                        //pc.printf("poscount je mensi\r\n");
                    }else{
                        posvaluecount = firstposvalue-summedpos;
                        //pc.printf("poscount je vetsi\r\n");
                        }
                    if (posvaluecount<110) { //kdyz pocet kroku motoru je mensi nez..
                        
                        
                    }else{
                        counterSkenu++;
                        smerOtaceni=true;
                        motorright.setPosition(0);
                        motorleft.setPosition(0);
                        motors(4,"stredni"); //jed doprava
                        //////firstposvalue = 0;
                        }
                }else{
                    if (posvaluecount<summedpos){
                        posvaluecount = summedpos-firstposvalue;
                        //pc.printf("poscount je mensi\r\n");
                    }else{
                        posvaluecount = firstposvalue-summedpos;
                        //pc.printf("poscount je vetsi\r\n");
                        }
                    if (posvaluecount<100) { //kdyz pocet kroku motoru je mensi nez..
                        
                    }else{
                        counterSkenu++;
                        smerOtaceni=false;
                        motorright.setPosition(0);
                        motorleft.setPosition(0);
                        motors(1, "rychla");
                        motors(3,"rychla"); //jed doleva
                        //firstposvalue = 0;
                        }
                    }
                if (vzdalenost<300) { //jestli vzdalenost je mensi nez 100 nastate state na UTOK
                    state = 3;
                    motors(1,"rychla");
                    pc.printf("menim state na 3\r\n");
                    }
                if (counterSkenu>10){
                    counterSkenu=0;
                    state=6;
                    motors(1,"rychla");
                    motors(3,"stredni");
                    }
                    
            } else if (state == 3){ //kdyz state se rovna UTOK
                if (cukTimer>0) {
                    ++cukTimer;
                    }
                counterUtoku++;
                if (counterUtoku>0 and counterUtoku<22){
                    vzorkovacRychlosti= vzorkovacRychlosti+abs(speedright)+abs(speedleft);
                    pc.printf("1 \r\n");
                    }
                if (counterUtoku==5) {
                    vzorekPrumerneRychlosti=vzorkovacRychlosti/5;
                    vzorkovacRychlosti=0;
                    pc.printf("2 \r\n");
                    }
                else if (counterUtoku==16){
                    prumernaRychlost=vzorkovacRychlosti/10; 
                    vzorkovacRychlosti=0;
                    counterUtoku=6;
                    pc.printf("3 \r\n");
                    } 
                    
                //pc.printf("%d %d %d %d \r\n", vzorekPrumerneRychlosti,vzorkovacRychlosti, prumernaRychlost, vzdalenost);
                if (cukTimer>30) {
                    if (vzorekPrumerneRychlosti-prumernaRychlost>65){
                        state=8;
                        motors(2, "rychla");
                        motorright.setPosition(0);
                        motorleft.setPosition(0);
                        cukTimer=0;
                        }
                    }
                if (vzdalenost>310){
                    counterSkenu=0;
                    state=2;
                    motorright.setPosition(0);
                    motorleft.setPosition(0);
                    motors(1, "rychla");
                    motors(3,"rychla");
                    smerOtaceni=false;
                    //firstposvalue = 0;
                    }
            
            } else if (state == 4){ //kdyz state se rovna COUVANI
                //pc.printf("state je couvanik zacatek\r\n");
                if (posvaluecount<summedpos){
                    posvaluecount = summedpos-firstposvalue;
                    //pc.printf("poscount je mensi\r\n");
                }else{
                    posvaluecount = firstposvalue-summedpos;
                    //pc.printf("poscount je vetsi\r\n");
                    }
                //pc.printf("Ujel jsem %d %d %d zadnych jednotek\r\n", firstposvalue,summedpos,posvaluecount);
                //pc.printf("state je couvani konec\r\n");
                
                if (posvaluecount<600) { //kdyz pocet kroku motoru je mensi nez..
                    
                }else{
                  state = 5;
                  motors(4,"stredni"); //otoc se 180
                  }
                      
                      
            } else if (state == 5){ //kdyz state se rovna OTOCKA 180  
                
                posvaluecount = summedpos-firstposvalue;
                if (posvaluecount>2000) {
                    counterSkenu=0;
                    state = 2;
                    motorright.setPosition(0);
                    motorleft.setPosition(0);
                    motors(1, "rychla");
                    motors(3,"rychla");
                    //firstposvalue = 0;
                    }
                    
            } else if (state == 0) { //kdyz state se rovna nejedu
                motors(1,"rychla");
                
            
            } else if(state==6) { //kdyz state se rovna stredni sken
                pc.printf("%d \r\n",counterSkenu);
                pc.puts("state je stredni sken\r\n");
                if (vzdalenost<400) { //jestli vzdalenost je mensi nez 100 nastate state na UTOK
                    state = 3;
                    motors(1,"rychla"); }
                ++counterSkenu;
                if (counterSkenu>60) {
                    state=7;
                    counterSkenu=0;
                    motors(1,"rychla");
                    motors(4, "pomala");
                    ba=0.5;
                    aa=0.5;
                    }
            } else if (state==7){ //kdyz state se rovna pomaly sken
                ++counterSkenu;
                if (vzdalenost<600) { //jestli vzdalenost je mensi nez 100 nastate state na UTOK
                    state = 3;
                    motors(1,"rychla"); }
                if (counterSkenu>100) {
                    state=2;
                    counterSkenu=0;
                    motors(1, "rychla");
                    motors(4,"stredni");
                    motorright.setPosition(0);
                    motorleft.setPosition(0);
                }
            } else if (state==8) { //kdyz state je cuknuti-dozadu
                pc.printf("cukaaaam\r\n");
                ++counterCuknuti;
                if (counterCuknuti>0.5){
                    counterCuknuti=0;
                    cukTimer=1;
                    counterSkenu=0;
                    counterUtoku=11;
                    vzorkovacRychlosti=0;
                    
                    motors(1,"rychla");
                    state = 3;
                    }
                }
        }else{
            motors(5, "rychla");
            }
        
         
        buttonWatch();
    }
}
