
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<unistd.h>
#include<arpa/inet.h>

int main () {
       //tao socket
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener != -1)
        printf("Socket created: %d\n", listener);
    else {
        printf("Failed to create socket.\n");
        exit(1);
    }

    // Khai bao cau truc dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8080);

    // Gan dia chi voi socket
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))){
        printf("bind() failed.\n ");
        exit (1);
    }

    if (listen(listener,5)){
        printf("listen() failed.\n");
        exit (1);
    }
    else printf("Waiting for a new client...\n");

    // preforking
    int num_process =8 ;
    char buf[1024];
    for(int i = 0 ; i< num_process; i++){
        if (fork() == 0){
            while (1){
                //chờ kết nối
                int client = accept(listener, NULL, NULL);
                printf("New client accepted in process %d: \n", client, getpid());

                //Chờ dữ liệu từ client 
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0) continue;

                //xử lý dữ liệu, gửi về cho client
                buf[ret] = 0;
                printf("Received from client %d: %s\n", client, buf );

                //trả lại dữ liệu cho client
               char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head><body><h1>Xin chao moi nguoi</h1></body></html>";

                send(client, msg, strlen(msg), 0);
                
                //dong ket noi 
                close(client);
                


            }
        }
    }
    wait(NULL);//getchar () ; killpg(0, SIGKILL);
    return 0;
}