#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define DEFAULT_PROTOCOL 0

// 사물함 구조체 정의
struct locker {
    int id;                     // 사물함 ID
    int empty;                  // 비어있는지 여부
    char pwd[10];               // 비밀번호
    char content[10][300];      // 최대 10개의 내용물
    int content_count;          // 현재 내용물 개수
};

// 함수 선언
void initializeLockers(struct locker *lockers, int k);
void displayUpdatedLockers(struct locker *lockers, int k);
void handleClient(int client_socket, struct locker *lockers, int k, int pipe_fd[2]);

int main() {
    char str[100];              // 사용자 입력을 저장할 버퍼
    int client_socket;          // 클라이언트 소켓
    int server_socket;          // 서버 소켓
    int num_lockers;            // 사물함 개수
    int client_len;             // 클라이언트 주소 길이
    int status;                 // 자식 프로세스 상태
    int child_pid;              // 자식 프로세스 ID

    struct locker *lockers;     // 사물함 배열

    struct sockaddr_un server_unix_addr, client_unix_addr;

    // 소켓 생성
    server_socket = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (server_socket < 0) {
        
        exit(1);
    }

    // 서버 주소 설정
    server_unix_addr.sun_family = AF_UNIX;
    strcpy(server_unix_addr.sun_path, "manager");
    unlink("manager");

    // 소켓 바인드
    if (bind(server_socket, (struct sockaddr *)&server_unix_addr, sizeof(server_unix_addr)) < 0) {
       
        exit(1);
    }

    // 사물함 개수 입력
    printf("사물함 개수를 입력하세요 : ");
    scanf("%s", str);

    num_lockers = atoi(str);

    

    // 사물함 메모리 할당 및 초기화
    lockers = (struct locker *)malloc((num_lockers + 1) * sizeof(struct locker));
    initializeLockers(lockers, num_lockers);

    // FIFO 파일 생성
    int file_descriptor;
    mkfifo("index", 0666);
    file_descriptor = open("index", O_WRONLY);

    // 소켓 리슨
    listen(server_socket, 5);

    while (1) {
        int pipe_fd[2];
        pipe(pipe_fd);

        // 클라이언트 연결 수락
        client_len = sizeof(client_unix_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_unix_addr, &client_len);    
        if (client_socket == -1) {
            
            continue;
        }

        // 자식 프로세스 생성
        if (fork() == 0) {
            close(pipe_fd[0]);

            // 사물함 개수 파일에 쓰기
            write(file_descriptor, str, 100);

            // 클라이언트 처리 함수 호출
            handleClient(client_socket, lockers, num_lockers, pipe_fd);
            exit(0);
        }

        // 부모 프로세스: 자식 프로세스 종료 대기
        child_pid = wait(&status);

        close(pipe_fd[1]);

        // 파이프에서 업데이트된 사물함 상태 읽기
        for (int i = 1; i <= num_lockers; i++) {
            read(pipe_fd[0], &lockers[i], sizeof(struct locker));
        }

        close(client_socket);
    }

    return 0;
}

// 사물함 초기화 함수
void initializeLockers(struct locker *lockers, int k) {
    printf("\n┌───────────────┬───────────────┬───────────────┬────────────────────────────────────────────┐\n");
    printf("│  Locker Number│    Status     │   Password    │                   Content                  │\n");
    printf("├───────────────┼───────────────┼───────────────┼────────────────────────────────────────────┤\n");

    for (int i = 1; i <= k; i++) {
        lockers[i].id = i;
        lockers[i].empty = 1;
        strcpy(lockers[i].pwd, "0000");
        lockers[i].content_count = 0;
        for (int j = 0; j < 10; j++) {
            strcpy(lockers[i].content[j], "");
        }
        printf("│      %2d       │    Empty      │  %-10s  │  %-40s  │\n", i, lockers[i].pwd, lockers[i].content[0]);
    }
    printf("└───────────────┴───────────────┴───────────────┴────────────────────────────────────────────┘\n");
}

// 클라이언트 처리 함수
void handleClient(int client_socket, struct locker *lockers, int k, int pipe_fd[2]) {
    char mode, empty_locker, available;
    char user_id[100], rea
}