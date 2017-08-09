#include <stdio.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#define BUF_SIZ 266

extern char *command_str(char *c);
int main(int argc, char **argv){
if(argc==2 || argc==4 || argc > 5){
printf("%s -c (command) or -f file(write file) -m mcastAddr (Write mode)\n",argv[0]);
printf("%s -m mcastAddr (Receive mode)\n",argv[0]);
return 0;
}
if(argc==1){
printf("%s -m mcastAddr (Receive mode)\n",argv[0]);
printf("%s -c (command) or -f file(write file) -m mcastAddr (Write mode)\n",argv[0]);
return 0;
}
struct sockaddr_in src, temp;
struct in_addr mcastaddr;
int so,sc,i;
unsigned int ttl,mlen;
//const char *str1="fdakfdaj";
char message[1025];
FILE *fp;
struct ip_mreq imr;
//struct hostent *mcastad=gethostbyname("235.235.232.213");
struct hostent *mcastad=gethostbyname(argv[argc-1]);
memcpy((char *) &mcastaddr.s_addr, mcastad->h_addr_list[0],mcastad->h_length); 
//mcastaddr.sin_family=AF_INET;
//mcastaddr.sin_port=htons(3020);
so=socket(AF_INET, SOCK_DGRAM,0);
//char *co=strstr(argv[1],"-c");
//printf(co);
temp.sin_family=AF_INET;
temp.sin_addr.s_addr=htonl(INADDR_ANY);
temp.sin_port=htons(3030);

char str[BUF_SIZ];
char *str2s;
bind(so, (struct sockaddr *) &temp, sizeof(temp));

setsockopt(so,IPPROTO_IP,IP_MULTICAST_TTL, &ttl,sizeof(ttl));
if(argc>4){
if(!strcmp(argv[3],"-c")){
for(i=4;i<argc && strcmp(argv[i],"-m");i++) {str2s=strcat(str2s,argv[i]);
str2s=strcat(str2s," ");}
 sc= sendto(so,str2s,strlen(str2s)+1, 0, (struct sockaddr *) &mcastaddr, sizeof(mcastaddr));
  }
else if(!strcmp(argv[3],"-f")){
fp = fopen(argv[4],"r");
while(fgets(str2s,1024,fp))
 sendto(so,str2s,strlen(str2s)+1, 0, (struct sockaddr *) &mcastaddr, sizeof(mcastaddr));
fclose(fp);
 }
}
else {

imr.imr_multiaddr.s_addr=mcastaddr.s_addr;
imr.imr_interface.s_addr= htonl(INADDR_ANY);
setsockopt(so, IPPROTO_IP, IP_ADD_MEMBERSHIP,  &imr, sizeof(struct ip_mreq));
FILE *fn;
while(1){
mlen=sizeof(src);

recvfrom(so, message, 1024, 0, (struct sockaddr *) &src , &mlen);
if(strstr(message,"-c"))
system(command_str(message));
else if(strstr(message,"-f"))
 fn= fopen(command_str(message),"a");
else if (fn) fputs(message,fn);
//if(getchar()==EOF) 
//return setsockopt(so,IPPRTO_IP, IP_DROP_MEMBERSHIP, &imr, sizeof(struct ip_mreq));
}
fclose(fn);
setsockopt(so,IPPROTO_IP, IP_DROP_MEMBERSHIP, &imr, sizeof(struct ip_mreq));
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
