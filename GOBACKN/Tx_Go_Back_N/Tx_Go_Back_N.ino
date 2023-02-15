#include <RF24Network.h>          //Libraries
#include <RF24.h>
#include <SPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
RF24 radio (9,10);                 //Select pins for connection

RF24Network network(radio);

const uint16_t this_node = 01;      //Select number of nodes
const uint16_t other_node = 00;

typedef struct Nodo
{
    char dato[50];
    struct Nodo *siguiente;
} Nodo;

Nodo * crearNodo(char *d )
{
    Nodo *nuevo;
    nuevo = (Nodo*) malloc(sizeof(Nodo));
   strcpy(nuevo -> dato,d);
    nuevo -> siguiente=NULL;
    return nuevo;
}

Nodo * insertar(Nodo *cima,char *d)
{
    Nodo *nuevo;
    nuevo=crearNodo(d);
 Nodo * aux=cima;
    if(cima==NULL){
    return nuevo;

    }else{
      while(aux->siguiente!=NULL){

        aux=aux->siguiente;


      }
      aux->siguiente=nuevo;
      return cima;
    }
}

void verFila(Nodo *cima)
{
    Nodo *aux;
    aux = cima;

   if(aux==NULL)
    {
        Serial.println("La Fila esta vacia");
        return 1;
    }

    while(aux!=NULL)
    {

        Serial.println("Fila");
        Serial.println(aux->dato);
        aux=aux->siguiente;
    }
    return 0;
}



void setup() {                      //Connection settings
  Serial.begin(9600);
  SPI.begin();
  radio.begin();
radio.setDataRate(RF24_1MBPS);
radio.setPALevel(RF24_PA_MAX) ;
  network.begin(90,this_node);
  delay(5000);
  //Serial.println("Base station OK");
  
}


char *MakeCRC(char *BitString)
   {
   static char Res[6];                                 // CRC Result
   char CRC[5];
   int  i;
   char DoInvert;
   
   for (i=0; i<5; ++i)  CRC[i] = 0;                    // Init before calculation
   
   for (i=0; i<strlen(BitString); ++i)
      {
      DoInvert = ('1'==BitString[i]) ^ CRC[4];         // XOR required

      CRC[4] = CRC[3];
      CRC[3] = CRC[2];
      CRC[2] = CRC[1] ^ DoInvert;
      CRC[1] = CRC[0];
      CRC[0] = DoInvert;
      }
      
   for (i=0; i<5; ++i)  Res[4-i] = CRC[i] ? '1' : '0'; // Convert binary to ASCII
   Res[5] = 0;                                         // Set string terminator

   return(Res);
   }

    void Enviar(char *msj){
    char mensaje[15];
    strcpy(mensaje,msj);
 bool okComandoEnviado=false;
  network.update();
  RF24NetworkHeader headerMensaje(other_node);
   okComandoEnviado = network.write(headerMensaje,&mensaje,sizeof(mensaje));

  if(okComandoEnviado){

    //Serial.println("Comand sent");
   // Serial.print("El mensaje enviado es:");
    //Serial.println(mensaje);
    }else{
     // Serial.println("Comand NOT sent");
         //Serial.print("El mensaje NO enviado es:");
    //Serial.println(mensaje);
      }

  
  }

  int recibir(){

  
    char msjrecibido[15]="";
  bool ok2=false;
  int numerorecibido;  

    //Serial.print("Mensajes totales recibidos :");

 network.update();
 while(network.available()){
  if(network.available() > 0){
    RF24NetworkHeader headermsjrecibido;
    ok2=network.read(headermsjrecibido,&numerorecibido,sizeof(numerorecibido));

    }
  }
   
if(ok2){

  Serial.print("Mensaje recibido : ");
Serial.println(numerorecibido);
return numerorecibido;
    
  }else{

    Serial.println("Mensaje no recibido ");
    return 0;
    }


  
  }

  char *stbit(char *binario){
        char mensaje[15];
    strcpy(mensaje,binario);
    int dim=strlen(mensaje); 
     static char tam[50];
     char *Data;
     int n=0;
     for(byte j=0;j<dim;j++){                //Read the message bit by bit to write it to a string of characters
    for(int i=7;i>=0;i--){
      tam[n]=bitRead(mensaje[j],i)+'0';  
      n++;
     
        }
      }
      tam[n]='\0';
     Data=tam;  
    return tam;
    }

char *proces(char *mensaje){
    char binario[10];
     strcpy(mensaje,binario);
 
  
    char  *Result;
    char *Data;
     Data=stbit(binario);                                
   Result = MakeCRC(Data);                     //Calculate CRC-5-USB               

    //Serial.print("Mensaje:");
  //Serial.println(Data); 
  //Serial.print("CRC:");
 // Serial.println(Result);
  strcat(binario,Result);

//Serial.print("Mensaje a enviar:");
  //Serial.println(binario);
  return binario;
  }
   

void loop() {
  char binario[10]={"Hola"};
 
  
    char  *Result;
    char *Data;
     Data=stbit(binario);                                
   Result = MakeCRC(Data);                     //Calculate CRC-5-USB               

   // Serial.print("Mensaje:");
  //Serial.println(Data); 
  //Serial.print("CRC:");
  //Serial.println(Result);
  strcat(binario,Result);

//Serial.print("Mensaje a enviar Final:");
  //Serial.println(binario);
  

Nodo *Cima;
Cima=NULL;

Cima=insertar(Cima,binario);
Cima=insertar(Cima,binario);
Cima=insertar(Cima,binario);
Cima=insertar(Cima,binario);
Cima=insertar(Cima,binario);
Cima=insertar(Cima,binario);

//Serial.print("Fila:");
//verFila(Cima);

int j=1;
int ack=0;
int ack2=0;
 Nodo *aux;
    aux = Cima;
 
    while(aux!=NULL)
    {
   
      if(aux->siguiente==NULL){
       // Serial.println("Ultimo paquete");
        Enviar(aux->dato);
        delay(500);
       ack=recibir();
        delay(500); 
            if(ack==1){
 
              aux=aux->siguiente;
              //Serial.print("mov ventana:");
              //Serial.println(j);
              j++;
       
              }
        }else{
             Enviar(aux->dato);
        delay(500); 
      Enviar(aux->siguiente->dato);
        
        delay(500); 
         ack=recibir();
    delay(500); 
    ack2=recibir();
    delay(500); 
    if(ack==1){
 
              aux=aux->siguiente;
              //Serial.print("mov ventana:");
              //Serial.println(j);
              j++;
      
              if(ack2==1){
        
                aux=aux->siguiente;
              // Serial.print("mov ventana:");
              //Serial.println(j);
              j++;
  
                        }   
              }
        }    
    }
  
}
