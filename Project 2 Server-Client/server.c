#include <time.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>

#define PORT 8080

#define MAXBUF 1024
#define MAXHOSTNAME 200
#define MAXACTION 10
#define MAXPORT 6


int main(int argc, char const *argv[]){

//log.txt file creation
FILE *log;
FILE *reader;

log = fopen("log.txt", "w+");



struct sockaddr_in my_addr;
socklen_t addrLen = sizeof(my_addr);
int port = 0;

int SIZE = 10; //max size

char *agents[SIZE]; // list of current agents and their connect timers
char *times[SIZE];  

//std::vector<agentInfo> agents; // check later 

printf("Enter the port: ");
scanf("%hu", &port); //test

//set values for struct
memset(&my_addr, '\0', addrLen); // set values to default
my_addr.sin_family = AF_INET;
my_addr.sin_port = htons(port);
my_addr.sin_addr.s_addr = INADDR_ANY;
memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));

int server_fd;
//Socket create
if((server_fd = socket(AF_INET,SOCK_STREAM,0)) == 0){

   fprintf(stderr, "ERROR: server failed socket create\n");
   exit(-1);
}

printf("\nServer Socket Created\n");

//Socket bind
if(bind(server_fd, (struct sockaddr *)&my_addr, addrLen) < 0){

   fprintf(stderr, "ERROR: server failed binding\n");
   exit(-1);
   }

printf("\nServer Bind Success\n");

char szHostName[255];
gethostname(szHostName, 255);
struct hostent *host_entry;
host_entry = gethostbyname(szHostName);
char* szLocalIP;
szLocalIP = inet_ntoa(*(struct in_addr *)*host_entry->h_addr_list);
printf("\nServer IP: %s\n", szLocalIP);
//TEST ^^^^^^^^^^^^^

//Prompt that server is running 
printf("\nListening for clients\n");
if ((listen(server_fd,1)) < 0){
   fprintf(stderr, "ERROR: Server Listen Failed\n");
   exit(-1);
}


char buffer[MAXBUF];
int bytes_read = 0;
int total_bytes_read = 0;
int found = 0; // flag
int i;
struct tm ts;
struct timeval connected[SIZE];
struct timeval current;
struct timeval difference;
time_t TIME;

for(i = 0; i < SIZE; i++){ //set all values in array to NULL
   agents[i] = NULL;
}

//infinite loop for running server
while(true){
   struct sockaddr_in client;
   int clientSocket;
   socklen_t clientLength = sizeof(client);
   memset(&client, 0, clientLength);

   clientSocket = accept(server_fd, (struct sockaddr *)&client, &clientLength);

   memset(buffer,0,MAXBUF); //Read command
   bytes_read = read(clientSocket, buffer, MAXBUF);

   if(bytes_read < 0)
      break;
   
   fprintf(stdout, "\nRead %d bytes: [%s]\r\n", bytes_read,buffer);
   
   char *connectedIP = strdup(inet_ntoa(client.sin_addr));
 

//JOIN  
   if((strcmp(buffer, "#JOIN")) == 0){
      found = 0;
      
      TIME = time(NULL);
      ts = *localtime(&TIME);
   
      char buf[80];
      strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
      fprintf(log, "%s: Recieved a \"#JOIN\" action from agent \"%s\"", buf, connectedIP);
      fprintf(log, "\n\n");
      fflush(log);

      for(i = 0; i < SIZE; i++){
         if(agents[i] == NULL){
         }
     
      else if(strcmp(agents[i], connectedIP) == 0){ //if agent already in list
         found = 1;
         
         char response[] = "#ALREADY MEMBER";
         write(clientSocket, response, strlen(response));
         
         TIME = time(NULL);
         ts = *localtime(&TIME);

         
         char buf[80];
         strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
         fprintf(log, "%s: Responded to agent \"%s\" with \"#ALREADY MEMBER\"", buf, connectedIP);
         fprintf(log, "\n\n");
         fflush(log);
         i = SIZE;
         }
      }
  
      if(found == 0){ // if new agent joins
         for(i = 0; i < SIZE; i++){
            if(agents[i] == NULL){
               agents[i] = connectedIP;

               gettimeofday(&connected[i],NULL);
               TIME = time(NULL);
              // times[i] = TIME;
               ts = *localtime(&TIME);
              
               char buf[80];
               strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
               fprintf(log, "%s: Responded to agent \"%s\" with \"#OK\"", buf, connectedIP);
               fprintf(log, "\n\n");
               fflush(log);
         
               char response[] = "#OK";
               write(clientSocket, response, strlen(response));
               i = SIZE;
               }
            }
         }
      }
//LEAVE   
   else if((strcmp(buffer, "#LEAVE")) == 0){
      found = 0;
  
      TIME = time(NULL);
      ts = *localtime(&TIME);

      char buf[80];
      strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z" , &ts);
      fprintf(log, "%s: Recieved a \"#LEAVE\" action from agent \"%s\"", buf, connectedIP);
      fprintf(log, "\n\n");
      fflush(log);

      for(i=0; i < SIZE; i++){
         if(agents[i] == NULL){
         }

         else if(!strcmp(agents[i], connectedIP)){ // remove agent
            found = 1;

            agents[i] = NULL;
            times[i] = NULL;
            
            TIME = time(NULL);
            ts = *localtime(&TIME);
         
            char buf[80];
            strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
            fprintf(log, "%s: Responded to agent \"%s\" with \"#OK\"", buf, connectedIP);
            fprintf(log, "\n\n");
            fflush(log);

            }
         }
   }

//LIST


   else if ((strcmp(buffer, "#LIST")) == 0){
      int index = -1;
      found = 0;
 
      TIME = time(NULL);
      ts = *localtime(&TIME);
 
      char buf[80];
      strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z" , &ts);
      fprintf(log, "%s: Recieved a \"#LIST\" action from agent \"%s\"", buf, connectedIP);
      fprintf(log, "\n\n");
      fflush(log);


      for(i = 0; i < SIZE; i++){
         if(agents[i] == NULL){
         }

      else if(strcmp(agents[i], connectedIP) == 0){
       
         found = 1;
         index = i;
         i = SIZE;
         }
      }

   if(found == 1){
      for(i = 0; i < SIZE; i++){
         if(agents[i] != NULL){
            gettimeofday(&current, NULL);
            timersub(&current, &connected[i], &difference);
        
            char response[100];
            sprintf(response, "<%s, %ld.%06ld>", agents[i], (long int)difference.tv_sec, (long int)difference.tv_usec);
            write(clientSocket, response, strlen(response));
      }
   }
   
   TIME = time(NULL);
   ts = *localtime(&TIME);

   char buf[80];
   strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
   fprintf(log, "%s: Responded to agent \"%s\" with current list", buf, connectedIP);
   fprintf(log, "\n\n");
   fflush(log);
   }

   else{
      TIME = time(NULL);
      ts = *localtime(&TIME);

      char buf[80];
      strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
      fprintf(log, "%s: No response supplied to agent \"%s\"", buf, connectedIP);
      fprintf(log, "\n\n");
      fflush(log);
      }
   }

//LOG

   else if((strcmp(buffer, "#LOG")) == 0){
      found = 0;
      int index = -1;

      TIME = time(NULL);
      ts = *localtime(&TIME);
   
      char buf[80];
      strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
      fprintf(log, "%s: Recieved a \"#LOG\" action from agent \"%s\"", buf, connectedIP);
      fprintf(log, "\n\n");
      fflush(log);

      for(i = 0; i < SIZE; i++){ //if agent not in list
         if(agents[i] == NULL){
         }

         else if(strcmp(agents[i], connectedIP) == 0){
            found = 1;
            index = i;
            i = SIZE;
         }
      }
      
      if(found == 0){ // if in list read log and write
      
      TIME = time(NULL);
      ts = *localtime(&TIME);
      
      char buf[80];
      strftime(buf, sizeof(buf), "%a %Y0%m0%d %H:%M:%S %Z", &ts);
      fprintf(log, "%s: No response supplied to agent \"%s\"", buf, connectedIP);
      fprintf(log, "\n\n");
      fflush(log);
      }
   
      else{
      
         memset(buffer, 0, MAXBUF);
         reader = fopen("log.txt", "r");
         if(reader == NULL) perror ("Error opening file");
         else{
            while(!feof (reader)){
               if(fgets(buffer, MAXBUF, reader) == NULL) break;
               write(clientSocket, buffer, strlen(buffer));
            }
            fclose(reader);
         }

         TIME = time(NULL);
         ts = *localtime(&TIME);

         char buf[80];
         strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
         fprintf(log, "%s: Responded to agent \"%s\" with current log", buf, connectedIP);
         fprintf(log, "\n\n");
         fflush(log);

      }
   }
   
   else{ //if no correct command is detected
      printf("ERROR: Invalid command from agent\n");
   
      char response[] = "#INVALID";
      write(clientSocket, response, strlen(response));
      
      TIME = time(NULL);
      ts = *localtime(&TIME);
      
      char buf[80];
      strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
      fprintf(log, "%s: Recieved an \"#INVALID\" action from agent \"%s\"", buf, connectedIP);
      fprintf(log, "\n\n");
      fflush(log);
      }

      close(clientSocket); // clean up socket and close out log.txt
      memset(&client, 0, sizeof(client));
      fflush(log);

   }
         
            
return 0;

}
