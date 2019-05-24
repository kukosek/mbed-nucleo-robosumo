#include "mbed.h"

#include "ultrasonic.h"
#include "encoder.h"

#define RATE 0.1

PwmOut ba(PA_9);
DigitalOut bb(PA_6);
PwmOut aa(PA_7);
DigitalOut ab(PA_11);

int vzdalenost = 0;

int casovacBtn = 0;


/*int NEJEDU = 0;
int SKEN = 2;
int OTOCKA = 4;
int UTOK = 3;
int OTOCKASTART = 1;*/
int stav = 0;
bool jedu = false;

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

//Kc, Ti, Td, interval


DigitalIn btn(PB_1);
DigitalOut myled(LED1);
DigitalOut ledmain(PA_10);
DigitalIn line(PA_8);
bool barvaDefault = false;

int pocitadlobliknutiWait;
int pocitadloSkenu;
int pocitadloUtoku=0;
int pocitadloCuknuti;

int vzorkovacRychlosti;
int vzorekPrumerneRychlosti;
int cukTimer;

bool smerOtaceni = false;

Serial pc(USBTX, USBRX)
;
Encoder motorright(PB_5,PB_7, true);
Encoder motorleft (PB_0, PA_12, true);



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

void dist(int distance)
{
    //put code here to execute when the distance has changed
    
    if (distance < 3000) {
        vzdalenost = distance;
        }
    }
    
ultrasonic mu(PA_0, PA_1, .1, 1, &dist);

void motory(int smer, const char* rychlost) {
    //1 DOPREDU      2 DOZADU      3 DOLEVA        4Doprava        5STOP
    if (smer == 1) {
        ab = true;
        bb = true;
        ba=0.1;
        aa=0.1;
        //pc.puts("Jedu dopredu\r\n");
    }else if(smer == 2) {
        ab = false;
        bb = false;
        ba=0.9;
        aa=0.9;
        //pc.puts("Jedu dozadu\r\n");
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
        //pc.puts("Jedu doleva\r\n");
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
        //pc.puts("Jedu doprava\r\n");
    }else if(smer == 5) {
        ab = false;
        bb = false;
        ba=0;
        aa=0;
        //pc.puts("STOP\r\n"); 
        }
        
    }
    
void bliknutiWait(int quantity, float waitingtime){
    for(pocitadlobliknutiWait=0; pocitadlobliknutiWait < quantity; ++pocitadlobliknutiWait){
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
                if (stav == 0) {
                    bliknutiWait(5, 0.5);
                    mu.startUpdates(); //start measuring the distance
                    pc.puts("Tlacitko. Jedu.. stav je OTOCKASTART\r\n");
                    cukTimer=1;
                    pocitadloSkenu=0;
                    pocitadloSkenu=0;
                    pocitadloUtoku=0;
                    vzorkovacRychlosti=0;
                    vzorekPrumerneRychlosti=0;
                    
                    //firstposvalue = 0;
                    motorright.setPosition(0);
                    motorleft.setPosition(0);
                    stav = 1;
                    jedu = true;
                    smerOtaceni=false;
                    motory(4,"rychla"); //otoc se 90
                }else{
                    mu.pauseUpdates();//stops measuring the distance
                    pc.puts("Tlacitko. Nejedu..\r\n");   
                    stav = 0;
                    jedu = false;
                    }
                casovacBtn = 0;    
            }else if (casovacBtn > 20) {
                pc.puts("Tlacitko. Dlouze. Nastavuju barvu kruhu\r\n");
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
    stav = 0;
    bliknutiWait(5,0.1);
    pc.printf("xd cau %d \r\n");
    //pc.printf("pwm set to %.2f %%\n", mypwm.read() * 100);
    
    while(true) {
        wait(RATE);
        if (jedu == true) {
            
            speed(); //funkce na kontrolovani enkoderuu
            mu.checkDistance();  //funkce na aktualizovani vzdalenosti ultrazvuku
            
            //pc.printf("Rychlost leveho moto je %d mm\r\n", speedleft);
            //pc.printf("Rychlost praveho moto je %d mm\r\n", posright);
            pc.printf("Stav je %d mm\r\n", stav);
            
            if (line!=barvaDefault) {
                motory(2,"rychla");
                motorright.setPosition(0);
                motorleft.setPosition(0);
                //firstposvalue = 0;
                stav=4;
                }
            
            if (stav == 1) { //kdyz stav se rovna OTOCKA na STARTU
                
                
                posvaluecount = summedpos-firstposvalue;
                if (posvaluecount>1350) {
                    pocitadloSkenu=0;
                    stav = 2;
                    motorright.setPosition(0);
                    motorleft.setPosition(0);
                    motory(1, "rychla");
                    motory(3,"rychla");
                    //motory(2,"rychla"); //jed dozadu
                    //firstposvalue = 0;
                    }
                
    
            } else if (stav == 2) { //kdyz stav se rovna RYCHLY SKEN
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
                        pocitadloSkenu++;
                        smerOtaceni=true;
                        motorright.setPosition(0);
                        motorleft.setPosition(0);
                        motory(4,"stredni"); //jed doprava
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
                        pocitadloSkenu++;
                        smerOtaceni=false;
                        motorright.setPosition(0);
                        motorleft.setPosition(0);
                        motory(1, "rychla");
                        motory(3,"rychla"); //jed doleva
                        //firstposvalue = 0;
                        }
                    }
                if (vzdalenost<300) { //jestli vzdalenost je mensi nez 100 nastav stav na UTOK
                    stav = 3;
                    motory(1,"rychla");
                    pc.printf("menim stav na 3\r\n");
                    }
                if (pocitadloSkenu>10){
                    pocitadloSkenu=0;
                    stav=6;
                    motory(1,"rychla");
                    motory(3,"stredni");
                    }
                    
            } else if (stav == 3){ //kdyz stav se rovna UTOK
                if (cukTimer>0) {
                    ++cukTimer;
                    }
                pocitadloUtoku++;
                if (pocitadloUtoku>0 and pocitadloUtoku<22){
                    vzorkovacRychlosti= vzorkovacRychlosti+abs(speedright)+abs(speedleft);
                    pc.printf("1 \r\n");
                    }
                if (pocitadloUtoku==5) {
                    vzorekPrumerneRychlosti=vzorkovacRychlosti/5;
                    vzorkovacRychlosti=0;
                    pc.printf("2 \r\n");
                    }
                else if (pocitadloUtoku==16){
                    prumernaRychlost=vzorkovacRychlosti/10; 
                    vzorkovacRychlosti=0;
                    pocitadloUtoku=6;
                    pc.printf("3 \r\n");
                    } 
                    
                pc.printf("%d %d %d %d \r\n", vzorekPrumerneRychlosti,vzorkovacRychlosti, prumernaRychlost, vzdalenost);
                if (cukTimer>30) {
                    if (vzorekPrumerneRychlosti-prumernaRychlost>65){
                        stav=8;
                        motory(2, "rychla");
                        motorright.setPosition(0);
                        motorleft.setPosition(0);
                        cukTimer=0;
                        }
                    }
                if (vzdalenost>310){
                    pocitadloSkenu=0;
                    stav=2;
                    motorright.setPosition(0);
                    motorleft.setPosition(0);
                    motory(1, "rychla");
                    motory(3,"rychla");
                    smerOtaceni=false;
                    //firstposvalue = 0;
                    }
            
            } else if (stav == 4){ //kdyz stav se rovna COUVANI
                //pc.printf("Stav je couvanik zacatek\r\n");
                if (posvaluecount<summedpos){
                    posvaluecount = summedpos-firstposvalue;
                    //pc.printf("poscount je mensi\r\n");
                }else{
                    posvaluecount = firstposvalue-summedpos;
                    //pc.printf("poscount je vetsi\r\n");
                    }
                //pc.printf("Ujel jsem %d %d %d zadnych jednotek\r\n", firstposvalue,summedpos,posvaluecount);
                //pc.printf("Stav je couvani konec\r\n");
                
                if (posvaluecount<600) { //kdyz pocet kroku motoru je mensi nez..
                    
                }else{
                  stav = 5;
                  motory(4,"stredni"); //otoc se 180
                  }
                      
                      
            } else if (stav == 5){ //kdyz stav se rovna OTOCKA 180  
                
                posvaluecount = summedpos-firstposvalue;
                if (posvaluecount>2000) {
                    pocitadloSkenu=0;
                    stav = 2;
                    motorright.setPosition(0);
                    motorleft.setPosition(0);
                    motory(1, "rychla");
                    motory(3,"rychla");
                    //firstposvalue = 0;
                    }
                    
            } else if (stav == 0) { //kdyz stav se rovna nejedu
                motory(1,"rychla");
                
            
            } else if(stav==6) { //kdyz stav se rovna stredni sken
                pc.printf("%d \r\n",pocitadloSkenu);
                pc.puts("Stav je stredni sken\r\n");
                if (vzdalenost<400) { //jestli vzdalenost je mensi nez 100 nastav stav na UTOK
                    stav = 3;
                    motory(1,"rychla"); }
                ++pocitadloSkenu;
                if (pocitadloSkenu>60) {
                    stav=7;
                    pocitadloSkenu=0;
                    motory(1,"rychla");
                    motory(4, "pomala");
                    ba=0.5;
                    aa=0.5;
                    }
            } else if (stav==7){ //kdyz stav se rovna pomaly sken
                ++pocitadloSkenu;
                if (vzdalenost<600) { //jestli vzdalenost je mensi nez 100 nastav stav na UTOK
                    stav = 3;
                    motory(1,"rychla"); }
                if (pocitadloSkenu>100) {
                    stav=2;
                    pocitadloSkenu=0;
                    motory(1, "rychla");
                    motory(4,"stredni");
                    motorright.setPosition(0);
                    motorleft.setPosition(0);
                }
            } else if (stav==8) { //kdyz stav je cuknuti-dozadu
                pc.printf("cukaaaam\r\n");
                ++pocitadloCuknuti;
                if (pocitadloCuknuti>0.5){
                    pocitadloCuknuti=0;
                    cukTimer=1;
                    pocitadloSkenu=0;
                    pocitadloUtoku=11;
                    vzorkovacRychlosti=0;
                    
                    motory(1,"rychla");
                    stav = 3;
                    }
                }
                
                
            
        }else{
            motory(5, "rychla");
            }
        
         
        buttonWatch();
        
            
        
    }
}
