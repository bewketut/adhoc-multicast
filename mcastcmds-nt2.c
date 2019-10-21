#include <stdio.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
//#define BUF_SIZ 266 
#define BUF_SIZ  832 
#define MCASTP 3020
extern char *command_str(char *c);
extern unsigned char calcfhash(char *m2, char m1, char *m, unsigned char hashes[], int arraysize);
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
unsigned char c,d;
FILE *fp;
struct ip_mreq imr;
//for (i=0; i<argc; i++) printf("%s", argv[2]);
if(argc!=1 && argc < 3 ){
printf("%s -c[f] command [command file] or -F(f) file(write file -f on stdout) -m mcastAddr (Write mode)\n",argv[0]);
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
 }
if(!strcmp(argv[1],"-F")|| !strcmp(argv[1],"-f") || 
 !strcmp(argv[1],"-cf")){
fp = fopen(argv[2],"rb");
if(argc>3) {printf("Put a file name\n"); return 0;}
if(!fp) {printf("%s\n","Unable to open file for reading (read permission).");
            exit(1);}
if(!strcmp(argv[1],"-cf")){
   message[0]='-';message[1]='c'; message[2]='f'; 
   while(fgets(message+3,50,fp))
       //sc=sendto(so,message,53, 0, (struct sockaddr *) &mcast, sizeof(mcast));
       while((n=sendto(so,message,54, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1)break;  
}
else{
int fileround=argv[2][0] + argv[2][strlen(argv[2])-5] - argv[2][strlen(argv[2])-3];
unsigned char filehash3= fileround%90;  

unsigned char filehash= filehash3;
char *filename= (char *) malloc(sizeof(char)*300);
if(!strcmp(argv[1],"-F")) { filename[1]='S'; filename[2]='0'; filename[3]='F';filename[4]='!';}
filename[0]=filehash; filename[5]='\0';
filename=strcat(filename,argv[2]);//initialization 
fseek(fp , 0 , SEEK_END); long size; 
size = ftell(fp); rewind(fp); 
long ntimes= (size/BUF_SIZ);
int rem = size%BUF_SIZ;
char rem1 =rem/256; char rem2=rem%256;

char *buffer = (char *) malloc(sizeof(char *)*size); 
//char *buffer2=(char *)malloc(sizeof(char *)*2); buffer2[0]=filehash3;
//int j=0;
int numr; 
sc=sendto(so,filename,strlen(filename)+1, 0, (struct sockaddr *) &mcast, sizeof(mcast)); 
if(sc==-1) printf("Unable to send, do group exist\n");
int ntimes0=ntimes*2;

for(i=0; i<ntimes0; i++)
while((numr=fread(buffer,sizeof(char),size,fp))!=0);

for(i=0;i< ntimes;i++){
do {numr=fread(buffer,sizeof(char),size,fp);} while(numr!=0);
  c=buffer[i*BUF_SIZ];
 d= buffer[i*BUF_SIZ+1];
buffer[i*BUF_SIZ]=buffer[i*BUF_SIZ] - filehash3;
buffer[i*BUF_SIZ+2]=buffer[i*BUF_SIZ+2]+d-c +filehash3;
while((n=sendto(so,buffer+i*BUF_SIZ,BUF_SIZ, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break; 
} 

 char *remn=(char *)malloc(sizeof(char)*6); remn[4]= rem1; remn[5]=rem2;
remn[0]=filehash3; remn[1]='E'; remn[2]='O'; remn[3]='L'; remn[6]='\0'; 
//printf("remchar:%d",(unsigned char)remn[4]*8);
while((sc=sendto(so,remn,6, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0)
if(sc!=-1) break;
do {numr=fread(buffer,sizeof(char),size,fp);} while (numr!=0);
//numr=fread(buffer,sizeof(char),size,fp);
 c=buffer[i*BUF_SIZ];
 d= buffer[i*BUF_SIZ+1];
buffer[i*BUF_SIZ]=buffer[i*BUF_SIZ] - filehash3;
buffer[i*BUF_SIZ+2]=buffer[i*BUF_SIZ+2]+d-c +filehash3;
while((n=sendto(so,buffer+i*BUF_SIZ,rem, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break ; 
fclose(fp);
remn[3]='f'; 
while((n=sendto(so,remn,6, 0, (struct sockaddr *) &mcast, sizeof(mcast)))!=0) if(n!=-1) break; 
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
FILE *fn[90]; 
//mcastfiles[0]->fhash='0';
//mcastfiles[0]->fname=fopen("1.txt","r");
unsigned char fhash=0,hash0,fhashes[90];
unsigned char index=0,previndex, files2write=0,k=0;
unsigned char  diff=0;
 char fsub,ssub,m2;
int nextlen[90]; for(i=0;i<90;i++)nextlen[i]=BUF_SIZ;
 while(1){
mlen=sizeof(src);
while((i=recvfrom(sock, message, BUF_SIZ, 0, (struct sockaddr *) &src , &mlen))!=0) if(i!=-1) break;
//if(i==-1) continue;


//printf("%s\n",message+1);
if(!strncmp(message+1,"S0F!",4)){ 
previndex= index;
fhash=(unsigned char)message[0];
for(k=0; k<90; k++)
if(fhashes[k]==0){
fhashes[k]= fhash;break;}
index=fhash%90;
if(fhash<90 && !fn[index])
fn[index]= fopen(message+5,"w"); files2write++;
printf("opening file %s for writing\n",message+5);
}
else if(!strncmp(message+1,"EOL",3)){
fhash=(unsigned char)message[0];
 index=fhash%90;
 nextlen[index]=((unsigned char)message[4])*256 + ((unsigned char)message[5]) ;
}
else if(!strncmp(message+1,"EOf",3)){
fhash= (unsigned char)message[0];
for(k=0; k< 90; k++) if(fhash!=0 && fhash==fhashes[k]){ fhashes[k]=0; break;}
fclose(fn[fhash%90]); fn[fhash%90]=NULL;
index= previndex; 
files2write--;
printf("%s %d\n","Finished writing", fhash);
}
else if((nextlen[index]!=BUF_SIZ) && files2write){
diff=calcfhash(message+2,message[1], message,fhashes,90);
if(diff> 0){
fwrite(message,1,nextlen[diff%90],fn[diff%90]);
if(diff==index){
printf("%s %d\n","Finishing writing file", index);
nextlen[index]=BUF_SIZ;}
 index=diff;
diff=0;
}
else if(!strncmp(message,"-c",2)){
if(!strncmp(message,"-cf",3)){
printf("%s\n", &message[3]);
system(&message[3]);
}
else {
printf("%s\n", &message[2]);
system(&message[2]);
}
}
}
else if(files2write){
diff= calcfhash(message+2,message[1], message,fhashes,90);
if(diff> 0){
fwrite(message,1,nextlen[diff%90],fn[diff%90]);
index=diff;
diff=0;
}
else if(!strncmp(message,"-c",2)){
if(!strncmp(message,"-cf",3)){
printf("%s\n", &message[3]);
system(&message[3]);
}
else {
printf("%s\n", &message[2]);
system(&message[2]);
}
}
} 
else if(!files2write && !strncmp(message,"-c",2)){
if(!strncmp(message,"-cf",3)){
printf("%s\n", &message[3]);
system(&message[3]);
}
else {
printf("%s\n", &message[2]);
system(&message[2]);
}
/*
  char *mystr= strstr(message, "-P");
if(mystr){
if(mystr[2]==' ')
 fprintf(stdout,"%s\n", mystr+3);
else 
 fprintf(stdout, "%s\n", mystr+2);
 }*/
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
char *command_str(char *c){
int i=0;
for(i=2;i<strlen(c);i++)
c[i-2]=c[i];
c[i-2]='\0';
return c;
}

unsigned char calcfhash(char *m2, char m1, char *m0, unsigned char hashes[], int arraysize){
int i, m0dec,fsub,ssub;
char diff,m2dec;
for(i=0; i < arraysize; i++){
if(hashes[i] >  0){
fsub= (unsigned char )*m2;
m2dec= *m2-m1+*m0;
m0dec= *m0 + hashes[i];
ssub=(unsigned char) m2dec+m1-m0dec;
diff=fsub-ssub - hashes[i];
//diff= ssub-hashes[i]; 
//printf("fsub:%d, ssub: %d, diff:%d\n", fsub, ssub,diff);
     if(!diff){ *m2=m2dec; *m0=m0dec; return hashes[i]; }
} }

 return 0;
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
