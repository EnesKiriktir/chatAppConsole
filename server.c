#include<stdio.h>
#include<string.h>    // for strlen
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h> // for inet_addr
#include<unistd.h>    // for write
#include<pthread.h>   // for threading, link with lpthread
#include <stdbool.h> // for using booleans
#include <json/json.h>
#define SIZEPHONE 200 // size 

int clientID[100];
char isPrivate[100]={}; // group is private or not
int clientSocketNumber[100];
char tempMessage[200]={};
char groupName[100][15]={};// kepping the gorup names
int clientGroups[100][50];
char phoneNumber[100][15]={}; // keeping the phoneNumber of users
int counterClientInGroup[100];
char privateGroupPassword[100][15]={}; // keeping the private groups' passwords
int counterClient=0; // counter for client number
char groupPhoneNumber[100][15]={};
int counterGroup=0; // couner for group number
char socketChar[5]={};

void convertFromJson(char jsonMessage[], char messageValue[]){

	enum json_type type;
	json_object *jobj = json_tokener_parse(jsonMessage);
	json_object_object_foreach(jobj, key, val)

	{

		type = json_object_get_type(val);

		switch (type)

		{

		case json_type_string:

			
			strcpy(messageValue,json_object_get_string(val));


			break;
		}
	}
}
void convertToJson(char phoneNumber1[], char socketNum[], char message1[], char sendingMessage[]){

	char *from=phoneNumber1;

	char *to = socketNum;

	char *message=message1;

	json_object *newObject=json_object_new_object();

	json_object *From=json_object_new_string(from);

	json_object *To =json_object_new_string(to);

	json_object *Message=json_object_new_string(message);

	json_object_object_add(newObject,"from",From);

	json_object_object_add(newObject,"to",To);

	json_object_object_add(newObject,"message",Message);

	strncpy(sendingMessage,json_object_to_json_string(newObject),SIZEPHONE);

}
void isEmpty(int group) // group is empty or not
{
	int i;
	int counterClients=0;// count for clients in the group
	int j;
	int control=0;
	
	for(i=0; i<counterClientInGroup[group]; i++)
	if(clientGroups[group][i]!=-1) counterClients++;
	if(!counterClients)       
	{
		memset(groupName[group],0,15);
		switch(isPrivate[group]){
		case 'p':
			memset(privateGroupPassword[group],0,15); // if group is private and empty then password should be none.
			break;
		}
		isPrivate[group]=0;
	}
	
}

bool comparator(char s1[], char s2[]){ // comparing two strings
	if(strcmp(s1,s2)==0){
		return true;
	}
	else{
		return false;
	}
	
}

bool comparatorFor_n(char s1[], char s2[],int i){// comparing two strings according to n
	if(strncmp(s1,s2,i)==0){
		return true;
	}
	else{return false;}
	
}

void send_message(int group, int sock, const char *message, int len) //sending message to clients that are in group
{

	int i,j,clientNumber=counterClientInGroup[group];
	for(i=0; i<clientNumber; i++)
	{
		if(clientGroups[group][i]==sock || clientGroups[group][i]==-1) continue;
		send(clientGroups[group][i],message,len,0);
	}
}

bool passwordControll(int group, char *password) // password checker
{
	if(comparator(privateGroupPassword[group],password)==true) return true;
	else{
		return false;
	}
	
}


bool intComparator(int i1, int i2){
	if(i1==i2){
		return true;
	}
	else
	return false;
}

void whoAmI_func(char phone[], int socketNum){
	
	char whoAmIMessage[1024]={};
	char socNum[5]={};
	char tp[1024]={};
	sprintf(tp,"--> Your Phone Number: %s\n\0",phone);
	sprintf(socNum,"%d",socketNum);
	convertToJson(phone,socNum,tp,whoAmIMessage);
	send(socketNum,whoAmIMessage,strlen(whoAmIMessage),0);
	memset(tp,0,1024);
	memset(whoAmIMessage,0,1024);
}

void copy(char str1[], char str2[]){
	
	strcpy(str1,str2);
}

bool isAlreadyCreated(const char *group_name){ // if it is true then group is already created.
	int counter=0;
	bool flag = true;
	if(counterGroup==0){
		flag =  comparator(groupName[counter],group_name);
		switch(flag){
		case true:
			flag=true;
			break;
		case false:
			flag= false;
			break;
		}}
	while(counter<counterGroup){
		flag =  comparator(groupName[counter],group_name);
		switch(flag){
		case true:
			flag=true;
			break;
		case false:
			flag= false;
			break;
		}
		counter++;
	}
	return flag;
}
void *chatApp(void *number_socket)
{
	
	char clientPhoneNumber[200]={},groupName_[15]={}; // users phone number and groupname
	int sockNum = *((int*)number_socket);
	char recv_buffer[200]={};
	char comingJsonFormat[200]={};
	char sendingJsonFormat[200]={};
	char comingPhoneJson[200]={};
	int sock = clientSocketNumber[sockNum];
	char send_buffer[200]={};
	int groupNumber=-1; // groupnumber ilk hali
	//Get the socket descriptor

	int i;
	int j;
	int k;
	int len;

	recv(sock, comingPhoneJson, 200, 0);
	
	convertFromJson(comingPhoneJson,clientPhoneNumber);
	
	strcpy(phoneNumber[sockNum],clientPhoneNumber);
	memset(comingPhoneJson,0,200);

	char welcome[100]={};
	sprintf(welcome,">>Now you are in the lobby\n");
	char jsonWelcome[100]={};
	char tempSocket[5]={};
	sprintf(tempSocket,"%d",sock);
	convertToJson(clientPhoneNumber,tempSocket,welcome,jsonWelcome);
	send(sock,jsonWelcome,strlen(jsonWelcome),0);
	memset(welcome,0,100);
	memset(jsonWelcome,0,100);
	memset(tempSocket,0,5);
	
	
	while(1)
	{
		
		if(recv(sock,comingJsonFormat,200,0)==0)//if buffer is empty
		{
			
			break;
		} 
		
		convertFromJson(comingJsonFormat,recv_buffer);
		
		memset(comingJsonFormat,0,200);
		len=strlen(recv_buffer);
		for(i=0; i<len; i++){ 
			if(recv_buffer[i]==' ' || recv_buffer[i]==0 || recv_buffer[i]=='\n' || recv_buffer[i]=='\r')
			break;}
		char exitCommand[]="-exit";
		bool commandExit = comparator(recv_buffer,exitCommand); // if it is true then the command is exit
		char whoAmI[] = "-whoami";
		bool whoAmIFlag = comparator(recv_buffer,whoAmI);
		char joinCommand[] = "-join";
		bool flagJoinCommand = comparatorFor_n(recv_buffer,joinCommand,i-1);
		char exit_Command[] = "-exit";
		bool flagExitFromGroup = comparatorFor_n(recv_buffer,exit_Command,i-1);
		char sendCommand[] = "-send";
		bool sendToGroup = comparatorFor_n(recv_buffer,sendCommand,i-1);
		if(commandExit==true){//exit
			//exit from app command 
			break;
			
		}
		
		else if(whoAmIFlag==true){//whoami
			///////////////////////////////////////
			//whoami command
			switch(whoAmIFlag){
			case true:
				whoAmI_func(clientPhoneNumber,sock);
				break;
			}
		}

		else{
			len=strlen(recv_buffer);
			for(i=0; i<len; i++){ 
				if(recv_buffer[i]==' ' || recv_buffer[i]==0 || recv_buffer[i]=='\n' || recv_buffer[i]=='\r')
				break;}
			char gcreateCommand[] = "-gcreate";
			bool gcreateFlag = comparatorFor_n(recv_buffer,gcreateCommand,i-1);
			if(gcreateFlag==true){//gcreate

				if(gcreateFlag==true){
					char *namePointer = recv_buffer+i+1;
					bool nameFlag = isAlreadyCreated(namePointer);
					char tempAlreadyCreatedMessage[100]={};
					char sendingAlreadyCreatedMessage[100]={};
					switch(nameFlag){ // the case of the group has already been created
					case true:
						sprintf(socketChar,"%d",sock);
						sprintf(tempAlreadyCreatedMessage,"--> This group is already created!\n");
						convertToJson(clientPhoneNumber,socketChar,tempAlreadyCreatedMessage,sendingAlreadyCreatedMessage);
						memset(socketChar, 0,5);
						send(sock,sendingAlreadyCreatedMessage,strlen(sendingAlreadyCreatedMessage),0);
						memset(recv_buffer,0,200);
						memset(tempAlreadyCreatedMessage,0,100);
						memset(sendingAlreadyCreatedMessage,0,100);
						break;
					}
					if(nameFlag==true){
						continue;
					}
					copy(groupName_,recv_buffer+i+1);
					groupNumber = counterGroup++;
					char tempEnterPasswordMessage[100]={};
					char sendingEnterPasswordMessage[100]={};
					sprintf(tempEnterPasswordMessage,"-->Enter the group password:");
					sprintf(socketChar,"%d",sock);	convertToJson(clientPhoneNumber,socketChar,tempEnterPasswordMessage,sendingEnterPasswordMessage);
					memset(socketChar, 0,5);
					send(sock, sendingEnterPasswordMessage,strlen(sendingEnterPasswordMessage),0);
					memset(tempEnterPasswordMessage,0,100);
					memset(sendingEnterPasswordMessage,0,100);
					
					char passwordOfGroup[15]={};
					char tempPasswordOfGroup[100]={};
					
					recv(sock,tempPasswordOfGroup,100,0);
					convertFromJson(tempPasswordOfGroup,passwordOfGroup);
					memset(tempPasswordOfGroup,0,100);
					clientGroups[groupNumber][counterClientInGroup[groupNumber]++]=sock;
					copy(groupName[groupNumber],groupName_);
					copy(privateGroupPassword[groupNumber],passwordOfGroup);
					copy(groupPhoneNumber[groupNumber],phoneNumber[groupNumber]);
					isPrivate[groupNumber] = 'y';
					char tempGroupMessage[100]={};
					sprintf(tempGroupMessage,"-->You are in group: %s\n\0",groupName_);
					sprintf(socketChar,"%d",sock);
					convertToJson(clientPhoneNumber,socketChar,tempGroupMessage,send_buffer);
					memset(socketChar, 0,5);
					send (sock,send_buffer,strlen(send_buffer),0);
					memset(tempGroupMessage,0,100);

				}
			}
			

			else if(flagJoinCommand==true){//join
				
				if(flagJoinCommand==true)
				{
					// groupname or phone number controll
					bool groupControlFlag=false;
					char temp[15]={};
					copy(temp,recv_buffer+i+1);
					
					for(i=0; i<sizeof(groupName); i++){ // if client enters the group name
						if(comparator(groupName[i],temp)==true) 
						{
							groupControlFlag=true;
							break;
						}
					}
					if(groupControlFlag==false){  
						for(i=0; i<sizeof(groupPhoneNumber); i++){ // if client enters the group name
							if(comparator(groupPhoneNumber[i],temp)==true) 
							{
								printf("giriyor\n");
								groupControlFlag=true;
								break;
							}
						}
					}
					
					if(groupControlFlag==false){
						char tempGroupFlag[100]={};
						char groupFlag[100]={};
						sprintf(tempGroupFlag,"-->There is no such group!\n");
						sprintf(socketChar,"%d",sock);
						convertToJson(clientPhoneNumber,socketChar,tempGroupFlag,groupFlag);//converting json format
						memset(socketChar, 0,5);
						send(sock,groupFlag,strlen(groupFlag),0);
						memset(recv_buffer,0,200); 
						memset(tempGroupFlag,0,100);
						memset(groupFlag,0,100);
						continue;
					}
					char tempPassword[15]={};
					char tempSend[100]={};
					char sendPass[100]={};
					sprintf(tempSend,"-->Enter the password:");
					sprintf(socketChar,"%d",sock);
					convertToJson(clientPhoneNumber,socketChar,tempSend,sendPass);
					memset(socketChar, 0,5);
					send(sock,sendPass,strlen(sendPass),0);
					memset(tempSend,0,100);
					memset(sendPass,0,100);
					char comingJsonPassword[100]={};
					recv(sock,comingJsonPassword,100,0);
					convertFromJson(comingJsonPassword,tempPassword);
					bool checkPassword = passwordControll(i,tempPassword);
					if(checkPassword==false){
						sprintf(tempSend,">>Password is not true! Try again later\n");
						sprintf(socketChar,"%d",sock);
						convertToJson(clientPhoneNumber,socketChar,tempSend,sendPass);
						memset(socketChar, 0,5);
						send(sock,sendPass,strlen(sendPass),0); 
						memset(recv_buffer,0,200); 
						memset(tempSend,0,100);
						memset(sendPass,0,100);
						memset(comingJsonFormat,0,100);
						continue;
					}
					copy(groupName_,groupName[i]);
					groupNumber=i;
					clientGroups[groupNumber][counterClientInGroup[groupNumber]++]=sock;
					char tempSuccess[100]={};
					char sendSuccess[100]={};
					sprintf(tempSuccess,"Successful login to group: %s\n\0",groupName_);
					sprintf(socketChar,"%d",sock);
					convertToJson(clientPhoneNumber,socketChar,tempSuccess,sendSuccess);
					memset(socketChar, 0,5);
					
					send(sock,sendSuccess,strlen(sendSuccess),0);
					memset(send_buffer,0,200);
					memset(tempSuccess,0,100);
					memset(sendSuccess,0,100);
					int new_line = strlen(clientPhoneNumber) -1;
					if (clientPhoneNumber[new_line] == '\n'){clientPhoneNumber[new_line] = '\0';}
					sprintf(tempSuccess,"-->The user %s entered the group\n\0",clientPhoneNumber);
					sprintf(socketChar,"%d",sock);
					convertToJson(clientPhoneNumber,socketChar,tempSuccess,sendSuccess);
					memset(socketChar, 0,5);
					
					send_message(groupNumber,sock,sendSuccess,strlen(sendSuccess));
					memset(tempSuccess,0,100);
					memset(sendSuccess,0,100);
				}
				i=0;

			}
			
			else if(flagExitFromGroup==true){//quit
				
				if(flagExitFromGroup==true)
				{
					if(intComparator(groupNumber,-1)==false){
						int x=0;
						while(x<counterClientInGroup[groupNumber]){
							if(intComparator(clientGroups[groupNumber][x],sock)==true){
								char exitFromGroupMessage[50] = {};
								char tempExitGroup[100]={};
								char sendExitGroup[100]={};
								char tempReturnLobby[100]={};
								char sendReturnedLobby[100]={};
								clientGroups[groupNumber][x]=-1;
								int new_line = strlen(clientPhoneNumber) -1;
								if (clientPhoneNumber[new_line] == '\n'){clientPhoneNumber[new_line] = '\0';}
								sprintf(tempExitGroup,"->> The user %s left the group\n\0",clientPhoneNumber);
								sprintf(socketChar,"%d",sock);
								convertToJson(clientPhoneNumber,socketChar,tempExitGroup,sendExitGroup);
								memset(socketChar, 0,5);
								
								printf("--> Socket:%d left the group:%s\n",sock,groupName_);
								sprintf(tempReturnLobby,"--> Exit from group was successful. Returned lobby\n");
								sprintf(socketChar,"%d",sock);
								convertToJson(clientPhoneNumber,socketChar,tempReturnLobby,sendReturnedLobby);
								memset(socketChar, 0,5);
								send(sock,sendReturnedLobby,strlen(sendReturnedLobby),0);
								send_message(groupNumber,sock,sendExitGroup,strlen(sendExitGroup));

								isEmpty(groupNumber);
								groupNumber=-1;
								memset(tempExitGroup,0,100);
								memset(sendExitGroup,0,100);
								memset(tempReturnLobby,0,100);
								memset(sendReturnedLobby,0,100);
								memset(groupName_,0,15);
								break;
							}
							x++;
						}
					}
				}
			}
			
			else if(sendToGroup==true){//send
				
				if(groupNumber==-1)
				{ 
					memset(recv_buffer,0,200);
					continue; 
				}

				int new_line = strlen(clientPhoneNumber) -1;
				if (clientPhoneNumber[new_line] == '\n'){clientPhoneNumber[new_line] = '\0';}
				char tempSendToGroup[100]={};
				char sendingGroup[100]={};
				sprintf(tempSendToGroup,"--->%s:%s\0",clientPhoneNumber,recv_buffer+i);
				sprintf(socketChar,"%d",sock);
				convertToJson(clientPhoneNumber,socketChar,tempSendToGroup,sendingGroup);
				memset(socketChar, 0,5);
				
				send_message(groupNumber,sock,sendingGroup,strlen(sendingGroup));
				memset(tempSendToGroup,0,100);
				memset(sendingGroup,0,100);
			}
		}
		
		memset(recv_buffer,0,200);
		memset(send_buffer,0,200);
		
	}
	clientSocketNumber[sockNum]=-1; //free socket
	printf("-->Socket:%d disconnected from server\n",sock);
	close(sock);    //close connection between server and socket
	return 0;
}

void fillClientNumber(char arr[]){
	int counter =0;
	while(counter<100){
		arr[counter]=counter;
		counter++;
	}
}

void fillGroupInit(char arr[100][50]){
	int counter1=0;
	int counter2=0;
	while(counter1<100){
		while(counter2<50){
			arr[counter1][counter2]=-1;
		}
	}
}

int main ()
{
	struct sockaddr_in server;
	struct sockaddr_in client;
	int i=0;
	int j=0;
	char *message;
	int socketDesc;
	int newSocket;
	int c;
	int *newSock;
	for(i=0; i<100; i++)
	clientID[i]=i;
	//initialize the room situations
	for(i=0; i<100; i++)
	for(j=0; j<50; j++)
	clientGroups[i][j]=-1;
	
	//fillClientNumber(clientID);
	//fillGroupInit(clientGroups);
	
	socketDesc = socket(AF_INET , SOCK_STREAM , 0); //creating socket
	switch(socketDesc){
	case -1:
		puts("Could not create socket");
		return 1;
		break;
	}
	
	//assign server's port and ip address
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(3205);
	
	//binding server
	int bindNum = bind(socketDesc, (struct sockaddr *)&server, sizeof(server));

	if( bindNum< 0)
	{
		puts("Binding failed");
		return 1;
	}
	//waiting clients
	listen(socketDesc, 50);  
	
	puts("Waiting connections...");
	c = sizeof(struct sockaddr_in);
	//client connection and multithread management
	while((newSocket = accept(socketDesc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		printf("-->Socket:%d connected to server\n",newSocket);
		
		//store client's socket info
		clientSocketNumber[counterClient]=newSocket;
		
		//create thread of client
		pthread_t cid;
		int pth=pthread_create(&cid, NULL, chatApp, (void*)&clientID[counterClient++]);
		if( pth < 0)
		{
			puts("Could not create thread");
			return 1;
		}
	}
	return 0;
}