# OpenSourceProject
> 라즈베리파이를 이용한 간단한 IoT 시스템
> 서버, 클라이언트, 라즈베리파이 간의 통신을 통해 읽은 온습도를 출력하는 프로그램

## 개요
- HTTP 프로토콜을 기반으로 통신을 주고받는다.
- IoT 온습도 센서를 통해 온습도를 데이터베이스에 저장하고 특이사항시 푸시알림을 보낸다.

## 개발기간 및 환경
- 기간 : 2021.9.30 ~ 2021.12.4
- OS : Ubuntu(VirtualBox)
- 메인 언어 : C언어
- 태그 : ```Mysql```, ```HTTP```, ```Raspberry-Pi```

## 개발내용
- server.c : 서버파일입니다. HTTP 프로토콜 양식을 받아 처리합니다.
- request.c : 서버에게 요청을 하거나 받는 파일입니다.
- config-ws.txt : 서버의 포트번호, 큐 사이즈, 스레드 개수가 적혀있습니다.
- clientPost.c, clientGet.c : 클라이언트입니다. 다양한 명령어가 들어있습니다.
- dataGet.c, dataPost.c : 데이터베이스 관련 파일입니다. 온습도 센서의 정보를 받거나 보냅니다.
- alarmClient.c, alarmServer.c, alarm.c : 알람기능 관련 파일입니다. 일정 값의 온도나 습도가 넘어가면 서버에 경고를 띄웁니다.
