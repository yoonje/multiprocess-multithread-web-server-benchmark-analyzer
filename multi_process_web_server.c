#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

void* request_handler(void* arg);
void send_data(FILE* fp, char* ct, char* file_name);
char* content_type(char* file);
void send_error(FILE* fp);
void error_handling(char* message);
void read_childproc(int sig);

pthread_mutex_t mutex_lock;

int func1( int x )
{
	return x+x;	
} 

inline int func2( int x )
{
	return x+x;		
}

int main(int argc, char *argv[])
{
   int serv_sock, clnt_sock;
   struct sockaddr_in serv_adr, clnt_adr;
   int clnt_adr_size,state;
   char buf[BUF_SIZE];
   pid_t pid;
   pthread_t t_id;
   struct sigaction act;
   clock_t start, end;
   double res;

   pthread_mutex_init(&mutex_lock, NULL);

   if(argc!=2) {
      printf("Usage : %s <port>\n", argv[0]);
      exit(1);
   }
   
   act.sa_handler=read_childproc;
   sigemptyset(&act.sa_mask);
   act.sa_flags=0;
   
   serv_sock=socket(PF_INET, SOCK_STREAM, 0);
   memset(&serv_adr, 0, sizeof(serv_adr));
   serv_adr.sin_family=AF_INET;
   serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
   serv_adr.sin_port = htons(atoi(argv[1]));

   if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
      error_handling("bind() error");
   if(listen(serv_sock, 20)==-1)
      error_handling("listen() error");

   while(1)
   {
      clnt_adr_size=sizeof(clnt_adr);
      clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
      printf("Connection Request : %s:%d\n", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
      pid=fork();
      start = clock();

      if(pid==-1)
      {
         close(clnt_sock);
         continue;
      }
      
      if(pid==0){
            close(serv_sock);
            request_handler(&clnt_sock);
            close(clnt_sock);
            end = clock();
            res = (double)(end - start);
            printf("Duration of Time:%f\n", res);
            return 0;
      }

      else{
         printf("pid : %d\n",pid);
         close(clnt_sock);
      }
   }
   close(serv_sock);
   return 0;
}

void* request_handler(void *arg)
{
   int clnt_sock=*((int*)arg);
   char req_line[SMALL_BUF];
   FILE* clnt_read;
   FILE* clnt_write;
   
   char method[10];
   char ct[15];
   char file_name[30];
 
   pthread_mutex_lock(&mutex_lock);

   clnt_read=fdopen(clnt_sock, "r");
   clnt_write=fdopen(dup(clnt_sock), "w");
   fgets(req_line, SMALL_BUF, clnt_read);

   if(strstr(req_line, "HTTP/")==NULL)
   {
      send_error(clnt_write);
      fclose(clnt_read);
      fclose(clnt_write);
      printf("Error\n");
      return;
    }
   
   strcpy(method, strtok(req_line, " /"));
   strcpy(file_name, strtok(NULL, " /"));
   strcpy(ct, content_type(file_name));

   if(strcmp(method, "GET")!=0)
   {
      send_error(clnt_write);
      fclose(clnt_read);
      fclose(clnt_write);
      printf("Error\n");
      return;
   }
   
   fclose(clnt_read);
   pthread_mutex_unlock(&mutex_lock);
   send_data(clnt_write, ct, file_name);
}

void send_data(FILE* fp, char* ct, char* file_name)
{
   char protocol[]="HTTP/1.0 200 OK\r\n";
   char server[]="Server:Linux Web Server \r\n";
   char cnt_len[SMALL_BUF];
   char cnt_type[SMALL_BUF];
   char buf[BUF_SIZE];
   FILE* send_file;
   int size;
   
   sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);
   send_file=fopen(file_name, "rb");

   fseek(send_file, 0, SEEK_END);
   size = ftell(send_file);
   fseek(send_file, 0, SEEK_SET);
   sprintf(cnt_len, "Content-length:%d\r\n", size);

   if(send_file==NULL)
   {
      printf("Send file Error\n");
      send_error(fp);
      return;
   }

   fputs(protocol, fp);
   fputs(server, fp);
   fputs(cnt_len, fp);
   fputs(cnt_type, fp);

   while(fgets(buf, BUF_SIZE, send_file)!=NULL)
   {
      fputs(buf, fp);
      fflush(fp);
   }
   fflush(fp);
   fclose(fp);
   printf("Success!!\n");
}

char* content_type(char* file)
{
   char extension[SMALL_BUF];
   char file_name[SMALL_BUF];
   strcpy(file_name, file);
   strtok(file_name, ".");
   strcpy(extension, strtok(NULL, "."));
   
   if(!strcmp(extension, "html")||!strcmp(extension, "htm"))
      return "text/html";
   else if(!strcmp(extension, "png"))
      return "image/png";
   else if(!strcmp(extension, "jpg"))
      return "image/jpg";
   else
      return "text/plain";
}

void send_error(FILE* fp)
{  
   char protocol[]="HTTP/1.0 400 Bad Request\r\n";
   char server[]="Server:Linux Web Server \r\n";
   char cnt_len[]="Content-length:2048\r\n";
   char cnt_type[]="Content-type:text/html\r\n\r\n";

   fputs(protocol, fp);
   fputs(server, fp);
   fputs(cnt_len, fp);
   fputs(cnt_type, fp);
   fflush(fp);

   printf("Error\n");
}

void error_handling(char* message)
{
   fputs(message, stderr);
   fputc('\n', stderr);
   exit(1);
}

void read_childproc(int sig)
{
   pid_t pid;
   int status;
   pid=waitpid(-1,&status,WNOHANG);
}
