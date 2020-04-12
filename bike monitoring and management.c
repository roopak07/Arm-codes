#include "mbed.h"
#include "ESP8266.h"
Serial pc(USBTX,USBRX);
 
//LDR sensor
AnalogIn pot(PTB1);
 
//wifi UART port and baud rate
ESP8266 wifi(PTE0, PTE1, 115200);
 
//buffers for wifi library
char snd[255],resp[1000];
char http_cmd[300], comm[300];
 
int timeout = 3000; //timeout for wifi commands
 
//SSID and password for connection
#define SSID "roopak"
#define PASS "roopak07"
 
//Remote IP
#define IP "172.17.0.184"
 
//ldrvalue global variable
float ldrval = 0;
 
//Public and private keys for phant
char* Public_Key = "2JM2WlNmvLhGND6rMAEX";
char* Private_Key = "GPw0R56GBxT0GdMwR18a";
//Wifi init function
void wifi_initialize(void){
   
    pc.printf("******** Resetting wifi module ********\r\n");
    wifi.Reset();
   
    //wait for 5 seconds for response, else display no response receiveed
    if (wifi.RcvReply(resp, 5000))   
        pc.printf("%s",resp);   
    else
        pc.printf("No response");
   
    pc.printf("******** Setting Station mode of wifi with AP ********\r\n");
    wifi.SetMode(1);    // set transparent  mode
    if (wifi.RcvReply(resp, timeout))    //receive a response from ESP
        pc.printf("%s",resp);    //Print the response onscreen
    else
        pc.printf("No response while setting mode. \r\n");
       
    pc.printf("******** Joining network with SSID and PASS ********\r\n");
    wifi.Join(SSID, PASS);    
    if (wifi.RcvReply(resp, timeout))   
        pc.printf("%s",resp);  
    else
        pc.printf("No response while connecting to network \r\n");
       
    pc.printf("******** Getting IP and MAC of module ********\r\n");
    wifi.GetIP(resp);    
    if (wifi.RcvReply(resp, timeout))   
        pc.printf("%s",resp);   
    else
        pc.printf("No response while getting IP \r\n");
       
}
 
void wifi_send(void){
   
    pc.printf("******** Setting WIFI UART passthrough ********\r\n");
    wifi.setTransparent();         
    if (wifi.RcvReply(resp, timeout))   
        pc.printf("%s",resp);   
    else
        pc.printf("No response while setting wifi passthrough. \r\n");
    wait(1);
   
    pc.printf("*** Setting WiFi into MultiChannel mode ***\r\n");
    strcpy(snd,"AT+CIPMODE=0");
    wifi.SendCMD(snd);
    wifi.RcvReply(resp, timeout);
   
    
    pc.printf("******** Setting single connection mode ********\r\n");
    wifi.SetSingle();            
    wifi.RcvReply(resp, timeout);
    if (wifi.RcvReply(resp, timeout))   
        pc.printf("%s",resp);   
    else
        pc.printf("No response while setting single connection \r\n");
    wait(1);
   
    pc.printf("******** Starting TCP connection on IP and port ********\r\n");
    wifi.startTCPConn(IP, 8080);    //cipstart
    wifi.RcvReply(resp, timeout);
    if (wifi.RcvReply(resp, timeout))   
        pc.printf("%s",resp);   
    else
        pc.printf("No response while starting TCP connection \r\n");
    wait(1);
   
    //create link
    sprintf(http_cmd,"/input/%s?private_key=%s&val=%0.3f",Public_Key,Private_Key,ldrval);
    
    pc.printf("******** Sending URL to wifi ********\r\n");
    wifi.sendURL(http_cmd, comm);   //cipsend and get command
    if (wifi.RcvReply(resp, timeout))   
        pc.printf("%s",resp);   
    else
        pc.printf("No response while sending URL \r\n");
   
}
 
int main () {
   
    
    wifi_initialize();
   
    while (1) {
        ldrval = pot.read();
        pc.printf("Potentiometer ldrvalue=%.3f \r\n", ldrval);
       
        wifi_send();
        wait(3);
    }
}
