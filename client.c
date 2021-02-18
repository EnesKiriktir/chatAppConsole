#include<stdio.h>

#include<string.h>

#include<sys/socket.h>

#include<arpa/inet.h> // for inet_addr

#include<unistd.h>    // for write

#include<pthread.h>   // for threading, link with lpthread

#include<stdlib.h>

#include <json/json.h>

#include <stdbool.h>

#define SIZEPHONE 200

struct sockaddr_in server;

int socket_desc; // socket descriptor

char phoneNumberConvert[15]={};

char messageString[SIZEPHONE];

char phoneNumber[15]={};
char tempPhoneNumber[15]={};


void convertToJson(char phoneNumber1[], char socketNum[], char message1[]){//converting to json string

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

	strncpy(messageString,json_object_to_json_string(newObject),SIZEPHONE);

}

bool stringComparator(char s1[], char s2[]){

	if(strcmp(s1,s2)==0){

		return true;

	}

	else{return false;}

	

}

void convertFromJson(char jsonMessage[]){ // convert from json

	enum json_type type;
	json_object *jobj = json_tokener_parse(jsonMessage);
	json_object_object_foreach(jobj, key, val)

	{

		type = json_object_get_type(val);

		switch (type)

		{

		case json_type_string:

			sprintf(messageString,"%s",json_object_get_string(val));
			
			break;
		}
	}
}

void getMessage(){// get  message from server

	char messageBody[SIZEPHONE]={};

	bool flag=true;

	while(flag==true){

		if(recv(socket_desc,messageBody,SIZEPHONE,0)>0){

			convertFromJson(messageBody);
			printf("%s\n",messageString);

		}

		else{break;}

		memset(messageBody,0,SIZEPHONE);
		memset(messageString,0,SIZEPHONE);
	}
}

void sendMessage(){ // send message to server

	char messageBody[SIZEPHONE]={};

	char strSocketNum[5]={};// for casting from int to char

	bool flag=true;

	while(flag==true){

		gets(messageBody); // get input

		sprintf(strSocketNum,"%d",socket_desc);

		convertToJson(phoneNumberConvert, strSocketNum, messageBody);//convert json

		send(socket_desc,messageString,strlen(messageString),0); // send to server

		memset(messageBody,0,SIZEPHONE);
		memset(messageString,0,SIZEPHONE);

		memset(strSocketNum,0,5);

		if(stringComparator(messageBody,"-exit") == true){

			flag==false;

			break;

		}

		else if(flag!=false){

			memset(messageBody,0,SIZEPHONE);

		}
	}
}

int main( int argc, char *argv[]){

	int x;
	int y;
	int z;
	int length;

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	switch(socket_desc){

	case -1:

		puts("Could not create socket");

		return 1;

		break;

	}

	server.sin_addr.s_addr = INADDR_ANY;

	server.sin_family = AF_INET;

	server.sin_port = htons(3205);

	//inet_pton(AF_INET,"127.0.0.1",&server.sin_addr);

	int cont = connect(socket_desc, (struct sockaddr *)&server, sizeof(server));

	if ( cont< 0)

	{

		puts("Connection Error");

		return 1;

	}
	char sckCharNum[5]={};

	printf("Enter phone number: \n"); // entering phone number for user

	fgets(tempPhoneNumber,SIZEPHONE,stdin);

	tempPhoneNumber[sizeof(tempPhoneNumber)-1]=0;
	sprintf(sckCharNum,"%d",socket_desc);
	convertToJson(tempPhoneNumber,sckCharNum,tempPhoneNumber);//convert to json
	
	send(socket_desc, messageString, strlen(messageString), 0); // send the user info to server
	memset(messageString,0,200);
	pthread_t sendingMessage_t;

	int contThread=pthread_create(&sendingMessage_t, NULL, (void *) sendMessage, NULL);

	if ( contThread!= 0) {

		printf ("Pthread Creating Error!\n");

		exit(EXIT_FAILURE);

	}

	//create receive message thread and wait to receive message until exit

	pthread_t receivingMessage_t;
	int contTH=pthread_create(&receivingMessage_t, NULL, (void *) getMessage, NULL);

	if ( contTH != 0) {

		printf ("Pthread Creating Error!\n");
		exit(EXIT_FAILURE);

	}

	//thread end
	pthread_join(receivingMessage_t,NULL);

	close(socket_desc); //connection end

}