#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>


void encrypt_input(char *s,int n,char *c);
void  get_enc_data(char *str,int n,char *c,int key);

int fast_exp(int a,int q,int n)
{
  long long int res=1;
  while(q>0)
  {
    if(q & 1)
      res=((res % n)*(a%n))%n;;
    a=((a%n)*(a%n))%n;
    q>>=1;
  }
  return (int)res;
}

int main(int argc,char *argv[])
{
  if(argc!=4)
  {
    printf("ERROR!! Incorrect input format.\n");
    exit(1);
  }
  int welcomeSocket, newSocket;
  FILE *fp;
  char buffer[1024];
  char str[1024];
  char c[1024];
  char *line2;
  char *ch;
  int i=0;
  int n,g,y,key;
  char *shared_keys[2];
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(atoi(argv[2]));
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  if(listen(welcomeSocket,5)==0)
    printf("Listening\n");
  else
    printf("Error\n");

  addr_size = sizeof serverStorage;
  newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
  strcpy(buffer,"Hello World\n");
  send(newSocket,buffer,13,0);
  buffer[0]='\0';
  recv(newSocket, buffer, 1024, 0);
  ch = strtok(buffer, "#");
  while (ch != NULL) {
  shared_keys[i]=ch;
  i++;
  ch = strtok(NULL, "# ");
  }
  n=atoi(shared_keys[0]);
  g=atoi(shared_keys[1]);
  y=(rand()%(n-3))+2;
  int B=y;
  y=fast_exp(g,y,n);
  buffer[0]='\0';
  sprintf(buffer,"%d",y);
  send(newSocket,buffer,13,0);
  buffer[0]='\0';
  recv(newSocket, buffer, 1024, 0);
  key=atoi(buffer);
  key=fast_exp(key,B,n);
  printf("\n key= %d\n",key);
  send(newSocket,"done",10,0);

  int readChar = 0;
  int r;
  fp=fopen(argv[3],"w");
  if(fp == NULL)
    printf("%s\n", "Error opening file");

memset(&buffer, 0, 1024);

  while(readChar = recv(newSocket,buffer,1022,0) > 0){
      memset(&c, 0, 1024);
      memset(&str, 0, 1024);
      encrypt_input(buffer,strlen(buffer),c);
      line2="";
      line2=c;
      get_enc_data(line2,strlen(line2),str,key);
      // fprintf(fp,"%s",str);
      fwrite(str, strlen(str), 1, fp);

      if(strlen(str) < 1022){
        break;
      }
      memset(&buffer, 0, 1024);
  }
  fclose(fp);
  close(newSocket); 
  printf("File received succesfully!!!\n");
  exit(1);
  
  return 0;
}

void  get_enc_data(char *str,int n,char *c,int key)
{
  char arr[69];
  arr[0]=' ';   arr[1]='A';   arr[2]='B';   arr[3]='C';   arr[4]='D';   arr[5]='E';   arr[6]='F';   arr[7]='G';   arr[8]='H';  
  arr[10]='J';  arr[11]='K';  arr[12]='L';  arr[13]='M';  arr[14]='N';  arr[15]='O';  arr[16]='P';  arr[17]='Q';  arr[18]='R';
  arr[19]='S';  arr[20]='T';  arr[21]='U';  arr[22]='V';  arr[23]='W';  arr[24]='X';  arr[25]='Y';  arr[26]='Z';  arr[27]=',';
  arr[28]='.';  arr[29]='?';  arr[30]='0';  arr[31]='1';  arr[32]='2';  arr[33]='3';  arr[34]='4';  arr[35]='5';  arr[36]='6';
  arr[37]='7';  arr[38]='8';  arr[39]='9';  arr[40]='a';  arr[41]='b';  arr[42]='c';  arr[43]='d';  arr[44]='e';  arr[45]='f';
  arr[46]='g';  arr[47]='h';  arr[48]='i';  arr[49]='j';  arr[50]='k';  arr[51]='l';  arr[52]='m';  arr[53]='n';  arr[54]='o';
  arr[55]='p';  arr[56]='q';  arr[57]='r';  arr[58]='s';  arr[59]='t';  arr[60]='u';  arr[61]='v';  arr[62]='w';  arr[63]='x';
  arr[64]='y';  arr[65]='z';  arr[66]='!';  arr[67]='\n'; arr[9]='I';
    c[0]='\0';
  char buffer[1024];
  int j=0;
  for(int i=0;i<n;)
    {
      int num=(int)str[i]-48;
      i++;
      num=(num*10+(str[i]-48));
      i++;
      num=(num-key)%68;
      if(num<0)
        num+=68;
      c[j]=arr[num];
      j++;
    }
}

void encrypt_input(char *s,int n,char *c)
{
  for(int i=0;i<n;i++)
    {
      switch(s[i])
      {
        case 'A': strcat(c,"01");
            break;
        case 'B': strcat(c,"02");
            break;
        case 'C': strcat(c,"03");
            break;
        case 'D': strcat(c,"04");
            break;
        case 'E': strcat(c,"05");
            break;
        case 'F': strcat(c,"06");
            break;
        case 'G': strcat(c,"07");
            break;
        case 'H': strcat(c,"08");
            break;
        case 'I': strcat(c,"09");
            break;
        case 'J': strcat(c,"10");
            break;
        case 'K': strcat(c,"11");
            break;
        case 'L': strcat(c,"12");
            break;
        case 'M': strcat(c,"13");
            break;
        case 'N': strcat(c,"14");
            break;
        case 'O': strcat(c,"15");
            break;
        case 'P': strcat(c,"16");
            break;
        case 'Q': strcat(c,"17");
            break;
        case 'R': strcat(c,"18");
            break;
        case 'S': strcat(c,"19");
            break;
        case 'T': strcat(c,"20");
            break;
        case 'U': strcat(c,"21");
            break;
        case 'V': strcat(c,"22");
            break;
        case 'W': strcat(c,"23");
            break;
        case 'X': strcat(c,"24");
            break;
        case 'Y': strcat(c,"25");
            break;
        case 'Z': strcat(c,"26");
            break;
        case ',': strcat(c,"27");
            break;
        case '.': strcat(c,"28");
            break;
        case '?': strcat(c,"29");
            break;
        case '0': strcat(c,"30");
            break;
        case '1': strcat(c,"31");
            break;
        case '2': strcat(c,"32");
            break;
        case '3': strcat(c,"33");
            break;
        case '4': strcat(c,"34");
            break;
        case '5': strcat(c,"35");
            break;
        case '6': strcat(c,"36");
            break;
        case '7': strcat(c,"37");
            break;
        case '8': strcat(c,"38");
            break;
        case '9': strcat(c,"39");
            break;
        case 'a': strcat(c,"40");
            break;
        case 'b': strcat(c,"41");
            break;
        case 'c': strcat(c,"42");
            break;
        case 'd': strcat(c,"43");
            break;
        case 'e': strcat(c,"44");
            break;
        case 'f': strcat(c,"45");
            break;
        case 'g': strcat(c,"46");
            break;
        case 'h': strcat(c,"47");
            break;
        case 'i': strcat(c,"48");
            break;
        case 'j': strcat(c,"49");
            break;
        case 'k': strcat(c,"50");
            break;
        case 'l': strcat(c,"51");
            break;
        case 'm': strcat(c,"52");
            break;
        case 'n': strcat(c,"53");
            break;
        case 'o': strcat(c,"54");
            break;
        case 'p': strcat(c,"55");
            break;
        case 'q': strcat(c,"56");
            break;
        case 'r': strcat(c,"57");
            break;
        case 's': strcat(c,"58");
            break;
        case 't': strcat(c,"59");
            break;
        case 'u': strcat(c,"60");
            break;
        case 'v': strcat(c,"61");
            break;
        case 'w': strcat(c,"62");
            break;
        case 'x': strcat(c,"63");
            break;
        case 'y': strcat(c,"64");
            break;
        case 'z': strcat(c,"65");
            break;
        case '!': strcat(c,"66");
            break;
        case '\n': strcat(c,"67");
            break;
        case ' ': strcat(c,"00");
            break;
        default : strcat(c,"68");
            break;
      }
    }
}