## 서버-클라이언트(server-client) 모델로 동작하는 사물함 관리 시스템을 구현하는 프로젝트

1. 삭제 모드 시작 확인
else if (mode == 'd') : mode가 'd'인 경우 내용물 삭제 모드를 시작함

2.사용자 ID 읽기
read(client_socket, user_id, 100);
locker_id = atoi(user_id);
 클라이언트로부터 사용자 ID를 읽어옵니다. 이 ID는 삭제할 내용물이 있는 사물함을 식별합니다.

3. 사물함이 비어있음을 확인하고 비밀번호 확인
if (!lockers[locker_id].empty) : 사물함이 비어있는지 확인
strcpy(send_password, lockers[locker_id].pwd);  :    사물함의 비밀번호를 send_password에 복사
read(client_socket, received_password, sizeof(received_password)); : 클라이언트로부터 비밀번호를 읽어옴
if (!strcmp(received_password, lockers[locker_id].pwd))  : 클라이언트로부터 받은 비밀번호와 사물함 비밀번호가 일치하는지 확인

4. 삭제할 내용물 읽기
read(client_socket, item_to_delete, sizeof(item_to_delete)); : 클라이언트로부터 삭제할 내용물을 읽어옴

5. 내용물 검색 및 삭제
for (int i = 0; i < lockers[locker_id].content_count; i++) {
    if (strcmp(lockers[locker_id].content[i], item_to_delete) == 0) {
        found = 1;  
사물함의 내용물에서 삭제할 항목을 찾음
 for (int j = i; j < lockers[locker_id].content_count - 1; j++) {
            strcpy(lockers[locker_id].content[j], lockers[locker_id].content[j + 1]);
        }
        lockers[locker_id].content[lockers[locker_id].content_count - 1][0] = '\0'; // 마지막 항목 초기화
        lockers[locker_id].content_count--;
        write(client_socket, "내용물 삭제 완료", sizeof("내용물 삭제 완료"));
        break;
    }
}
항목을 찾으면, 그 항목을 삭제하고 배열을 재정렬
6. 삭제할 내용물이 없는 경우
if (!found) {
    write(client_socket, "해당 내용물이 없습니다", sizeof("해당 내용물이 없습니다"));
}  만약 삭제할 항목을 찾지 못한 경우, 클라이언트에 해당 내용물이 없음을 알림


  
