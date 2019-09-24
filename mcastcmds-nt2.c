#include <stdio.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
//#define BUF_SIZ 266 
#define BUF_SIZ 4096 
#define MCASTP 3020
extern char *command_str(char *c);
/*
struct srcmutexfiles {
		unsigned char *fhash;
		FILE *fname;
}; typedef struct srcmutexfiles mcastsrcfile;
*/
int main(int argc, char **argv){
struct sockaddr_in src, temp,mcast;
struct in_addr mcastaddr;
int so,sc,i,sock,n;
unsigned int ttl,mlen;
//const char *str1="fdakfdaj";
char message[BUF_SIZ];
FILE *fp;
struct ip_mreq imr;
//for (i=0; i<argc; i++) printf("%s", argv[2]);
if(argc!=1 && argc < 3 ){
printf("%s -c[f] command [command file]  or -F(f) file(write file -f on stdout) -m mcastAddr (Write mode)\n",argv[0]);
printf("%s -m mcastAddr (using -235.235.232.213)(Receive mode)\n",argv[0]);
return 0;
}
char *inetadr="235.235.232.213";
for(i=1; i<argc; i++) 
if(!strcmp(argv[i],"-m")){ inetadr= argv[i+1]; break;}
mcastaddr.s_addr=inet_addr(inetadr);
mcast.sin_family=AF_INET;
mcast.sin_addr.s_addr=inet_addr(inetadr);
mcast.sin_port=htons(MCASTP);
//struct hostent *mcastad=gethostbyname(argv[argc-1]);
//memcpy((char *) &mcastaddr.s_addr, mcastad->h_addr_list[0],mcastad->h_length); 
//char *co=strstr(argv[1],"-c");
//printf(co);

  if(!IN_MULTICAST(ntohl(mcastaddr.s_addr))) {
   printf("%s : given address '%s' is not multicast\n",argv[0],
	   inet_ntoa(mcastaddr));
    exit(1);
  }
if(argc>2 && strcmp(argv[1],"-m")){
so=socket(AF_INET, SOCK_DGRAM,0);
src.sin_family=AF_INET;
src.sin_addr.s_addr=htonl(INADDR_ANY);
src.sin_port=htons(0);
bind(so, (struct sockaddr *) &src, sizeof(src));
setsockopt(so,IPPROTO_IP,IP_MULTICAST_TTL, &ttl,sizeof(ttl));
if(!strcmp(argv[1],"-c")){
char *command=argv[1]; 
for(i=2;i<argc && strcmp(argv[i],"-m"); i++) 
command= strcat(strcat(command,argv[i])," "); 
 sc= sendto(so,command, strlen(command)+1, 0, (struct sockaddr *) &mcast, sizeof(mcast));
if(sc==-1) printf("Unable to send, do group exist\n");
//printf("%s%d\n",command, sc);
 }
if(!strcmp(argv[1],"-F")|| !strcmp(argv[1],"-f") || 
 !strcmp(argv[1],"-cf")){
if(argc>3) {printf("Put a file name ONLY\n"); return 0;}
fp = fopen(argv[2],"rb");

if(!strcmp(argv[1],"-cf")){
   message[0]='-';message[1]='c'; message[2]='f';
   while(fgets(message+3,50,fp))
       //sc=sendto(so,message,53, 0, (struct sockaddr *) &mcast, sizeof(mcast));
       while((n=sendto(so,message,53, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1)break;  
}
else{
char filehash3= (char) rand();
char filehash= filehash3;
char *filename= (char *) malloc(sizeof(char)*300);
filename[0]=filehash;filename[2]='\0';
filename=strcat(strcat(filename,argv[1]),argv[2]);//initialization 
fseek(fp , 0 , SEEK_END); long size; 
size = ftell(fp); rewind(fp); 
long ntimes= (size/BUF_SIZ);
int rem = size%BUF_SIZ;
char *buffer = (char *) malloc(sizeof(char *)*size); 
//char buffer2[BUF_SIZ];
//int j=0;
int numr; 
//char *filestart=(char *)malloc(sizeof(char)*(ntimes+1));
//for(i=0;i<ntimes+1;i++)filestart[i]=filehash3; 
n = 0;
sc=sendto(so,filename,strlen(filename)+1, 0, (struct sockaddr *) &mcast, sizeof(mcast)); 
if(sc==-1) printf("Unable to send, do group exist\n");
numr=fread(buffer,sizeof(char),size,fp);
fclose(fp);

for(i=0;i< ntimes; i++){
//strncpy(buffer2,buffer+i*BUF_SIZ,BUF_SIZ);
while((n=sendto(so,buffer+i*BUF_SIZ,BUF_SIZ, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break; 
}

 char *remn=(char *)malloc(sizeof(char)*6); remn[4]=(unsigned char) rem;
remn[0]=filehash3; remn[1]='E'; remn[2]='O'; remn[3]='F'; remn[5]='\0'; 
printf("remchar:%d",(unsigned char)remn[4]*8);
 sc=sendto(so,remn,6, 0, (struct sockaddr *) &mcast, sizeof(mcast));
while((n=sendto(so,buffer+ i*BUF_SIZ,rem, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break; 

 
 sc=sendto(so,"EOF",strlen("EOF")+1, 0, (struct sockaddr *) &mcast, sizeof(mcast));
if(sc==-1) printf("Unable to send, do group exist\n");
//if(sc==-1) printf("Unable to send, do group exist\n");
//while((n=sendto(so,buffer+n,numr-n, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=-1);  //for video maybe
}
 }
}
else {
temp.sin_family=AF_INET;
temp.sin_addr.s_addr=htonl(INADDR_ANY);
temp.sin_port=htons(MCASTP);
if((sock=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
bind(sock, (struct sockaddr *) &temp, sizeof(temp));
imr.imr_multiaddr.s_addr=mcastaddr.s_addr;
imr.imr_interface.s_addr= htonl(INADDR_ANY);
i=setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,  &imr, sizeof(struct ip_mreq));
if(i < 0) {printf("Cannot join Multicast Group\n"); exit(0);}
//mcastsrcfile *mcastfiles= (mcastsrcfile *) malloc(sizeof(mcastsrcfile)*250); //250 mutex file
FILE *fn[250];
//mcastfiles[0]->fhash='0';
//mcastfiles[0]->fname=fopen("1.txt","r");
int index=0;
int nextlen[250]; for(i=0;i<250;i++)nextlen[i]=BUF_SIZ;
 while(1){
mlen=sizeof(src);
i=recvfrom(sock, message, nextlen[index]+1, 0, (struct sockaddr *) &src , &mlen);
if(i==-1) continue;
//printf("nextlen[index]:%d,index:%d\n",nextlen[index],index);
//printf("%s\n",message);
if(!fn[index] && strstr(message,"-c")){
if(strstr(message,"-cf")){
printf("%s\n", &message[3]);
system(&message[3]);
}
else {
printf("%s\n", &message[2]);
system(&message[2]);
}
}
else if(!fn[index] && strstr(message,"-F"))  { 
 index=((unsigned int)message[0])%250;
fn[index]= fopen(message+3,"w");
printf("opening file %s for writing\n",message+3);
}
else if(strstr(message,"EOF")!=NULL){
 index=((unsigned int)message[0])%250;
 nextlen[index]=((unsigned char)message[4])*8 ;
}
else if((nextlen[index]!=BUF_SIZ) && fn[index]){
fwrite(message,1,nextlen[index],fn[index]);
printf("%s\n","Finishing writing file");
 fclose(fn[index]); fn[index]=NULL;}
else if(fn[index]){
fwrite(message,1,nextlen[index],fn[index]);
}
else fwrite(message,1,nextlen[index], stdout);

}//if(getchar()==EOF) 
//return setsockopt(so,IPPRTO_IP, IP_DROP_MEMBERSHIP, &imr, sizeof(struct ip_mreq));

}
return 0;
}
char *command_str(char *c){
int i=0;
for(i=2;i<strlen(c);i++)
c[i-2]=c[i];
c[i-2]='\0';
return c;
}
/*
char *int2str(int k)
{ int i;
  char str[10];
      for(i=1; i<k; i*=10);

       k%i 
}
*/
/*
static struct option long_opt[] = {
    {"cmd", 1, 0, 'c'},
    {"file", 1, 0, 'f'},
    {"maddr", 1, 0, 'm'},
    {NULL, 0, NULL, 0}
};
static char *short_opt = "c:f:m:";
char *myargs(int argc, char **argv){
    while (1) {
	c = getopt_long(argc, argv, short_opt, long_opt, NULL);
	if (c == -1)
	    break;
         
         else return optarg;
}
*/
