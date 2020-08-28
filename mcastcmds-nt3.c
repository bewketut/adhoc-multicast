#include <stdio.h> 
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include<sys/stat.h>
#include<sys/uio.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/mman.h>
#define FILE_MODE (S_IRUSR |S_IWUSR |S_IRGRP |S_IROTH)
#define BUF_SIZ 899
#define MCASTBUF_SIZ (BUF_SIZ+3) 
#define MCASTP 4010 //3100
#define NMUTEXFILES  256
extern char *base256(int num,char *str);
extern int tobase10(char *str);
extern ssize_t writen(int fd,const void *ptr, size_t n);
extern ssize_t readn(int fd, void *ptr, size_t n);
extern ssize_t wrtrdn(int fd, void *ptr, size_t n, ssize_t *fn(int,  void *, size_t
));/*
struct srcmutexfiles {
		unsigned char *fhash;
		FILE *fname;
}; typedef struct srcmutexfiles mcastsrcfile;
*/
int main(int argc, char **argv){
struct sockaddr_in src,temp[NMUTEXFILES],mcast,tmp2, *listadr;
struct in_addr mcastaddr;
int so[NMUTEXFILES][NMUTEXFILES],sc,i,sock,sock2,sock3,n;
unsigned int ttl=1, k; socklen_t j, mlen;  
char message[MCASTBUF_SIZ];
unsigned char c=0, d;
FILE *fp;
struct ip_mreq imr;
//char str[20],*str2; 
//str2=base256(65489,str);
//printf("%d\n,", tobase10(str2));

char *inetadr= (char *) malloc(sizeof(char *)*INET_ADDRSTRLEN);
strcpy(inetadr,"227.226.225.");
int sendflag=0;

for(i=1; i<argc; i++) {
if(!strcmp(argv[i],"-m")){ inetadr= argv[i+1]; break;}
if(!strcmp(argv[i],"-mn")){ c=1; strcat(inetadr, argv[i+1]); break;}
  }
if(!c) strcat(inetadr,"224");

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
  
char *fcomp= (char *)malloc(sizeof(char *)*70);
int fcompflag=0; unsigned char srcflag=0;

uid_t user;
user=getuid();
struct passwd *user_p=getpwuid(user); 
char *host= (char *)malloc(sizeof(char)*15);
gethostname(host,255);

char *useraddr= strcat(strcat(user_p->pw_name,"@"),host);
useraddr=strcat(useraddr,"~");
unsigned char fileround_userchannel=useraddr[0] + useraddr[1]-useraddr[2]+ useraddr[strlen(useraddr)-(strlen(useraddr)/2)] - useraddr[strlen(useraddr)-(strlen(useraddr)/3)] + useraddr[strlen(useraddr)- (strlen(useraddr)/4)] - useraddr[strlen(useraddr)-(strlen(useraddr)/5)] ;
unsigned char userchannel= fileround_userchannel%NMUTEXFILES;
int channelname=0;channelname= ((userchannel-'0')> 0)? userchannel-'0': userchannel;
if(argc!=1 && argc < 3 ){
printf("Your channel is %d from ur username and a folder name channel%d or channel4all in this directory be with read/write permission 4file_sharing. You can be viewed at udp://127.0.0.1:%d with a media player.\n",channelname,channelname,MCASTP+channelname);
printf("%s -c command or -F(f) files(-F write files on your channel%d folder -f streaming) -m[n] mcastaddr[or n-number] (Write mode)\n",argv[0], channelname);
printf("%s -m[n] mcastaddr[or n] (default using -227.226.225.224)(Receive mode)\n",argv[0]);
return 0;
}
 if((sock=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
 FILE *psfp= popen("ip route show | grep -o src.*192.*","r"); 
   char *addr= (char *)malloc(sizeof(char *)*(5+INET_ADDRSTRLEN)),*peern=(char *)malloc(sizeof(char *)*INET_ADDRSTRLEN),*addr2;
//addr=strcpy(addr+4,"127.0.0.1");
if(psfp && fgets(addr,INET_ADDRSTRLEN+5,psfp)!=NULL) pclose(psfp);
else {fprintf(stderr,"Connect first via wifi-direct or hotspot with the other devices. run ifconfig.\n"); exit(0);}
addr+=4;
strchr(addr,'\n')[0]='\0';
strchr(addr,' ')[0]='\0';

//char peerf=0; 
psfp= popen("ip neigh show | grep -o 192.* | grep -v FAILED | tail -1","r");
 fgets(peern,INET_ADDRSTRLEN,psfp);
if((addr2=strchr(peern,' ')))
 addr2[0]='\0';
 pclose(psfp);
if(peern[1]!='9'){ fprintf(stderr,"Connect first to the other devicesvia wifi-Direct or Wifi hotspot. The device's ip is:%s for this session. Restart this program if this error shows.\n",addr); strncpy(peern,addr,INET_ADDRSTRLEN);}
tmp2.sin_family=src.sin_family=AF_INET;//
tmp2.sin_port=src.sin_port=htons(MCASTP);
imr.imr_interface.s_addr=htonl(INADDR_ANY);
if(!strcmp(strrchr(addr,'.')+1,"1")){ //if _server x.x.x.1
imr.imr_interface.s_addr=inet_addr(addr);
}

src.sin_addr.s_addr=inet_addr(peern);


if(argc>2 && strncmp(argv[1],"-m",2)){
sendlabel:
src.sin_addr.s_addr=inet_addr(addr);
//src.sin_addr.s_addr=htonl(INADDR_ANY);//inet_addr("224.0.0.1");// mcastaddr.s_addr;

bind(sock, (struct sockaddr *) &src, sizeof(src));
/*setsockopt(sock,IPPROTO_IP,
IP_MULTICAST_LOOP, &ttl,sizeof(ttl));*/
setsockopt(sock,IPPROTO_IP,IP_MULTICAST_TTL, &ttl,sizeof(ttl));// IP_DEFAULT_MULTICAST_TTL
src.sin_addr.s_addr=inet_addr(inetadr);
sc= sendto(sock,"test", 5, 0, (struct sockaddr *) &mcast, sizeof(mcast));

if(sc==-1 && (!strcmp(strrchr(addr,'.')+1,"1"))){ //if _server x.x.x.1
fprintf(stderr,"The connection host. Close the receiving if waiting mode..(send to group or receive from group )not both\n"); exit(1); }
/*

sc= sendto(sock,"XOFMCAST", 9, 0, (struct sockaddr *) &mcast, sizeof(mcast));
psfp= popen("ip neigh show | grep -o 192.* | grep -v FAILED -c","r");
 fgets(fcomp,2,psfp); c=fcomp[0]; 
pclose(psfp);
 if(c > '1') {
strcpy(fcomp,".mcastrc"); strcat(fcomp,inetadr);
if(c=='1'){ fp=fopen(fcomp,"w");
fprintf(fp,"%s",peern); fclose(fp);}
fp=fopen(fcomp,"r"); 
if(fp)
fgets(message,1+INET_ADDRSTRLEN,fp);
c=message[0];

//src.sin_addr.s_addr=htonl(INADDR_ANY);//inet_addr("224.0.0.1");// mcastaddr.s_addr;
//i=setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,  &imr, sizeof(struct ip_mreq));
//if(i < 0) printf("Cannot join Multicast Group. Waiting in unicast.");
mlen=sizeof(src);
while(1){
while((i=recvfrom(sock, message,9, 0, (struct sockaddr *) &src, &mlen))!=0)
if(i!=-1) break;
        if(!strncmp(message,"XOFREADY",8)){
        getpeername(sock, (struct sockaddr *)&src,&mlen);
printf("%s\n","Great!! multicasting receiver that sends is found.");
setsockopt(sock,IPPROTO_IP,IP_MULTICAST_TTL, &ttl,sizeof(ttl));// IP_DEFAULT_MULTICAST_TTL
//mcast=src; srcflag=1;
break;
}
}
 }
*/




if(!strcmp(argv[1],"-c")){
char *command=(char *) malloc(sizeof(char)*MCASTBUF_SIZ);
strcpy(command,argv[1]); 
strcpy(command+2,useraddr);
command= strcat(strcat(command,argv[2])," "); 
for(i=3;i<argc && strncmp(argv[i],"-m",2); i++) 
command= strcat(strcat(command,argv[i])," "); 
//if(srcflag)
//  sc= sendto(sock,command, 401, 0, (struct sockaddr *) &tmp2, sizeof(src));
command[MCASTBUF_SIZ-3]=srcflag; 
  sc= sendto(sock,command, MCASTBUF_SIZ, 0, (struct sockaddr *) &mcast, sizeof(src));
if(sc==-1) printf("Unable to send, do group exist\n");
 }
if(!strcmp(argv[1],"-F")|| !strcmp(argv[1],"-f") || 
 !strcmp(argv[1],"-cf")){
for(j=2;j<argc && strncmp(argv[j],"-m",2); j++) {argv[2]=argv[j];
 strcpy(fcomp,"tar cvfz "); int strf=0; char *strf2=  strrchr(argv[2],'/');
if(strf2)strf=(int)strrchr(strf2,'.');
else strf= (int)strrchr(argv[2],'.');
strcat(fcomp,argv[2]); if(strf)strrchr(fcomp,'.')[0]='\0'; 
strcat(fcomp,".tgz ");
if(!strf)
system(strcat(fcomp,argv[2])); 
strcpy(fcomp+6,argv[2]);if(strf){strrchr(fcomp+6,'.')[0]='\0';fcompflag=1;}
strcat(fcomp,".tgz");
//create tar cvfz strcat(argv[2],".tgz") 
if(!(fp=fopen(argv[2],"r")))
{fp=fopen(fcomp+6,"r"); 
argv[2]=fcomp+6;
}
if(!fp) {printf("%s\n","Unable to open file for reading (read permission). is the folder location correct?");
            exit(1);}
if(!strcmp(argv[1],"-cf")){
     strcpy(message,"-cf"); strcpy(message+3,useraddr);
   int strx= strlen(useraddr)+3;
message[MCASTBUF_SIZ-3]=srcflag; 
   while(fgets(message+strx,400,fp))
       while((n=sendto(sock,message,MCASTBUF_SIZ+1, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1)break;  
}
else{

unsigned char fileround=argv[2][0] + argv[2][strlen(argv[2])-(strlen(argv[2])/2)] - argv[2][strlen(argv[2])-(strlen(argv[2])/3)];
unsigned char filehash3= fileround%NMUTEXFILES;  
unsigned char filehash= filehash3;
char *filename= (char *) malloc(sizeof(char)*(MCASTBUF_SIZ));
if(!strcmp(argv[1],"-F") || !strcmp(argv[1],"-f")) { filename[1]='S'; filename[2]='0'; filename[3]='F';filename[4]='!';if(!strcmp(argv[1],"-f")) filename[3]='f';}
filename[0]=filehash; filename[5]=userchannel; filename[6]='\0';
if(strrchr(argv[2],'/')) filename=strcat(filename,strrchr(argv[2],'/')+1);
else
filename=strcat(filename,argv[2]);//initialization 
fseek(fp , 0 , SEEK_END);
 long size; 
size = ftell(fp); rewind(fp); 
long ntimes= (size/BUF_SIZ);
int rem = size%BUF_SIZ;
char rem1 =rem/256; 
char rem2=rem%256;
char *buffer = (char *)malloc(sizeof(char *)*(size+MCASTBUF_SIZ -rem)); 
fclose(fp); 
//if(srcflag)
//sc=sendto(sock,filename,strlen(filename)+1, 0, (struct sockaddr *) &tmp2,sizeof(tmp2)); 
filename[MCASTBUF_SIZ-1]=filehash3 ; 
filename[MCASTBUF_SIZ-2]=userchannel ; 
filename[MCASTBUF_SIZ-3]=srcflag; 
sc=sendto(sock,filename,MCASTBUF_SIZ, 0, (struct sockaddr *) &mcast,sizeof(mcast)); 
if(sc==-1) {printf("Unable to send, do group exist %s\n", inet_ntoa(mcast.sin_addr));exit(0);}

int fdin;
void *srs,*dst=malloc(sizeof(void *)*2);
struct stat statbuf;
fdin=open(argv[2],O_RDONLY);
fstat(fdin,&statbuf);
//off_t pa_offset = 0 & ~(sysconf(_SC_PAGE_SIZE)-1);
srs=mmap(0,statbuf.st_size,PROT_READ,MAP_SHARED,fdin,0);

memcpy(buffer,srs,statbuf.st_size);


readn(fdin,buffer,size);
for(i=0,k=0;k< ntimes;k++,i+=BUF_SIZ){
	memcpy(dst,buffer+i+MCASTBUF_SIZ-3,3);
buffer[i+MCASTBUF_SIZ-3]=srcflag;
buffer[i+MCASTBUF_SIZ-1]=filehash3;
buffer[i+MCASTBUF_SIZ-2]=userchannel;
//if(srcflag)
//while((n=sendto(sock,buffer+i,MCASTBUF_SIZ, 0, (struct sockaddr *) &tmp2, sizeof(tmp2)))!=0) if(n!=-1) break; 
n=0; j=0;
while((n=sendto(sock,buffer+i+n,MCASTBUF_SIZ-j, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0){if(n==-1) continue; j+=n; if(j==MCASTBUF_SIZ) break; }
memcpy(buffer+i+MCASTBUF_SIZ-3,dst, 3);
//readn(fdin,buffer+i,BUF_SIZ);
//for(j=0; j<200; j++)
//do {numr=fread(buffer+i,sizeof(char),size-i,fp);} while(numr!=0);
} 

close(fdin);
 if(fcompflag){strncpy(fcomp,"rm -f ",6); system(fcomp);}
 char *remn=(char *)malloc(sizeof(char *)*MCASTBUF_SIZ); remn[4]= rem1; remn[5]=rem2;
remn[MCASTBUF_SIZ-1]=filehash;
remn[MCASTBUF_SIZ-2]=userchannel;
remn[MCASTBUF_SIZ-3]=srcflag;
remn[0]=filehash3; remn[1]='E'; remn[2]='O'; remn[3]='L'; remn[6]=userchannel;
remn[7]='\0'; 
//if(srcflag)
//while((sc=sendto(sock,remn,8, 0, (struct sockaddr *) &tmp2, sizeof(tmp2)))!=0)
//if(sc!=-1) break;

while((sc=sendto(sock,remn,MCASTBUF_SIZ, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0)
if(sc!=-1) break;
buffer[i+MCASTBUF_SIZ-3]=srcflag;
buffer[i+MCASTBUF_SIZ-2]=userchannel;
buffer[i+MCASTBUF_SIZ-1]=filehash3;
//if(srcflag)
//while((n=sendto(sock,buffer+i,MCASTBUF_SIZ, 0, (struct sockaddr *) &tmp2, sizeof(tmp2)))!=0) if(n!=-1) break ; 

n=0; j=0;
while((n=sendto(sock,buffer+i+n,MCASTBUF_SIZ-j, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0){ if(n==-1) continue; j+=n;  if(j==MCASTBUF_SIZ) break; }

}
 }
}
if(sendflag) goto receivelabel;
 }
else {
char *argstr= argv[0];
if(strrchr(argv[0],'/'))argstr=strrchr(argv[0],'/')+1;
if(strrchr(argv[0],'.'))argstr=strrchr(argv[0],'/');
fprintf(stderr,"%s%s%s%s%s\n","Prepared to receive commands and file transfers!\n Now do ",argstr," -F filename or ",argstr," -c commandname\n on another terminal or computer on the network.\n waiting...");
int  fn[NMUTEXFILES][NMUTEXFILES]; 
unsigned char findexmn=0,channel=0, prev;//fflag,k=0,*cm;
 char *chead, *filen,y='x',x,*cwdir= (char *) malloc(sizeof(char)*400);/*,*buff2=(char *)malloc(sizeof(char *)*6*BUF_SIZ)*/
char **filen1 = (char **)malloc(sizeof(char **)*500);
unsigned char reflect[NMUTEXFILES];
char *file_ats, *warning=(char *)malloc(sizeof(char)*85); strcpy(warning,"EEOf");
int sockp[2];
char channelfolder[130]; strcpy(channelfolder,"channel"); c=0;
int nextlen[NMUTEXFILES][NMUTEXFILES],recvonly='0',count=0, cnt=0, files2write=0; for(i=0;i<NMUTEXFILES;i++){
temp[i].sin_addr.s_addr=0; reflect[i]=0; for(j=0; j< NMUTEXFILES; j++){
nextlen[i][j]=BUF_SIZ; fn[i][j]=0; so[i][j]=0;
}
}
filen= (char *)malloc(sizeof(char)*20);
strcpy(cwdir,"cd "); FILE *html=NULL,*html1=NULL; 

html= fopen("index.htm","w");
if(html){
fprintf(html,"<!doctype html>\
<html>\
    <head>\
        <meta charset='utf-8'>\
        <meta http-equiv='x-ua-compatible' content='ie=edge'>\
        <title></title>\
        <meta name='description' content='www'>\
<meta name='viewport' content='width=device-width, initial-scale=1.0'><script>\
function vidcload(){window.location.reload(true);}</script></head>");
fclose(html);} char id[2]; id[0]='0'; id[1]='\0';


char localport[8];
int channelport;
char vid[540]; getcwd(cwdir+4,40);
psfp=popen("cd ~/ && pwd","r");
fgets(cwdir+4,40,psfp);
pclose(psfp);
receivelabel:

if(recvonly!=2 && recvonly =='0'){
if( y!='R'){
fprintf(stderr,"Receive(R)/Send command(s)/Recievefor now(r/1-9/a-10p-28/)/Send file(x/v-stream)/quit(q)?\n(R/s/r[1-9a-p]/x[v]/q)");
while((x= getchar())!='\n')if(x!='r') y= toupper(x);else y=x; fprintf(stderr,"\n");
if(y>='0' && y<='P'){if(y<='9') recvonly=y+1; else recvonly=  y-6;} 
  if(recvonly>'0' && recvonly<=('W')){ recvonly--;}
if(y=='R')recvonly=2;
 if (y=='S' || y=='X' || y=='V'){
if(y=='X'|| y=='V')
printf("Write a filename/s(regex accepted):");
else if(y=='S') printf("$:~");
 fgets(filen,30,stdin);
strrchr(filen,'\n')[0]='\0'; 
if(y=='X'|| y=='V') {
if(y=='V') argv[1]="-f";
else argv[1]="-F"; strcpy(fcomp,"ls "); i=0;
psfp=popen(strcat(fcomp,filen),"r");
if(!filen1[i])
filen1[i]=(char *)malloc(sizeof(char *)*130);
while(fscanf(psfp,"%s",filen1[i])!=EOF){
argv[i+2]=filen1[i];
i++;
if(!filen1[i]) 
filen1[i]=(char *)malloc(sizeof(char *)*130);
}
pclose(psfp);
argc=i+3;
}
else if(y=='S'){
 argv[1]="-c"; system(filen);  if(strstr(filen,"cd ") ){ 
if(strstr(filen,"cd ~")){ chdir(cwdir+4);if(strstr(filen,"cd ~/")) strcpy(fcomp,strstr(filen,"cd ~/")+5);  if(strchr(fcomp,' '))
strchr(fcomp,' ')[0]='\0'; if(fcomp) chdir(fcomp);
 }//fchdir(fd); }
else{
strcpy(fcomp,strstr(filen,"cd ")+3);  if(strchr(fcomp,' '))
strchr(fcomp,' ')[0]='\0'; chdir(fcomp);}
}  
argv[2]=filen; argc=3;
 }

sendflag=1;
goto sendlabel;
}}else y='l';
if(y=='Q'|| y=='T') return 0;

}
if(recvonly>'0' && recvonly!=2)
recvonly--; 
if(count==1) count--;
if(!(cnt%25)){
               if(!cnt){
                  tmp2.sin_addr.s_addr= htonl(INADDR_ANY);// inet_addr(addr); 
		if((sock2=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
             imr.imr_multiaddr.s_addr=mcastaddr.s_addr;
		bind(sock2, (struct sockaddr *) &tmp2, sizeof(tmp2));		
//printf("defaultttl፡=%d",IP_DEFAULT_MULTICAST_LOOP);
                            }
i=setsockopt(sock2, IPPROTO_IP, IP_ADD_MEMBERSHIP,  &imr, sizeof(struct ip_mreq));
if(i < 0) {printf("Cannot join Multicast Group. Waiting in unicast. is this %sself is connection server host device-switch %s.(?)\n",useraddr,addr);
}

else if(strcmp(strrchr(addr,'.')+1,"1")) { //if _no_server x.x.x.1
 sendto(sock2,"XOFREADY",9,0,(struct sockaddr *)&src,sizeof(src));
if((sock3=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
setsockopt(sock3,IPPROTO_IP,IP_MULTICAST_TTL, &ttl,sizeof(ttl));// IP_DEFAULT_MULTICAST_TTL
bind(sock3, (struct sockaddr *) &mcast, sizeof(mcast));		
sockp[0]=sock2; sockp[1]=sock3;
//pipe(sockp);
//socketpair(AF_UNIX,SOCK_STREAM,0,sockp);
srcflag= 'R'; 
}
  
}  cnt++;
mlen=sizeof(tmp2);
 while(1){
if(!files2write && count==1) goto receivelabel;
while((i=recvfrom(sock2, message, MCASTBUF_SIZ, 0, (struct sockaddr *) &tmp2 , &mlen))!= 0) if(i!=-1) break;
if(i==-1) continue;

//write(sock3,message, MCASTBUF_SIZ);
/*
if(message[MCASTBUF_SIZ-3] && srcflag){
if(message[MCASTBUF_SIZ-3]==1){
channel=message[MCASTBUF_SIZ-2];
findexmn=message[MCASTBUF_SIZ-1];
j=(channel+findexmn)%NMUTEXFILES;
reflect[j]=1;
message[MCASTBUF_SIZ-3]=2;
n=0; k=0;
while((n=sendto(sock2,message+k,i-k, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0)
{ if(n==-1) continue; k+=n; if(k==i) break;}
}

else if(reflect[j]==1 && message[MCASTBUF_SIZ-3]==2)
 continue;

}
*/ 
if(!strncmp(message+1,"S0F!",4)||!strncmp(message+1,"S0f!",4)){ 
channel= ((unsigned char)message[5])%NMUTEXFILES;
findexmn=((unsigned char)message[0])%NMUTEXFILES;
channelport= ((channel-'0') > 0)?channel-'0': channel;
 snprintf(channelfolder+7,4,"%d",channelport);
strcat(channelfolder,"/");
strcat(channelfolder,message+6);
if(fn[channel][findexmn]<1){
if(fopen(channelfolder,"r")){ 
if((file_ats=strrchr(channelfolder,'.'))){ file_ats[0]='\0';strcpy(filen,"_1.");strcat(filen,file_ats+1);  strcat(channelfolder,filen);}
else strcat(channelfolder,"1");
}
if(!strncmp(message+1,"S0f!",4)){
so[channel][findexmn]=1;
if(!temp[channel].sin_addr.s_addr){
temp[channel].sin_family=AF_INET;
temp[channel].sin_addr.s_addr=inet_addr("127.0.0.1");
temp[channel].sin_port=htons(MCASTP+channelport);}
}
else if (opendir(strrchr(channelfolder,'/')) && (fn[channel][findexmn]=open(channelfolder,O_WRONLY | O_CREAT |O_TRUNC)));
else if (opendir("channel4all/")){
char channel4all[120]; strcpy(channel4all,"channel4all/"); strcat(channel4all,message+6);
if(fopen(channel4all,"r")){ 
if((file_ats=strrchr(channel4all,'.'))){ file_ats[0]='\0';strcpy(filen,"_1.");strcat(filen,file_ats+1);  strcat(channel4all,filen);}
else strcat(channel4all,"1");
} 
fn[channel][findexmn]=open(channel4all,O_WRONLY | O_CREAT |O_TRUNC);
}
//fprintf(stderr,"Please make this progam root folder read only. Create a folder named channel4all (or the filesharer's channelfolder) under it for right operations of file sharing and multicast streaming.This option is left for network managers use!!:$\n");

else {
if(fopen(message+6,"r")){ 
if((file_ats=strrchr(message+6,'.'))){ file_ats[0]='\0';strcpy(filen,"_1.");strcat(filen,file_ats+1);  strcat(message+6,filen);}
else strcat(message+6,"1");
}
fn[channel][findexmn]=open(message+6,O_WRONLY | O_CREAT |O_TRUNC);
strrchr(channelfolder,'/')[1]='\0'; 
strcat(channelfolder,message+6);
} 

} 
if(fn[channel][findexmn])
fprintf(stderr,"opening file %s for writing %d\n",channelfolder,findexmn);
else if(so[channel][findexmn])
 fprintf(stderr,"The file %s is being streamed on udp://127.0.0.1:%d\n",channelfolder,MCASTP+channelport);
else {
so[channel][findexmn]=1;
if(!temp[channel].sin_addr.s_addr){
temp[channel].sin_family=AF_INET;
temp[channel].sin_addr.s_addr=inet_addr("127.0.0.1");
temp[channel].sin_port=htons(MCASTP+channelport);
 fprintf(stderr,"File can't be opened- ro folder. The file %s is being streamed on udp://127.0.0.1:%d\n",channelfolder,MCASTP+channelport);}
}

if(prev!=findexmn){
//if(files2write==0) 
//snprintf(localport,8, "%d",30100);
 snprintf(localport,8, "%d",MCASTP+channelport);
html1=fopen("index.htm","a");
if(html1){
strcpy(vid,"<video  style='margin-left:3%;' width='100' height='330' autoplay='' controls='' id='thevid");strcat(vid,id); strcat(vid,"'><source src='");
strcat(vid,"udp://127.0.0.1:");strcat(vid,localport);  
 strcat(vid,"'></video><button onclick='document.querySelector(\"#thevid");
strcat(vid,id); strcat(vid,"\").src=\"");
strcat(vid,"udp://127.0.0.1:");strcat(vid,localport);  strcat(vid,"\"'>Update now</button>"); 
fprintf(html1,"%s",vid);
fclose(html1);
id[0]++;
} }
files2write++;
//if(files2write>0)
//system("vlc udp://127.0.0.2:40121&");
//else if(files2write> 1) system("vlc udp://127.0.0.3:40122&");
}
else if(!strncmp(message+1,"EOL",3)){
findexmn=((unsigned char)message[0])%NMUTEXFILES;
channel=((unsigned char)message[6])%NMUTEXFILES;
 nextlen[channel][findexmn]=((unsigned char)message[4])*256 + ((unsigned char)message[5]) ;
findexmn=0; channel=0;
}
else if(files2write){
prev=findexmn;
channel=((unsigned char) message[MCASTBUF_SIZ-2])%NMUTEXFILES;
findexmn=((unsigned char) message[MCASTBUF_SIZ-1])%NMUTEXFILES;
channelport= ((channel-'0') > 0)?channel-'0': channel;
 snprintf(channelfolder+7,4,"%d",channelport);
strcat(channelfolder,"/");
message[MCASTBUF_SIZ-2]=0;
message[MCASTBUF_SIZ-1]=0;
if(findexmn>0){
if(fn[channel][findexmn]) {
writen(fn[channel][findexmn],message,nextlen[channel][findexmn]);
}
else  {
//if(k>0)
//buff2= (char *) realloc(buff2,sizeof(char*)*k*BUF_SIZ);
//if(k<6)
//memcpy(buff2+k*BUF_SIZ,message,BUF_SIZ);
//else k=0;
//printf("%s val:%d\n",buff2+k*BUF_SIZ,k);
//sendto(sock2,buff2,k*BUF_SIZ, 0, (struct sockaddr *) &temp[channel], sizeof(temp[channel]));
while((n=sendto(sock2,message/*buff2+k*BUF_SIZ*/,BUF_SIZ, 0, (struct sockaddr *) &temp[channel], sizeof(temp[channel])))!=0) if(n!=-1) break;
sleep(2);
}
if(nextlen[channel][findexmn]!=BUF_SIZ){
if(fn[channel][findexmn]){
	close(fn[channel][findexmn]);
fn[channel][findexmn]=0;
fprintf(stderr,"%s %d\n","Finished writing and just closed file ",findexmn);
 } 
else if(so[channel][findexmn]){
so[channel][findexmn]=0;
 fprintf(stderr,"Finished streaming %s%d on udp://127.0.0.1:%d\n",channelfolder,findexmn,MCASTP+channelport); }
else 
fprintf(stderr,"file can't be opened- ro folder/or is not being streamed\n");
if(reflect[j])
reflect[j]=0;
files2write--;
if(message[0]=='E')printf("%s\n",message+4);
count++;
nextlen[channel][findexmn]=BUF_SIZ;}
}
else if(!strncmp(message,"-c",2)){
chead= strchr(message,'~');
chead[0]='\0';
if(!strncmp(message,"-cf",3))
printf("%s:-%s", message+3,chead+1);
else printf("%s:-%s\n", message+2,chead+1);
if(strchr(chead+1,'/')&& !strstr(chead+1,"//")){
sendto(sock,strcat(strcat(warning,"Warning error: Access is limited to the program folder ('//' forbidden)_no_command_executed!!~from~"),useraddr),125, 0, (struct sockaddr *) &mcast, sizeof(mcast)); count++;}
else {
if(strstr(chead+1,"cd ~")) {system(chead+1); chdir(cwdir+4);}
else if(strstr(chead+1,"cd ")) {system(chead+1); chdir(strstr(chead+1,"cd ")+3);}
else
system(chead+1);}count++;
}}
else if(!strncmp(message,"XOFMCAST",8)) 
 sendto(sock2,"XOFREADY",9,0,(struct sockaddr *)&src,sizeof(src));
else if(!files2write && !strncmp(message,"-c",2)){
chead= strchr(message,'~');
chead[0]='\0';
if(!strncmp(message,"-cf",3))
printf("%s:~%s", message+3,chead+1);
else 
printf("%s:~%s\n", message+2,chead+1);
if(strchr(chead+1,'/')&& !strstr(chead+1,"//")){
sendto(sock,strcat(strcat(warning,"Warning error: Access is limited to the program folder ('//' forbidden)_no_command_executed!!~from~"),strcat(useraddr,cwdir+4)),150, 0, (struct sockaddr *) &mcast, sizeof(mcast));//y='w'; count++;
}
else {
if(strstr(chead+1,"cd ~")) {system(chead+1); chdir(cwdir+4);}
else if(strstr(chead+1,"cd ")) {system(chead+1); chdir(strstr(chead+1,"cd ")+3);}
else
system(chead+1);count++; }
}
/*

*/ 
else { 
;
}

}
//return setsockopt(so,IPPRTO_IP, IP_DROP_MEMBERSHIP, &imr, sizeof(struct ip_mreq));

}
return 0;
}
ssize_t readn(int fd, void *ptr, size_t n){
 return  wrtrdn(fd,ptr,n,read);
}
ssize_t writen(int fd,const void *ptr, size_t n){
 return  wrtrdn(fd,ptr,n,write);
}//
ssize_t wrtrdn(int fd, void *ptr, size_t n, ssize_t *fn(int,  void *, size_t
)){
	size_t nleft; ssize_t nwritten;
	nleft=n;
	while(nleft>0){
		if((nwritten=fn(fd,ptr,nleft))<0){ if(nleft==n) return -1;
		else break;}
		else if(nwritten==0) break;
	nleft-=nwritten;
	ptr+=nwritten;	} return (n-nleft);
}
char *base256(int num,char *str){
unsigned char rem;
int divn;
int len=strlen(str);printf("%d\n",strlen(str)); str[len--]='\0';
for(divn= num/256,rem=num%256,str[len--]=rem; divn > 256; divn=divn/256,rem=divn%256)
str[len--]=rem;
str[len--]=divn;
return str+len;
}
int tobase10(char *str){
int len= strlen(str);
int tot,i,j;
tot=(unsigned char) str[len-1];
printf("%s\n",str);
int base=100;
int mult=256;
for(i=0, j=len-2; j>-1 && i <len-2; i++,j--){
tot+=((unsigned char) str[j])*mult;
mult=mult*base;
base=base*100;
printf("total:%d\n",tot);
}

return tot;
}
/*

char *int2str(int k)
{ int i;
  char str[10];
      for(i=1; i<k; i*=10);

       k%i 
}


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

