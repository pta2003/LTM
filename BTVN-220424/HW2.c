
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<unistd.h>
#include<arpa/inet.h>
#include <time.h>

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
    addr.sin_port = htons(9000);

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
                //Kiểm tra cú pháp của lệnh mà client gửi GET_TIME [format
                char cmd[32], format[32], tmp[32];
                int n = sscanf(buf, "%s %s %s", cmd, format, tmp);
                if (n == 2 && strcmp(cmd, "GET_TIME")==0 ){
                    time_t rawtime;
                    struct tm *timeinfo;
                    char date[32];

                    // Lấy thời gian hiện tại
                    time(&rawtime);
                    timeinfo = localtime(&rawtime);
                    if (strcmp(format, "dd/mm/yyyy") == 0){   
                        strftime(date, sizeof(date), "%d/%m/%Y", timeinfo);
                    }
                    else if (strcmp(format, "dd/mm/yy") == 0){
                        strftime(date, sizeof(date), "%d/%m/%y", timeinfo);
                    }
                    else if (strcmp(format, "mm/dd/yyyy") == 0) {
                         strftime(date, sizeof(date), "%m/%d/%Y", timeinfo);
                    }
                    else if (strcmp(format, "mm/dd/yy") == 0){
                         strftime(date, sizeof(date), "%m/%d/%y", timeinfo);
                    }
                    else {
                        strcpy(date, "Sai cu phap. GET_TIME [format]");
                    }

                    
                      send(client, date, strlen(date), 0);

                }
                else{
                    //trả lại dữ liệu cho client
                    char *msg = "Sai cu phap. GET_TIME [format]";
                     send(client, msg, strlen(msg), 0);
                }
              
               
                
                //dong ket noi 
                close(client);
                


            }
        }
    }
    wait(NULL);//getchar () ; killpg(0, SIGKILL);
    return 0;
}