#include <stdio.h>
#include "header/client_info.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int start_server_for_web();
int command(client_info *client);
int android[100] = {0, };

int main()
{
    int pid;
	int i, j;
	
	for (i = 0; i < 100; i++)
		android[i] = 1;
		
	
   
    // Web Server로 부터 명령 대기
    if ((pid = fork()) > 0)
    {
        start_server();       
    }
    else if (pid == 0)
    {
       
    }
   


   
}

int start_server()
{
    client_info *client;

    char buffer[500];
    struct sockaddr_in server_addr, client_addr;
    int server_fd, client_fd;
    int size;
    int len;
	int nSockopt = 1;


    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Server_For_Web : can't open stream socket\n");
        return -1;
    }

    memset(&server_addr, 0x00, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(5799);


	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &nSockopt, sizeof(int));

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Server_For_Web : can't bind local address.\n");
        return -1;
    }

    if (listen(server_fd, 5) < 0)
    {
        printf("Server_For_Web : can't listening connect.\n");
        return -1;
    }
   
    memset(buffer, 0x00, sizeof(buffer));
    printf("Server_For_Web : Waiting connection request.\n");
    len = sizeof(client_addr);

    while(1)
    {
        client = (client_info *)malloc(sizeof(client_info));

        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
        if (client_fd < 0)
        {
            printf("Server_For_Web : Accept failed.\n");
            continue;
        }
        printf("Server_For_Web : Connected with Web Server.\n");
        char *temp;
       
		
        size = read(client_fd, buffer, 500);
        printf("%s\n", buffer);

        temp = strtok(buffer, " ");
        client->command = atoi(temp);
        printf("%d\n", client->command);
		
		temp = strtok(NULL, " ");
        client->os_type = atoi(temp);
        printf("%d\n", client->os_type);

		temp = strtok(NULL, " ");
		client->index = atoi(temp);
		printf("%d\n", client->index);	
		
		temp = strtok(NULL, " ");
		client->id = temp;
		printf("%s\n", client->id);

		if (client->command == 0 || client->command == 3) 
		{
			temp = strtok(NULL, " ");
			client->ip = temp;
			printf("%s\n", client->ip);

			temp = strtok(NULL, " ");
			client->connect_type = atoi(temp);
			printf("%d\n", client->connect_type);
			
			temp = strtok(NULL, " ");
			client->port_i = atoi(temp);
			printf("%d\n", client->port_i);
			
			temp = strtok(NULL, " ");
			client->port_a = atoi(temp);
			printf("%d\n", client->port_a);

			temp = strtok(NULL, " ");
			client->port_e = atoi(temp);
			printf("%d\n", client->port_e);
		}
		



/*
        client->command = atoi(buffer);
        printf("%d %d \n", size,client->command);
//1
        size = read(client_fd, buffer, 1);
        client->type = atoi(buffer);
        printf("%d %d \n", size,client->type);
//2
        size = read(client_fd, buffer, 1);
        client->width = atoi(buffer);
        printf("%d %d \n", size,client->width);
//3
        size = read(client_fd, buffer, 1);
        client->height = atoi(buffer);
        printf("%d %d \n", size,client->height);
//4
        size = read(client_fd, buffer, 1);
        client->id_length = atoi(buffer);
        printf("%d %d \n", size,client->id_length);
//5
        client->id = (char *)malloc(client->id_length);
       
        size = read(client_fd, client->id, client->id_length);
        printf("%d %s \n", size,client->id);
//6
        size = read(client_fd, buffer, 1);
        client->ip_length = atoi(buffer);
        printf("%d %d \n", size,client->ip_length);
        client->ip = (char *)malloc(client->ip_length);
//7
        size = read(client_fd, client->ip, client->ip_length);
        printf("%d %s \n", size,client->ip);
//8
*/
        command(client);
        close(client_fd);       
    }
   
}

int command(client_info *client)
{ 
	FILE *fp = NULL;
	char command[100];
	char os[100];
	int index=0;
	int i;
	char buf[100];
	int pid;

	
	if (client->command == 1)
	{
		if (client->os_type == 0) 
		{
			for (i = 0 ; i < 100 ; i++)
			{
				if (android[i] == 1)
				{
					index = i;
					android[i] = 0;
					break;
				}

			}
		}
	}

	else if (client->command == 2)
	{
		if (client->os_type == 0) 
		{
			for (i = 0 ; i < 100 ; i++)
			{
				if (android[i] == 0)
				{
					index = i;
					android[i] = 1;
					break;
				}

			}
		}
	}

	


	switch (client->os_type)
	{
		case 0:
			strcpy(os,"android");
			break;
		case 1:
			strcpy(os, "tizen");
			break;
		case 2:
			strcpy(os, "ubuntu");
			break;
		case 3:
			strcpy(os, "windows7");
			break;
	}
	
	if (client -> command == 0 || client -> command == 3)
	{
		chdir(client->id);
		printf("%s\n", get_current_dir_name());
		if ((fp = fopen("client_data.dat", "w")) == NULL)
		{
			printf("file open() error\n");
			return -1;
		}
		sprintf(buf, "%d %d %d %d", client->os_type ,client->port_i, client->port_a, client->port_e);
		fwrite(buf, 1, strlen(buf), fp);
		
		if (fclose(fp) < 0)
		{
			printf("file close() error\n");
			return -1;
		}
		chdir("..");
	}
	sprintf(command, "./command.sh %d %s %s %d %d", client->command, client->id, os, client->index ,index);
	if ((pid = fork()) > 0)
	{
		system(command);
	}

	
	
	

}
