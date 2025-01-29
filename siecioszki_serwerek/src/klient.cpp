#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <string.h>




void maintest(int argc, char * argv[]){


int des,rc;
char buf[255];
struct sockaddr_in saddr;
struct hostent * h;
h = gethostbyname(argv[1]);

des = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
memset(&saddr,0,sizeof(saddr));

saddr.sin_family = AF_INET;
saddr.sin_port = htons(atoi(argv[2]));
memcpy(&saddr.sin_addr.s_addr,h->h_addr,h->h_length);

connect(des,(struct sockaddr*)&saddr,sizeof(saddr));
write(des,"123456",6);
rc = read(des,buf,sizeof(buf));
write(1,buf,rc);
close(des);





}
