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
#define BUF_SIZ 899 
#define MCASTBUF_SIZ (BUF_SIZ+1) 
#define MCASTP 40120
#define NMUTEXFILES  256
extern char *base256(int num,char *str);
extern int tobase10(char *str);
/*
struct srcmutexfiles {
		unsigned char *fhash;
		FILE *fname;
}; typedef struct srcmutexfiles mcastsrcfile;
*/
int main(int argc, char **argv){
struct sockaddr_in src, temp,temp2,mcast;
struct in_addr mcastaddr;
int so,so2,sc,i,sock,n;
unsigned int ttl,mlen;
char message[MCASTBUF_SIZ];
unsigned char c,d;
FILE *fp;
struct ip_mreq imr;
char str[20],*str2; 
//str2=base256(65489,str);
//printf("%d\n,", tobase10(str2));
if(argc!=1 && argc < 3 ){
printf("%s -c[f] command [command file] or -F(f) file(write file -f on stdout) -m mcastAddr (Write mode)\n",argv[0]);
printf("%s -m mcastAddr (using -235.234.232.213)(Receive mode)\n",argv[0]);
return 0;
}
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
fp = fopen(argv[2],"r");
if(!fp) {printf("%s\n","Unable to open file for reading (read permission).");
            exit(1);}
if(!strcmp(argv[1],"-cf")){
     strcpy(message,"-cf"); strcpy(message+3,useraddr);
   int strx= strlen(useraddr)+3;
   while(fgets(message+strx,400,fp))
       while((n=sendto(sock,message,400+strx, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1)break;  
}
else{
unsigned char fileround=argv[2][0] + argv[2][strlen(argv[2])-5] - argv[2][strlen(argv[2])-3];
unsigned char filehash3= fileround%NMUTEXFILES;  
unsigned char filehash= filehash3;
char *filename= (char *) malloc(sizeof(char)*300);
if(!strcmp(argv[1],"-F")) { filename[1]='S'; filename[2]='0'; filename[3]='F';filename[4]='!';}
filename[0]=filehash; filename[5]='\0';
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
buffer[i*BUF_SIZ+MCASTBUF_SIZ-1]=filehash3;
while((n=sendto(sock,buffer+i*BUF_SIZ,MCASTBUF_SIZ, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break; 
buffer[i*BUF_SIZ+MCASTBUF_SIZ-1]=c;
do {numr=fread(buffer,sizeof(char),size,fp);} while(numr!=0);
} 
do {numr=fread(buffer,sizeof(char),size,fp);} while (numr!=0);
fclose(fp);
 char *remn=(char *)malloc(sizeof(char)*6); remn[4]= rem1; remn[5]=rem2;
remn[0]=filehash3; remn[1]='E'; remn[2]='O'; remn[3]='L'; remn[6]='\0'; 
while((sc=sendto(sock,remn,6, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0)
if(sc!=-1) break;
buffer[i*BUF_SIZ+MCASTBUF_SIZ-1]=filehash3;
while((n=sendto(sock,buffer+i*BUF_SIZ,MCASTBUF_SIZ, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break ; 
remn[3]='f'; 
while((n=sendto(sock,remn,6, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break; 
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
printf("%s%s%s%s%s\n","Prepared to receive commands and file transfers!\n Now do ",argstr," -F filename or ",argstr," -c commandname\n on another terminal or computer on the network.\n waiting...");
FILE *fn[NMUTEXFILES]; 
unsigned char index=0,prev,fflag;
 char *chead,*filen,y='X',x,*cm,*cwdir= (char *) malloc(sizeof(char)*40);
char *file_ats, *warning=(char *)malloc(sizeof(char)*85); strcpy(warning,"0EOf");
int nextlen[NMUTEXFILES],k=0,recvonly='0',count=0, files2write=0; for(i=0;i<NMUTEXFILES;i++){nextlen[i]=BUF_SIZ; fn[i]=NULL;}
filen= (char *)malloc(sizeof(char)*20);
strcpy(cwdir,"cd ");
getcwd(cwdir+4,40);
receivelabel:
if(recvonly!=('9'+1) && recvonly =='0'){
if(y!='x' && y!='r' && y!='f' && y!='w'){
printf("Receive (R)/Send command(x)/Recieve for now (1-9)/Send file(f)/quit(q)?(r/-/f/q)");
while((x= getchar())!='\n')y=x;
if(y>='0' && y<='9') recvonly=  y+1; 
  if(recvonly>'0' && recvonly<=('9'+1)){ recvonly--;}
if(y=='R')recvonly='9'+1;
 if (y=='f' || y=='x'){
 system("ls");
if(y=='f')
printf("Please write a filename:");
else if(y=='x') printf("$:~");
fgets(filen,30,stdin);
strrchr(filen,'\n')[0]='\0'; 
if(y=='f')
argv[1]="-F";
else
 argv[1]="-c"; 
argv[2]=filen;
sendflag=1;
goto sendlabel;
}}else y='l';
if(y=='q'|| y=='t') return 0;

}
if(recvonly>'0')
recvonly--; 
if((so=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
if((so2=socket(AF_INET, SOCK_DGRAM,0))<0) exit(0);
temp2.sin_family=temp.sin_family=AF_INET;
temp.sin_addr.s_addr=inet_addr("127.0.0.2");
temp2.sin_addr.s_addr=inet_addr("127.0.0.3");
temp.sin_port=htons(MCASTP+count);
temp2.sin_port=htons(MCASTP+2);
//if(count== 1) 
//concat(concat("vlc",inetadr),":40121"))
 
//else if(files2write> 0) system("vlc udp://127.0.0.3:40122&");
if(count==1) count--;
 while(1){
mlen=sizeof(src);
if(!files2write && count==1) goto receivelabel;
while((i=recvfrom(sock, message, MCASTBUF_SIZ, 0, (struct sockaddr *) &src , &mlen))!=0)
if(i!=-1) break;
if(!strncmp(message+1,"S0F!",4)){ 
index=((unsigned char)message[0])%NMUTEXFILES;
if(fn[index]==NULL){
if(fopen(message+5,"r")){ 
if((file_ats=strchr(message+5,'.'))){ file_ats[0]='\0';strcpy(filen,"_1.");strcat(filen,file_ats+1);  strcat(message+5,filen);}
else strcat(message+5,"1");
}
fn[index]= fopen(message+5,"w");
} 
files2write++;
printf("opening file %s for writing %d\n",message+5,index);
//if(files2write>0)
//system("vlc udp://127.0.0.2:40121&");
//else if(files2write> 1) system("vlc udp://127.0.0.3:40122&");
}
else if(!strncmp(message+1,"EOL",3)){
index=((unsigned char)message[0])%NMUTEXFILES;
 nextlen[index]=((unsigned char)message[4])*256 + ((unsigned char)message[5]) ;
}
else if(!strncmp(message+1,"EOf",3)){
index=((unsigned char)message[0])%NMUTEXFILES;
if(files2write && fn[index]!=NULL){fclose(fn[index]);
fn[index]=NULL;
files2write--;
printf("%s %d\n","Finished writing", index); } 
if(message[0]=='0')printf("%s\n",message+4);
count++;
}
else if(files2write){
prev=index;
index=((unsigned char) message[MCASTBUF_SIZ-1])%NMUTEXFILES;
message[MCASTBUF_SIZ-1]=0;
if(index>0){
fwrite(message,1,nextlen[index],fn[index]);
if(index==prev)
sendto(so,message,BUF_SIZ, 0, (struct sockaddr *) &temp, sizeof(temp));
     //  write(so,message,nextlen[index]); 
else //write(so2,message,nextlen[index]); 
sendto(so2,message,BUF_SIZ, 0, (struct sockaddr *) &temp2, sizeof(temp2));
if(nextlen[index]!=BUF_SIZ){
printf("%s %d\n","Finishing writing file", index);
nextlen[index]=BUF_SIZ;}
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
else  
 fwrite(message,1,nextlen[index], stdout);

}
//fclose(fn[index]); fn[index]=NULL;
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
