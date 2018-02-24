#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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
int get_factors(int *arr, int n)
{
int flag=0;
int c=0;
  while (n%2 == 0)
  {
    flag=1;
    n = n/2;
  }
  if(flag==1)
   { arr[c++]=2;}

  for (int i = 3; i <= sqrt(n); i = i+2)
  {
    flag=0;
    while (n%i == 0)
    {
      flag=1;
      n = n/i;
    }
    if(flag==1)
      arr[c++]=i;
  }
  if (n > 2)
    arr[c++]=n;
  return c;
}

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


int rabin_miller(int a,int k,int q,int n)
{
  long long int cong_val=fast_exp(a,q,n);
  if(cong_val%n==1)
    return 1;
  for(int j=1;j<k;j++)
  {
    long long int b=1<<j;
    b=b*q;
    cong_val=fast_exp(a,b,n);
    if(cong_val%n==((n-1)%n))
      return 1;
  }
  return 0;
}


int check_prime(int num)
{
  int q=num-1;
  int k=0;
  while(q%2==0)
  {
    q/=2;
    k++;
  }
  int a=(rand()%(num-3))+2;
  int conclusion=rabin_miller(a,k,q,num);
  return conclusion;  
}


int main(int argc,char *argv[]){
 
 if(argc!=4)
 {
  printf("ERROR!! Incorrect input format.\n");
  exit(1);
 }
  FILE *fp;
  char c[1024];
  size_t len = 0;
  int read;
  char line[1024],line2[1024];
  int clientSocket;
  char g_in_str[20];
  char x_in_str[20];
  int  x,key;
  char *file_name;
  char random_num_str[20];
  char buffer[1024];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(atoi(argv[2]));
  serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  addr_size = sizeof(serverAddr);
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  recv(clientSocket, buffer, 1024, 0);

  srand(time(0));
  int random_num;
  int got_prime=0;



  /**********prime number begin************/
  while(got_prime!=1)
  {
    random_num=rand()%10001;
    if(random_num%2==0 || random_num<4)
      continue; 
    got_prime=check_prime(random_num);
  }
  /**********prime number end************/



  /*********primitive root begin***********/
    int g;///primitive root
    int primitive_root[random_num];
    int max_num=random_num-1;
    int size=get_factors(primitive_root,max_num);
    for (int r=2; r<=max_num; r++)
    {
    int flag=0;
    for(int arr_index=0; arr_index<size; arr_index++)
    {
    if (fast_exp(r,max_num/primitive_root[arr_index],random_num) == 1)
      {
        flag=1;
        break;
      }
    }
    if (flag==0)
   { g=r;break;}
    }
  /*********primitive root end***********/


  /***********wired protocol**************/
  sprintf(random_num_str,"%d",random_num);
  strcat(random_num_str,"#");
  sprintf(g_in_str,"%d",g);
  strcat(random_num_str,g_in_str);

  /*********sending g and n to server********/
  send(clientSocket,random_num_str,sizeof(random_num_str),0);


  x=(rand()%(random_num-3))+2;
  int A=x;
  x=fast_exp(g,x,random_num);
  buffer[0]='\0';

  /*********receiving server side equation**********/
  recv(clientSocket,buffer, 1024, 0);
  key=atoi(buffer);
  sprintf(x_in_str,"%d",x);

  /*********sending cliend sided equation**********/
  send(clientSocket,x_in_str,sizeof(x_in_str),0);

  /******calculating symmetric key*************/
  key=fast_exp(key,A,random_num);

  printf("key= %d\n",key);
  memset(&buffer, 0, 1024);

  recv(clientSocket, buffer, 1024, 0);
  fp=fopen(argv[3],"r");
  FILE *fd_encoded=fopen("enc_file","w");
  if(fp==NULL)
    printf("ERROR!FILE DOES NOT EXIST");
  else
  {
    while ((read = fread(line, 1, 1022, fp)) > 0)
    {
      char str[1024];
      //char c[1024];
      memset(&str, 0, 1024);
      memset(&c, 0, 1024);
      encrypt_input(line,strlen(line),c);
      // line2="";
      memset(&line2, 0, 1024);
      strcpy(line2, c);
      get_enc_data(line2,strlen(line2),str,key);
      fwrite(str, strlen(str), 1, fd_encoded);

      memset(&line, 0, 1024);
      }
      fclose(fp);
      fclose(fd_encoded);


    fd_encoded=fopen("enc_file","r");
    if(fd_encoded==NULL)
      printf("ERROR!FILE DOES NOT EXIST");
    else
      {
        while ((read = fread(line, 1, 1022, fd_encoded)) > 0)
        {
          send(clientSocket,line,read,0); 
          memset(&line,0,1024);

        }
      }
      fclose(fd_encoded);
      printf("File sent successfully!!!!\n");   
  }
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
      c[j]=arr[(num+key)%68];
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