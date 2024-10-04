#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>

#define DEFAULT_PROTOCOL 0

// 함수 선언
void displayLockers(int client_socket, int k);
void processChatMode(int client_socket);
void processLockerUsage(int client_socket, int k);
void deleteLockerContent(int client_socket, int k);

int main() {
    int client_socket;       // 클라이언트 소켓
    int connect_result;      // 연결 결과
    int file_descriptor;     // 파일 디스크립터
    char locker_number[100]; // 사물함 번호를 저장할 버퍼
    char prompt;             // 사용자 입력
    int k;                   // 사물함 개수

    struct sockaddr_un server_unix_addr;

    // 클라이언트 소켓 생성
    client_socket = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (client_socket < 0) {
        perror("소켓 생성 실패");
        exit(1);
    }

    // 서버 주소 설정
    server_unix_addr.sun_family = AF_UNIX;
    strcpy(server_unix_addr.sun_path, "manager");

    // 서버와 연결 시도
    do {
        connect_result = connect(client_socket, (struct sockaddr *)&server_unix_addr, sizeof(server_unix_addr));
        sleep(1);
    } while (connect_result == -1);

    // FIFO 파일에서 사물함 개수 읽기
    file_descriptor = open("index", O_RDONLY);
    if (file_descriptor < 0) {
        perror("파일 열기 실패");
        exit(1);
    }
    read(file_descriptor, locker_number, 100);
    k = atoi(locker_number); // 사물함 개수를 정수로 변환

    // 사물함 상태 표시
    displayLockers(client_socket, k);

    // 사용자 메뉴 반복
    while (1) {    
        int choice;
        printf("1. 사물함 사용\n");
        printf("2. 사물함 내용물 삭제\n");
        printf("3. 관리자와 대화\n");
        printf("선택하세요: ");
        scanf("%d", &choice);

        // 사용자의 선택에 따라 함수 호출
        if (choice == 1) {
            processLockerUsage(client_socket, k);
        } else if (choice == 2) {
            deleteLockerContent(client_socket, k);
        } else if (choice == 3) {
            processChatMode(client_socket);
        }

        // 계속 여부 확인
        printf("계속 하시려면 'y'를 입력해 주세요: ");
        scanf(" %c", &prompt);

        // 서버에 사용자 입력 전송
        write(client_socket, &prompt, 1);

        // 'N' 입력 시 프로그램 종료
        if (prompt == 'N') {
            break;
        }
    }

    close(client_socket); // 클라이언트 소켓 닫기
    return 0;
}

// 사물함 상태를 표시하는 함수
void displayLockers(int client_socket, int k) {
    char empty_locker; // 사물함 상태를 저장할 변수
    int j; // 상태를 정수로 변환하기 위한 변수

    // 표 헤더 출력
    printf("┌───────────────┬───────────────┐\n");
    printf("│  Locker Number│    Status     │\n");
    printf("├───────────────┼───────────────┤\n");

    // 각 사물함 상태 출력
    for (int i = 1; i <= k; i++) {
        read(client_socket, &empty_locker, sizeof(empty_locker));
        j = empty_locker - '0';
        if (j == 1) {
            printf("│       %2d      │    Empty      │\n", i);
        } else if (j == 0) {
            printf("│       %2d      │  Occupied     │\n", i);
        }
    }
    printf("└───────────────┴───────────────┘\n");
}

// 관리자와 채팅하는 함수
void processChatMode(int client_socket) {
    char send_message[300], receive_message[300];
    char mode = 'c'; // 채팅 모드

    // 채팅 모드 시작 신호 전송
    write(client_socket, &mode, 1);
    printf("\n관리자와의 채팅이 시작됩니다(종료하려면 Q를 입력)\n");

    while (1) {
        // 메시지 입력 및 전송
        printf("Client : ");
        scanf("%[^\n]s", send_message);    
        write(client_socket, send_message, sizeof(send_message));

        // 'Q' 입력 시 채팅 종료
        if (!strcmp(send_message, "Q")) break;

        // 관리자 메시지 수신 및 출력
        read(client_socket, receive_message, sizeof(receive_message));
        printf("Manager : %s\n", receive_message);
    }

    // 관리자와의 채팅이 끝나면 메인 메뉴로 돌아가기 위한 메시지
    printf("\n관리자와의 채팅이 종료..메인 메뉴로 돌아갑니다.\n");
}

// 사물함을 사용하는 함수
void processLockerUsage(int client_socket, int k) {
    char user_id[100], password[10], confirm_password[10];
    char content[300];
    char mode, available;
    int input_amount,
}