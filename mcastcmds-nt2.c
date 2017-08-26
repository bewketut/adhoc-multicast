#include <stdio.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#define BUF_SIZ 266
#define MCASTP 3020
extern char *command_str(char *c);
int main(int argc, char **argv){
if(argc!=1 && argc!=3 ){
printf("%s -c (command) or -f file(write file) -m mcastAddr (Write mode)\n",argv[0]);
printf("%s -m mcastAddr (using -235.235.232.213)(Receive mode)\n",argv[0]);
return 0;
}

struct sockaddr_in src, temp,mcast;
struct in_addr mcastaddr;
int so,sc,i,sock;
unsigned int ttl,mlen;
//const char *str1="fdakfdaj";
char message[1025];
FILE *fp;
struct ip_mreq imr;
mcastaddr.s_addr=inet_addr("235.235.232.213");
mcast.sin_family=AF_INET;
mcast.sin_addr.s_addr=inet_addr("235.235.232.213");
mcast.sin_port=htons(MCASTP);
//struct hostent *mcastad=gethostbyname(argv[argc-1]);
//memcpy((char *) &mcastaddr.s_addr, mcastad->h_addr_list[0],mcastad->h_length); 
//char *co=strstr(argv[1],"-c");
//printf(co);
char str[BUF_SIZ];
char *str2s;

  if(!IN_MULTICAST(ntohl(mcastaddr.s_addr))) {
    printf("%s : given address '%s' is not multicast\n",argv[0],
	   inet_ntoa(mcastaddr));
    exit(1);
  }


if(argc==3){
so=socket(AF_INET, SOCK_DGRAM,0);
src.sin_family=AF_INET;
src.sin_addr.s_addr=htonl(INADDR_ANY);
src.sin_port=htons(0);
bind(so, (struct sockaddr *) &src, sizeof(src));
setsockopt(so,IPPROTO_IP,IP_MULTICAST_TTL, &ttl,sizeof(ttl));
if(!strcmp(argv[1],"-c")){
int slen=strlen(argv[2])+1;
 sc= sendto(so,argv[2],slen, 0, (struct sockaddr *) &mcast, sizeof(mcast));
if(sc==-1) printf("Unable to send, do group exist\n");
 }
if(!strcmp(argv[1],"-f")){
fp = fopen(argv[2],"r");
while(fgets(str2s,100,fp))
 sc=sendto(so,str2s,strlen(str2s)+1, 0, (struct sockaddr *) &mcast, sizeof(mcast));
if(sc==-1) printf("Unable to send, do group exist\n");
fclose(fp);
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
FILE *fn;
 while(1){
mlen=sizeof(src);
i=recvfrom(sock, message, 100, 0, (struct sockaddr *) &src , &mlen);
if(i==-1) continue;
if(strstr(message,"-c"))
system(command_str(message));
else if(strstr(message,"-f"))
 fn= fopen(command_str(message),"w");
else if (fn) fputs(message,fn);
}

if(fn)
fclose(fn);

//if(getchar()==EOF) 
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
