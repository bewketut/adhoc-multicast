#include <netinet/in.h>
/*
#define CONNECTION4TWOSV 1
#if defined(IP_MULTICAST_TTL) && defined(IP_MULTICAST_LOOP) &&(defined (CONNECTEDCL) || defined(CONNECTION4TWOSV) || defined(CONNECTION4TWOCL))
#undef IP_MULTICAST_TTL
#undef IP_MULTICAST_LOOP
group init has d/t extra interface
//4server
//#ifdef
//#define IP_DEFAULT_MULTICAST_TTL 1
//#define IP_DEFAULT_MULTICAST_LOOP 1
//4client
#ifdef CONNECTEDCL
#define IP_MULTICAST_TTL 2
#define IP_MULTICAST_LOOP 2
#endif
just 4two 1toMany-Manager style-poorman version
//4server
#ifdef CONNECTION4TWOSV
#define IP_MULTICAST_TTL 0
#define IP_MULTICAST_LOOP 0
#endif
//4client
#ifdef CONNECTION4TWOCL
#define IP_DEFAULT_MULTICAST_TTL 1
#define IP_DEFAULT_MULTICAST_LOOP 0

#endif
#endif

*/
#include <stdio.h> 

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include<sys/stat.h>
#include<sys/uio.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>
#define FILE_MODE (S_IRUSR |S_IWUSR |S_IRGRP |S_IROTH)
#define BUF_SIZ 899
#define MCASTBUF_SIZ (BUF_SIZ+2) 
#define MCASTP 4010 //3100
#define NMUTEXFILES  256
#define MCASTP_S "3100"
extern char *base256(int num,char *str);
extern int tobase10(char *str);
extern ssize_t writen(FILE *fd,const void *ptr, size_t n);
/*
struct srcmutexfiles {
		unsigned char *fhash;
		FILE *fname;
}; typedef struct srcmutexfiles mcastsrcfile;
*/
int main(int argc, char **argv){
struct sockaddr_in src,temp[NMUTEXFILES][NMUTEXFILES],mcast,tmp2, *listadr;
struct in_addr mcastaddr;
int so[NMUTEXFILES][NMUTEXFILES],sc,i,j,k,sock,sock2,n;
unsigned int ttl=1; socklen_t mlen;  
char message[MCASTBUF_SIZ];
unsigned char c=0,d;
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
int fcompflag=0,srcflag=0;

if((sock=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
 FILE *psfp= popen("ip route show | grep -o src.*192.*","r"); 
   char *addr= (char *)malloc(sizeof(char *)*(6+INET_ADDRSTRLEN)),*peern=(char *)malloc(sizeof(char *)*INET_ADDRSTRLEN),*addr2;
//addr=strcpy(addr+4,"127.0.0.1");
if(psfp && fgets(addr,INET_ADDRSTRLEN+6,psfp)!=NULL) pclose(psfp);
else {fprintf(stderr,"There is no network interface up. run ifconfig.\n"); exit(0);}
addr+=4;
strchr(addr,'\n')[0]='\0';
strchr(addr,' ')[0]='\0';
char peerf=0; 
psfp= popen("ip neigh show | grep -o 192.*","r");
 fgets(peern,INET_ADDRSTRLEN,psfp);
if((addr2=strchr(peern,' ')))
 addr2[0]='\0';
 pclose(psfp);
if(!peern[0]){ fprintf(stderr,"Connect first. It is still in wait to receive or send. Your ip is:%s for this session.\n",addr); peern=addr;}
if(strcmp(strrchr(addr,'.')+1,"1")) //if server_ x.x.x.1
peerf=1;
//printf("peerf:%d peerip:%s\n",peerf,peern);
// ip route show | grep src.*
//memory mapped i/o for sending
//and writen for receiver. 
//multicast ttl for two d/t networks
src.sin_family=AF_INET;//
src.sin_port=htons(MCASTP);
src.sin_addr.s_addr=inet_addr(addr);//htonl(INADDR_ANY);
if(peerf)
src.sin_addr.s_addr=inet_addr(peern);//htonl(INADDR_ANY);
//inet_addr("192.168.43.214");
imr.imr_multiaddr.s_addr=mcastaddr.s_addr;
imr.imr_interface.s_addr=htonl(INADDR_ANY);


//const char *addr; char abuf[254];
//struct hostent *hs= gethostent();
/*
//gethostname(message,50);
struct addrinfo hint, *ailist,*aip;

//hint.ai_addr=hint.ai_next=NULL;
getaddrinfo(hs->h_name,"nfs",&hint,&ailist);
if(ailist->ai_family==AF_INET)
listadr=(struct sockaddr_in *)ailist->ai_addr;
//src.sin_addr.=listadr->sin_addr;
//memcpy((char *)abuf, hs->h_addr_list[0],hs->h_length);
//src.sin_addr.s_addr=abuf;
inet_pton(AF_INET,hs->h_addr_list[0],abuf);
src.sin_addr.s_addr=htonl(abuf) & 0xff0000ff;
//endhostent();*/			
//addr=inet_ntop(AF_INET,&src.sin_addr,abuf,50);
//src.sin_addr.s_addr=htonl(abuf);
//printf("inaddr:%s %s\n",addr,hs->h_name);

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
 

//inet_addr("192.168.43.1");// 

if(argc>2 && strncmp(argv[1],"-m",2)){
sendlabel:
for(k=0;k<1;k++){
bind(sock, (struct sockaddr *) &src, sizeof(src));
//src=src;
setsockopt(sock,IPPROTO_IP,IP_MULTICAST_TTL, &ttl,sizeof(ttl));// IP_DEFAULT_MULTICAST_TTL
setsockopt(sock,IPPROTO_IP,
IP_MULTICAST_LOOP, &ttl,sizeof(ttl));
sc= sendto(sock,"test", 5, 0, (struct sockaddr *) &mcast, sizeof(mcast));
if(sc==-1) {//printf("Unable to send to group, do group exist? sending to one device ONLY.\n");
mcast=src; srcflag=1;
 }


//IP_DEFAULT_MULTICAST_TTL
if(!strcmp(argv[1],"-c")){
char *command=(char *) malloc(sizeof(char)*400);
strcpy(command,argv[1]); 
strcpy(command+2,useraddr);
command= strcat(strcat(command,argv[2])," "); 
for(i=3;i<argc && strncmp(argv[i],"-m",2); i++) 
command= strcat(strcat(command,argv[i])," "); 

if(!srcflag)
 sc= sendto(sock,command, 400, 0, (struct sockaddr *) &src, sizeof(mcast));
 
  sc= sendto(sock,command, 400, 0, (struct sockaddr *) &mcast, sizeof(src));
if(sc==-1) printf("Unable to send, do group exist\n");
 }
//sock=socket(AF_INET, SOCK_DGRAM,0);
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
   while(fgets(message+strx,400,fp))
       while((n=sendto(sock,message,400+strx, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1)break;  
}
else{
unsigned char fileround=argv[2][0] + argv[2][strlen(argv[2])-(strlen(argv[2])/2)] - argv[2][strlen(argv[2])-(strlen(argv[2])/3)];
unsigned char filehash3= fileround%NMUTEXFILES;  
unsigned char filehash= filehash3;
char *filename= (char *) malloc(sizeof(char)*300);
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
char *buffer = (char *)malloc(sizeof(char *)*(size+MCASTBUF_SIZ)); 
int numr; 

if(!srcflag)
sc=sendto(sock,filename,strlen(filename)+1, 0, (struct sockaddr *) &src,sizeof(src));
 
sc=sendto(sock,filename,strlen(filename)+1, 0, (struct sockaddr *) &mcast,sizeof(mcast)); 
if(sc==-1) {printf("Unable to send, do group exist%s\n", inet_ntoa(mcast.sin_addr));exit(0);}
int fdin;
void *srs,*dst;
struct stat statbuf;
fdin=open(argv[2],O_RDONLY);
fstat(fdin,&statbuf);
srs=mmap(0,statbuf.st_size,PROT_READ,MAP_SHARED,fdin,0);
memcpy(buffer,srs,statbuf.st_size);
close(fdin);
for(j=0; j<230; j++)
do {numr=fread(buffer,sizeof(char),size,fp);} while(numr!=0);

for(i=0,k=0;k< ntimes;k++,i+=BUF_SIZ){
  c=buffer[i+MCASTBUF_SIZ-1];
d=buffer[i+MCASTBUF_SIZ-2];
buffer[i+MCASTBUF_SIZ-1]=filehash3;
buffer[i+MCASTBUF_SIZ-2]=userchannel;

if(!srcflag)
while((n=sendto(sock,buffer+i,MCASTBUF_SIZ+1, 0, (struct sockaddr *) &src, sizeof(src)))!=0) if(n!=-1) break; 
 
while((n=sendto(sock,buffer+i,MCASTBUF_SIZ+1, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break; 

buffer[i+MCASTBUF_SIZ-1]=c;
buffer[i+MCASTBUF_SIZ-2]=d;

for(j=0; j<230; j++)
do {numr=fread(buffer+i,sizeof(char),size-i,fp);} while(numr!=0);
} 

 if(fcompflag){strncpy(fcomp,"rm -f ",6); system(fcomp);}
 char *remn=(char *)malloc(sizeof(char)*8); remn[4]= rem1; remn[5]=rem2;
remn[0]=filehash3; remn[1]='E'; remn[2]='O'; remn[3]='L'; remn[6]=userchannel;
remn[7]='\0'; 

if(!srcflag)
while((sc=sendto(sock,remn,8, 0, (struct sockaddr *) &src, sizeof(src)))!=0)
if(sc!=-1) break;

while((sc=sendto(sock,remn,8, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0)
if(sc!=-1) break;
//memcpy(message,srs,rem);
//if(i>0) i-=BUF_SIZ;
buffer[i+MCASTBUF_SIZ-1]=filehash3;
buffer[i+MCASTBUF_SIZ-2]=userchannel;
if(!srcflag)
while((n=sendto(sock,buffer+i,MCASTBUF_SIZ+1, 0, (struct sockaddr *) &src, sizeof(src)))!=0) if(n!=-1) break ; 

while((n=sendto(sock,buffer+i,MCASTBUF_SIZ+1, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break ; 
fclose(fp);
//remn[3]='f'; //EOf 
//while((n=sendto(sock,remn,7, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break; 
//if(sc==-1) printf("Unable to send, do group exist\n");
}
 }
}}
if(sendflag) goto receivelabel;
 }
else {
char *argstr= argv[0];
if(strrchr(argv[0],'/'))argstr=strrchr(argv[0],'/')+1;
if(strrchr(argv[0],'.'))argstr=strrchr(argv[0],'/');
fprintf(stderr,"%s%s%s%s%s\n","Prepared to receive commands and file transfers!\n Now do ",argstr," -F filename or ",argstr," -c commandname\n on another terminal or computer on the network.\n waiting...");
FILE  *fn[NMUTEXFILES][NMUTEXFILES]; 
unsigned char findexmn=0,channel=0, prev;//fflag,k=0,*cm;
 char *chead,*filen,y='x',x,*cwdir= (char *) malloc(sizeof(char)*400);
char *file_ats, *warning=(char *)malloc(sizeof(char)*85); strcpy(warning,"EEOf");
char channelfolder[130]; strcpy(channelfolder,"channel");
int nextlen[NMUTEXFILES][NMUTEXFILES],recvonly='0',count=0, cnt=0, files2write=0; for(i=0;i<NMUTEXFILES;i++){for(j=0; j< NMUTEXFILES; j++)nextlen[i][j]=BUF_SIZ; fn[i][j]=NULL;}
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
//char mcastp_s[5];//strcpy(mcastp_s,MCASTP_S);
char localport[8];
int channelport;
char vid[540]; getcwd(cwdir+4,40);

receivelabel:

if(recvonly!=2 && recvonly =='0'){
if( y!='R'){
fprintf(stderr,"Receive(R)/Send command(s)/Recievefor now(r/1-9/a-10p-28/)/Send file(x/v-stream)/quit(q)?\n(R/s/r[1-9a-p]/x[v]/q)");
while((x= getchar())!='\n')if(x!='r') y= toupper(x);else y=x; fprintf(stderr,"\n");
if(y>='0' && y<='P'){if(y<='9') recvonly=y+1; else recvonly=  y-6;} 
  if(recvonly>'0' && recvonly<=('W')){ recvonly--;}
if(y=='R')recvonly=2;
 if (y=='S' || y=='X' || y=='V'){
 system("ls");
if(y=='X'|| y=='V')
printf("Please write a filename:");
else if(y=='S') printf("$:~");
 fgets(filen,30,stdin);
strrchr(filen,'\n')[0]='\0'; 
if(y=='X')
argv[1]="-F";
else if(y=='V') argv[1]="-f";
else if(y=='S')
 argv[1]="-c"; 
argv[2]=filen; argc=3;
sendflag=1;
goto sendlabel;
}}else y='l';
if(y=='Q'|| y=='T') return 0;

}
if(recvonly>'0' && recvonly!=2)
recvonly--; 
//if((so2=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
//if(count== 1) 
//concat(concat("vlc",inetadr),":40121"))
 
//else if(files2write> 0) system("vlc udp://127.0.0.3:40122&");
if(count==1) count--;
if(!(cnt%25)){
	if(!srcflag){
               if(!cnt){
		if((sock2=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
			src.sin_addr.s_addr=htonl(INADDR_ANY);//inet_addr(addr);//
                  
		bind(sock2, (struct sockaddr *) &src, sizeof(src));		
//printf("defaultttl፡=%d",IP_DEFAULT_MULTICAST_LOOP);
//inet_addr("192.168.43.1");// 
                            }
i=setsockopt(sock2, IPPROTO_IP, IP_ADD_MEMBERSHIP,  &imr, sizeof(struct ip_mreq));
if(i < 0) {printf("Cannot join Multicast Group. Waiting in unicast. is this %sself is connection server host device-switch %s.(?)\n",useraddr,addr);
//	src.sin_addr.s_addr=inet_addr("192.168.43.1");//htonl(INADDR_ANY);
//		bind(sock2, (struct sockaddr *) &src, sizeof(src));
	//	sock2=sock;
}}}  cnt++;
//printf("srcaddr:%s\n",htonl(src.sin_addr.s_addr));
mlen=sizeof(src);
 while(1){
if(!files2write && count==1) goto receivelabel;
while((i=recvfrom(sock2, message, MCASTBUF_SIZ+1, 0, (struct sockaddr *) &src , &mlen))!=0)
if(i!=-1) break;
if(!strncmp(message+1,"S0F!",4)||!strncmp(message+1,"S0f!",4)){ 
channel= ((unsigned char)message[5])%NMUTEXFILES;
findexmn=((unsigned char)message[0])%NMUTEXFILES;
channelport= ((channel-'0') > 0)?channel-'0': channel;
 snprintf(channelfolder+7,4,"%d",channelport);
strcat(channelfolder,"/");
strcat(channelfolder,message+6);
if(fn[channel][findexmn]==NULL){
if(fopen(channelfolder,"r")){ 
if((file_ats=strrchr(channelfolder,'.'))){ file_ats[0]='\0';strcpy(filen,"_1.");strcat(filen,file_ats+1);  strcat(channelfolder,filen);}
else strcat(channelfolder,"1");
}
if(!strncmp(message+1,"S0f!",4)){
if((so[channel][findexmn]=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
temp[channel][findexmn].sin_family=AF_INET;
temp[channel][findexmn].sin_addr.s_addr=inet_addr("127.0.0.1");
//temp[channel][findexmn].sin_port=htons(30100);
//else
temp[channel][findexmn].sin_port=htons(MCASTP+channelport);
}
else if (!(fn[channel][findexmn]=fopen(channelfolder,"w"))){
char channel4all[120]; strcpy(channel4all,"channel4all/"); strcat(channel4all,message+6);
if(fopen(channel4all,"r")){ 
if((file_ats=strrchr(channel4all,'.'))){ file_ats[0]='\0';strcpy(filen,"_1.");strcat(filen,file_ats+1);  strcat(channel4all,filen);}
else strcat(channel4all,"1");
}
if(!(fn[channel][findexmn]=fopen(channel4all,"w")) && fopen("ttt.t","w")){
//fprintf(stderr,"Please make this progam root folder read only. Create a folder named channel4all (or the filesharer's channelfolder) under it for right operations of file sharing and multicast streaming.This option is left for network managers use!!:$\n");

if(fopen(message+6,"r")){ 
if((file_ats=strrchr(message+6,'.'))){ file_ats[0]='\0';strcpy(filen,"_1.");strcat(filen,file_ats+1);  strcat(message+6,filen);}
else strcat(message+6,"1");
}
fn[channel][findexmn]=fopen(message+6,"w");
strrchr(channelfolder,'/')[1]='\0'; 
strcat(channelfolder,message+6);
} 
}
//printf("message+5: %s",message+5);
} 
if(fn[channel][findexmn])
fprintf(stderr,"opening file %s for writing %d\n",channelfolder,findexmn);
else if(so[channel][findexmn])
 fprintf(stderr,"The file %s is being streamed on udp://127.0.0.1:%d\n",channelfolder,MCASTP+channelport);
else if((so[channel][findexmn]=socket(AF_INET, SOCK_DGRAM,0))>0){
temp[channel][findexmn].sin_family=AF_INET;
temp[channel][findexmn].sin_addr.s_addr=inet_addr("127.0.0.1");
//temp[channel][findexmn].sin_port=htons(30100);
//else
temp[channel][findexmn].sin_port=htons(MCASTP+channelport);
 fprintf(stderr,"File can't be opened- ro folder. The file %s is being streamed on udp://127.0.0.1:%d\n",channelfolder,MCASTP+channelport);}
else 
fprintf(stderr,"file can't be opened- ro folder/is not being streamed\n");

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
}
else if(files2write){
prev=findexmn;
channel=((unsigned char) message[MCASTBUF_SIZ-2])%NMUTEXFILES;
findexmn=((unsigned char) message[MCASTBUF_SIZ-1])%NMUTEXFILES;
channelport= ((channel-'0') > 0)?channel-'0': channel;
 snprintf(channelfolder+7,4,"%d",channelport);
strcat(channelfolder,"/");
message[MCASTBUF_SIZ-2]=0;
if(findexmn>0){
if(fn[channel][findexmn])
writen(fn[channel][findexmn],message,nextlen[channel][findexmn]);
//fwrite(message,1,nextlen[channel][findexmn],fn[channel][findexmn]);
else 
sendto(so[channel][findexmn],message,BUF_SIZ+1, 0, (struct sockaddr *) &temp[channel][findexmn], sizeof(temp[channel][findexmn]));
     //  write(so,message,nextlen[channel][findexmn]); 
//else //write(so2,message,nextlen[channel][findexmn]); 
//sendto(so2,message,BUF_SIZ, 0, (struct sockaddr *) &temp2, sizeof(temp2));
if(nextlen[channel][findexmn]!=BUF_SIZ){
if(fn[channel][findexmn]){
	fclose(fn[channel][findexmn]);
fn[channel][findexmn]=NULL;
fprintf(stderr,"%s %d\n","Finished writing and just closed file ",findexmn);
 } 
else if(so[channel][findexmn])
 fprintf(stderr,"Finished streaming %s%d on udp://127.0.0.1:%d\n",channelfolder,findexmn,MCASTP+channelport);
else 
fprintf(stderr,"file can't be opened- ro folder/or is not being streamed\n");
files2write--;
if(message[0]=='E')printf("%s\n",message+4);
count++;
//fprintf(stdout,"EOF");
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
if(strstr(chead+1,"cd ~")||strstr(chead+1,"cd ..")) system(cwdir);
else
system(chead+1);}count++;
}}
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
if(strstr(chead+1,"cd ~")||strstr(chead+1,"cd ..")) system(cwdir);
else
system(chead+1);count++; }
}
else { 
// if(strncmp(message,"-f",2) && k==0){ system("totem fd://0 &")|| system("vlc fd://0 &");k=1;}
 ;//fwrite(message,1,nextlen[channel][findexmn], stdout);
}
}
//fclose(fn[channel][findexmn]); fn[channel][findexmn]=NULL;
//if(getchar()==EOF) 
//return setsockopt(so,IPPRTO_IP, IP_DROP_MEMBERSHIP, &imr, sizeof(struct ip_mreq));

}
return 0;
}
ssize_t writen(FILE *fd,const void *ptr, size_t n){
	size_t nleft; ssize_t nwritten;
	nleft=n;
	while(nleft>0){
		if((nwritten=fwrite(ptr,1,nleft,fd))<0){ if(nleft==n) return -1;
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
