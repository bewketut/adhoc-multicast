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
#include <ctype.h>
#define BUF_SIZ 899 
#define MCASTBUF_SIZ (BUF_SIZ+2) 
#define MCASTP 40120
#define NMUTEXFILES  256
#define MCASTP_S "3100"
extern char *base256(int num,char *str);
extern int tobase10(char *str);
/*
struct srcmutexfiles {
		unsigned char *fhash;
		FILE *fname;
}; typedef struct srcmutexfiles mcastsrcfile;
*/
int main(int argc, char **argv){
struct sockaddr_in src,temp[NMUTEXFILES][NMUTEXFILES],mcast;
struct in_addr mcastaddr;
int so[NMUTEXFILES][NMUTEXFILES],sc,i,j,sock,n;
unsigned int ttl,mlen;
char message[MCASTBUF_SIZ];
unsigned char c,d;
FILE *fp;
struct ip_mreq imr;
char str[20],*str2; 
//str2=base256(65489,str);
//printf("%d\n,", tobase10(str2));

char *inetadr="235.234.232.213";
int sendflag=0;

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
char *fcomp= (char *)malloc(sizeof(char *)*70);
int fcompflag=0;
if((sock=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
src.sin_family=AF_INET;
src.sin_addr.s_addr=htonl(INADDR_ANY);
src.sin_port=htons(MCASTP);
bind(sock, (struct sockaddr *) &src, sizeof(src));
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
printf("Your channel number is %d from your username and a folder name channel%d or channel4all under this directory is to be created with read/write permission for file sharing. You can be viewed on udp://127.0.0.1:%d\n",channelname,channelname,3100+channelname);
printf("%s -c command /-F(f) file(-F write file on your channel%d -f streaming) -m mcastaddr (Write mode)\n",argv[0], channelname);
printf("%s -m mcastaddr (default using -235.234.232.213)(Receive mode)\n",argv[0]);
return 0;
}
if(argc>2 && strcmp(argv[1],"-m")){
sendlabel:
setsockopt(sock,IPPROTO_IP,IP_MULTICAST_TTL, &ttl,sizeof(ttl)); 
if(!strcmp(argv[1],"-c")){
char *command=(char *) malloc(sizeof(char)*400);
strcpy(command,argv[1]); 
strcpy(command+2,useraddr);
command= strcat(strcat(command,argv[2])," "); 
for(i=3;i<argc && strcmp(argv[i],"-m"); i++) 
command= strcat(strcat(command,argv[i])," "); 
 sc= sendto(sock,command, 400, 0, (struct sockaddr *) &mcast, sizeof(mcast));
if(sc==-1) printf("Unable to send, do group exist\n");
 }
//sock=socket(AF_INET, SOCK_DGRAM,0);
if(!strcmp(argv[1],"-F")|| !strcmp(argv[1],"-f") || 
 !strcmp(argv[1],"-cf")){
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
if(fcompflag==1)
fp=fopen(argv[2],"r");
else {fp=fopen(fcomp+6,"r");
argv[2]=fcomp+6;
}
if(!fp) {printf("%s\n","Unable to open file for reading (read permission).");
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
fseek(fp , 0 , SEEK_END); long size; 
size = ftell(fp); rewind(fp); 
long ntimes= (size/BUF_SIZ);
int rem = size%BUF_SIZ;
char rem1 =rem/256; char rem2=rem%256;

char *buffer = (char *) malloc(sizeof(char *)*(size+MCASTBUF_SIZ)); 
int numr; 
sc=sendto(sock,filename,strlen(filename)+1, 0, (struct sockaddr *) &mcast, sizeof(mcast)); 
if(sc==-1) printf("Unable to send, do group exist\n");

do {numr=fread(buffer,sizeof(char),size,fp);} while(numr!=0);
for(i=0;i< ntimes;i++){
do {numr=fread(buffer,sizeof(char),size,fp);} while(numr!=0);
  c=buffer[i*BUF_SIZ+MCASTBUF_SIZ-1];
  d=buffer[i*BUF_SIZ+MCASTBUF_SIZ-2];
buffer[i*BUF_SIZ+MCASTBUF_SIZ-1]=filehash3;
buffer[i*BUF_SIZ+MCASTBUF_SIZ-2]= userchannel;
while((n=sendto(sock,buffer+i*BUF_SIZ,MCASTBUF_SIZ, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break; 
buffer[i*BUF_SIZ+MCASTBUF_SIZ-1]=c;
buffer[i*BUF_SIZ+MCASTBUF_SIZ-2]=d;
do {numr=fread(buffer,sizeof(char),size,fp);} while(numr!=0);
} 
do {numr=fread(buffer,sizeof(char),size,fp);} while (numr!=0);
fclose(fp); if(fcompflag){strncpy(fcomp,"rm -f ",6); system(fcomp);}
 char *remn=(char *)malloc(sizeof(char)*6); remn[4]= rem1; remn[5]=rem2;
remn[0]=filehash3; remn[1]='E'; remn[2]='O'; remn[3]='L'; remn[6]=userchannel;
remn[7]='\0'; 
while((sc=sendto(sock,remn,7, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0)
if(sc!=-1) break;
buffer[i*BUF_SIZ+MCASTBUF_SIZ-1]=filehash3;
buffer[i*BUF_SIZ+MCASTBUF_SIZ-2]=userchannel;
while((n=sendto(sock,buffer+i*BUF_SIZ,MCASTBUF_SIZ, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break ; 
remn[3]='f'; //EOf 
while((n=sendto(sock,remn,7, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break; 
if(sc==-1) printf("Unable to send, do group exist\n");
}
 }
if(sendflag) goto receivelabel;
}
else {
imr.imr_multiaddr.s_addr=mcastaddr.s_addr;
imr.imr_interface.s_addr= htonl(INADDR_ANY);
i=setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,  &imr, sizeof(struct ip_mreq));
if(i < 0) {printf("Cannot join Multicast Group\n"); exit(0);}

char *argstr= argv[0];
if(strrchr(argv[0],'/'))argstr=strrchr(argv[0],'/')+1;
if(strrchr(argv[0],'.'))argstr=strrchr(argv[0],'/');
fprintf(stderr,"%s%s%s%s%s\n","Prepared to receive commands and file transfers!\n Now do ",argstr," -F filename or ",argstr," -c commandname\n on another terminal or computer on the network.\n waiting...");
FILE *fn[NMUTEXFILES][NMUTEXFILES]; 
unsigned char index=0,channel=0, prev,fflag;
 char *chead,*filen,y='x',x,*cm,*cwdir= (char *) malloc(sizeof(char)*40);
char *file_ats, *warning=(char *)malloc(sizeof(char)*85); strcpy(warning,"EEOf");
char channelfolder[40]; strcpy(channelfolder,"channel");
int nextlen[NMUTEXFILES][NMUTEXFILES],k=0,recvonly='0',count=0, files2write=0; for(i=0;i<NMUTEXFILES;i++){for(j=0; j< NMUTEXFILES; j++)nextlen[i][j]=BUF_SIZ; fn[i][j]=NULL;}
filen= (char *)malloc(sizeof(char)*20);
strcpy(cwdir,"cd "); FILE *html=NULL,*html1=NULL; 

html= fopen("index.htm","w");
fprintf(html,"<!doctype html>\
<html>\
    <head>\
        <meta charset='utf-8'>\
        <meta http-equiv='x-ua-compatible' content='ie=edge'>\
        <title></title>\
        <meta name='description' content='www'>\
<meta name='viewport' content='width=device-width, initial-scale=1.0'><script>\
function vidcload(){window.location.reload(true);}</script></head>");
fclose(html); char id[2]; id[0]='0'; id[1]='\0';
char mcastp_s[5];strcpy(mcastp_s,MCASTP_S);
char localport[8];
char vid[540]; getcwd(cwdir+4,40);
receivelabel:
if(recvonly!=2 && recvonly =='0'){
if(y!='X' && y!='S' && y!='r' && y!='V'){
fprintf(stderr,"Receive(R)/Send command(x)/Recievefor now(r/1-9/a-10p-28/)/Send file(s/v-stream)/quit(q)?\n(R/x/r[1-9a-p]/s[v]/q)");
while((x= getchar())!='\n')if(x!='r') y= toupper(x);fprintf(stderr,"\n");
if(y>='0' && y<='P'){if(y<='9') recvonly=y+1; else recvonly=  y-6;} 
  if(recvonly>'0' && recvonly<=('W')){ recvonly--;}
if(y=='R')recvonly=2;
 if (y=='S' || y=='X' || y=='V'){
 system("ls");
if(y=='S'|| y=='V')
printf("Please write a filename:");
else if(y=='X') printf("$:~"); fgets(filen,30,stdin);
strrchr(filen,'\n')[0]='\0'; 
if(y=='S')
argv[1]="-F";
else if(y=='V') argv[1]="-f";
else
 argv[1]="-c"; 
argv[2]=filen;
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
 while(1){
mlen=sizeof(src);
if(!files2write && count==1) goto receivelabel;
while((i=recvfrom(sock, message, MCASTBUF_SIZ, 0, (struct sockaddr *) &src , &mlen))!=0)
if(i!=-1) break;
if(!strncmp(message+1,"S0F!",4)||!strncmp(message+1,"S0f!",4)){ 
channel= ((unsigned char)message[5])%NMUTEXFILES;
index=((unsigned char)message[0])%NMUTEXFILES;
int channelport= ((channel-'0') > 0)?channel-'0': channel;
 snprintf(channelfolder+7,4,"%d",channelport);
strcat(channelfolder,"/");
strcat(channelfolder,message+6);
if(fn[channel][index]==NULL){
if(fopen(channelfolder,"r")){ 
if((file_ats=strrchr(channelfolder,'.'))){ file_ats[0]='\0';strcpy(filen,"_1.");strcat(filen,file_ats+1);  strcat(channelfolder,filen);}
else strcat(channelfolder,"1");
}
if(!strncmp(message+1,"S0f!",4) || !(fn[channel][index]=fopen(channelfolder,"w"))){
if((so[channel][index]=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
temp[channel][index].sin_family=AF_INET;
temp[channel][index].sin_addr.s_addr=inet_addr("127.0.0.1");
//if(files2write==0) 
//temp[channel][index].sin_port=htons(30100);
//else
temp[channel][index].sin_port=htons(3100+channelport);
}
else {
char channel4all[40]; strcpy(channel4all,"channel4all/"); strcat(channel4all,message+6);
if(fopen(channel4all,"r")){ 
if((file_ats=strrchr(channel4all,'.'))){ file_ats[0]='\0';strcpy(filen,"_1.");strcat(filen,file_ats+1);  strcat(channel4all,filen);}
else strcat(channel4all,"1");
}
if(!(fn[channel][index]=fopen(channel4all,"w")) && fopen("ttt.t","w"))
fprintf(stderr,"Please make this program root directory read only if you don't want it to be written or make a folder named channel4all (or the filesharer's channelnumber folder under it for right operations\n");
fopen(message+6,"w");
}
//printf("message+5: %s",message+5);
} 
if(prev!=index){
//if(files2write==0) 
//snprintf(localport,8, "%d",30100);
 snprintf(localport,8, "%d",3100+channelport);
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
fprintf(stderr,"opening file %s for writing %d\n",channelfolder,index);
//if(files2write>0)
//system("vlc udp://127.0.0.2:40121&");
//else if(files2write> 1) system("vlc udp://127.0.0.3:40122&");
}
else if(!strncmp(message+1,"EOL",3)){
index=((unsigned char)message[0])%NMUTEXFILES;
channel=((unsigned char)message[6])%NMUTEXFILES;
 nextlen[channel][index]=((unsigned char)message[4])*256 + ((unsigned char)message[5]) ;
}
else if(!strncmp(message+1,"EOf",3)){
index=((unsigned char)message[0])%NMUTEXFILES;
channel=((unsigned char)message[6])%NMUTEXFILES;
if(files2write && fn[channel][index]!=NULL){fclose(fn[channel][index]);
fn[channel][index]=NULL;
 } 
files2write--;
fprintf(stderr,"%s %d\n","Closed file",index);
if(message[0]=='E')printf("%s\n",message+4);
count++;
}
else if(files2write){
prev=index;
channel=((unsigned char) message[MCASTBUF_SIZ-2])%NMUTEXFILES;
index=((unsigned char) message[MCASTBUF_SIZ-1])%NMUTEXFILES;
message[MCASTBUF_SIZ-1]=0;
message[MCASTBUF_SIZ-2]=0;
if(index>0){
if(fn[channel][index])
fwrite(message,1,nextlen[channel][index],fn[channel][index]);
else 
sendto(so[channel][index],message,BUF_SIZ, 0, (struct sockaddr *) &temp[channel][index], sizeof(temp[channel][index]));
     //  write(so,message,nextlen[channel][index]); 
//else //write(so2,message,nextlen[channel][index]); 
//sendto(so2,message,BUF_SIZ, 0, (struct sockaddr *) &temp2, sizeof(temp2));
if(nextlen[channel][index]!=BUF_SIZ){
fprintf(stderr,"%s %d\n","Finished writing file", index);
//fprintf(stdout,"EOF");
nextlen[channel][index]=BUF_SIZ;}
}
else if(!strncmp(message,"-c",2)){
chead= strchr(message,'~');
chead[0]='\0';
if(!strncmp(message,"-cf",3))
printf("%s:-%s", message+3,chead+1);
else printf("%s:-%s\n", message+2,chead+1);
if(strchr(chead+1,'/')&& !strstr(chead+1,"//")){
sendto(sock,strcat(strcat(warning,"Warning error: Access is limited to the program folder ('/' forbidden)_no_command_executed!!~from~"),useraddr),125, 0, (struct sockaddr *) &mcast, sizeof(mcast)); count++;}
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
sendto(sock,strcat(strcat(warning,"Warning error: Access is limited to the program folder ('/' forbidden)_no_command_executed!!~from~"),strcat(useraddr,cwdir+4)),150, 0, (struct sockaddr *) &mcast, sizeof(mcast));//y='w'; count++;
}
else {
if(strstr(chead+1,"cd ~")||strstr(chead+1,"cd ..")) system(cwdir);
else
system(chead+1);count++; }
}
else { 
// if(strncmp(message,"-f",2) && k==0){ system("totem fd://0 &")|| system("vlc fd://0 &");k=1;}
 fwrite(message,1,nextlen[channel][index], stdout);
}
}
//fclose(fn[channel][index]); fn[channel][index]=NULL;
//if(getchar()==EOF) 
//return setsockopt(so,IPPRTO_IP, IP_DROP_MEMBERSHIP, &imr, sizeof(struct ip_mreq));

}
return 0;
}

char *base256(int num,char *str){
unsigned char rem;
int div;
int len=strlen(str);printf("%d\n",strlen(str)); str[len--]='\0';
for(div= num/256,rem=num%256,str[len--]=rem; div > 256; div=div/256,rem=div%256)
str[len--]=rem;
str[len--]=div;
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
