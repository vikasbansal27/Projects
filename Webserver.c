/* This is a HTTP web server. Having a client and a host. File types such as html page, image (gif, jpg, jpeg and png), zip, tar and text. */
 
#include<sys/types.h>      
#include<sys/socket.h>      
#include<sys/time.h>       
#include<time.h>            
#include<netinet/in.h>      
#include<arpa/inet.h>    
#include<errno.h>
#include<netdb.h>
#include<signal.h>
#include<sys/stat.h>      
#include<sys/uio.h>        
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/select.h>
#include<sys/sysctl.h>
#include<poll.h>
#include<strings.h>
#include<sys/ioctl.h>
#include<pthread.h>
#include<assert.h>
#include<fcntl.h>

#define MAXLINE 4096
#define BACKLOG 5

void handle_request(int,char [], int);
int process_request(int);

int PORT=5665;
char root[100]="/www";

int main(int argc, char **argv)
{
//------variable declaration-------
	struct sockaddr_in hostaddr, destaddr;
	int maxfd;
	int sockfd;
	int newfd;
	int data;
	socklen_t siz;
	fd_set tempfd, mainfd;
	int i, j;
	int addrlen;	
	struct timeval tv;

	FD_ZERO(&mainfd);
	FD_ZERO(&tempfd);
//---------------------------------------
    if(argc == 1)    return;
    for(i=1;i<argc;i++)
        if(argv[i][0] == '-'){
            if(!strcmp(&argv[i][1],"p"))
                sscanf(argv[++i],"%d",&PORT);
            else if(!strcmp(&argv[i][1],"root"))
                sscanf(argv[++i],"%s",root);
             else{
                printf("Invalid arguments\nOnly -p port -root directory\n");
                exit(1);
            }
        }

//--------create socket------------
	
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{	perror("socket");
		exit(1);
	}

//-------socket address info----------

	hostaddr.sin_family = AF_INET;
	hostaddr.sin_addr.s_addr = INADDR_ANY;
	hostaddr.sin_port = htons(PORT);
	memset(&(hostaddr.sin_zero), '\0', 8);
	siz= sizeof(struct sockaddr) ;

//--------bind the socket------------

	if(bind(sockfd, (struct sockaddr *)&hostaddr, siz) == -1)
	{	perror("listen");
		exit(1);
	}

//--------listen on the socket,sockfd-----------
	
	if(listen(sockfd, BACKLOG) == -1 )
	{	perror("listen");
		exit(1);
	}
		
//----------Now using SELECT-----------
	
	FD_SET(sockfd, &mainfd);
	tv.tv_sec = 30;
	tv.tv_usec = 0;
	maxfd = sockfd;

	while(1)
	{	tempfd = mainfd;
		if(select(maxfd+1, &tempfd, NULL, NULL, &tv) == -1)
		{	perror("select");
			exit(1);
		}
		
		for(i=0; i <= maxfd; i++)
		{	if(FD_ISSET(i, &tempfd))
			{	if(i == sockfd)
				{	addrlen = sizeof(destaddr);
					if((newfd = accept(sockfd, (struct sockaddr *)&destaddr, &addrlen)) == -1)
					{	perror("accept");
						continue;
					}
					else
					{	FD_SET(newfd, &mainfd);
						if(newfd > maxfd)
							maxfd = newfd;
						printf("selectserver: New connection from %s on socket %d\n", inet_ntoa(destaddr.sin_addr), newfd);
					}
				}
				else
				{	int status = process_request(i);
					if(!status)
                       				FD_CLR(i, &mainfd);
				}
			}
		}
	}	
}


int process_request(int sockfd)
{
    int n;
    char buf[MAXLINE];
    // read the request and echo it back
    if((n = read(sockfd, buf, MAXLINE)) == 0){// connection closed by client
        close(sockfd);
        puts("connection closed by client");
        return 0;
    }
    else{
        perror("read failure :");
        buf[n]=0;
        //handles request of the client
        handle_request(sockfd,buf,n);
        return 1;
    }
}



void handle_request(int sockfd,char buf[], int n)
{
	struct
	{
	        char *ext,*filetype;
	}
	extensions[] = {{"gif", "image/gif" },{"jpg", "image/jpeg"}, {"jpeg","image/jpeg"},{"png", "image/png" }, 
                {"zip", "image/zip" },{"gz",  "image/gz"  },{"tar", "image/tar" },{"htm", "text/html" }, 
                {"html","text/html" },{"c","text" },{"cpp","text" },
                {0,0}};

    int i,j,filesize;
    char code[100],temp[100];;
    char response[400];
    strcpy(code,"200 OK");
    strcpy(response,"");   


        for(i=0;i<n;i++)      // remove CR and LF characters
                if(buf[i] == '\r' || buf[i] == '\n')
                        buf[i]=' ';

    if((!strncmp(buf,"GET ",4)) || (!strncmp(buf,"get ",4))){
            for(i=4;i<n;i++)// null terminate after the second space to ignore extra stuff
                    if(buf[i] == ' ') { // string is "GET URL " +lots of other stuff
                            buf[i] = 0;
                            break;
                    }
    }
        else  if((!strncmp(buf,"HEAD ",5)) || (!strncmp(buf,"HEAD ",5))){
                for(i=5;i<n;i++)// null terminate after the second space to ignore extra stuff
                        if(buf[i] == ' ') { // string is "HEAD URL " +lots of other stuff
                                buf[i] = 0;
                                break;
                        }
        }
    else{
        perror("request not supported");
        strcpy(code,"405 Method not allowed");
        strcpy(response,"<h1>405 Method not allowed</h1>");
        goto here;
    }


        for(j=0;j<i-1;j++)      // check for illegal parent directory use ..
                if(buf[j] == '.' && buf[j+1] == '.'){
                        perror("Parent directory (..) path names not supported");
            strcpy(code,"403 FORBIDDEN");
            strcpy(response,"<h1>403 FORBIDDEN</h1>");
            goto here;
        }



        if( !strncmp(buf,"GET /\0",6) || !strncmp(buf, "get /\0",6) ) // convert no filename to index file
        strcpy(buf,"GET /index.html");

        if( !strncmp(buf,"HEAD /\0",7) || !strncmp(buf, "head /\0",7) ) // convert no filename to index file
              strcpy(buf,"GET /index.html");


        // work out the file type and check we support it
        int buflen=strlen(buf);
    puts(buf);
        char *fstr = (char *)0;
    printf("buf = %s\n",buf);
        for(i=0;extensions[i].ext != 0;i++) {
              int len = strlen(extensions[i].ext);
              if( !strncmp(&buf[buflen-len], extensions[i].ext, len)) {
                        fstr =extensions[i].filetype;
                        break;
                }
       }
   
        if(fstr == 0){
        perror("SORRY file extension type not supported");
        strcpy(code,"400 BAD REQUEST");
        strcpy(response,"<h1>400 BAD REQUEST</h1>");
        goto here;
    }

    int fd;
    char path[200];
    path[0]='\0';
    strcat(path,root);
    strcat(path,"/");
    strcat(path,&buf[5]);
    puts(path);
        if(( fd = open(path,O_RDONLY)) == -1){ // open the file for reading
                perror("SORRY failed to open file");
        strcpy(code,"404 NOT FOUND");
        strcpy(response,"<h1>404 NOT FOUND</h1>");
    }

    //GOTO LABEL
    here:

    //send header

    filesize=strlen(response);

    if(!strncmp(code,"200 OK",6)){
        filesize=lseek(fd,0,SEEK_END);
        lseek(fd,0,SEEK_SET);
    }
   

        sprintf(temp,"HTTP/1.1 %s\r\nContent-length: %d\r\nContent-Type: %s\r\n\r\n",code,filesize,fstr);
    puts(temp);
        if(write(sockfd,temp,strlen(temp)) == -1){
        perror("write : ");   
    }
   
    if(!strncmp(code,"200 OK",6)){
        if((!strncmp(buf,"GET ",4)) || (!strncmp(buf,"get ",4))){
                while ( (n = read(fd, buf, MAXLINE)) > 0 )
                        write(sockfd,buf,n);
            close(fd);
            puts("file succesfully sent");
        }
    }
    else{
        write(sockfd,response,strlen(response));
    }
}
					
			






	
