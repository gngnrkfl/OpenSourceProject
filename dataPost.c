#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include "/usr/include/mysql/mysql.h"  //mysql include

void insertdatabase(MYSQL *conn, char* name, char* value, char* time){
  MYSQL_RES *res; //mysql�� ��� ������ �����ϴ� ����
  MYSQL_ROW row;  //mysql������ �ϳ��� �����ϴ� ����
  int s_number = 1;  //sensor ���̺��� �����ϴ� ����
  char sensor[MAXLINE] = "sensor";  //sensor ���̺� �տ� ���� ���ڿ�
  char s_num[5];  //s_number�� ���� �迭
  char query[MAXLINE];  //mysql_query�� sprintf�� �̿��ϱ����� �迭
  int sec = 1;  //���� ���ʷ� �ִ��� �ƴ��� �����ϱ� ���� ����
  
  if(mysql_query(conn, "SELECT * FROM sensorList")){  //sensorList ���̺� ����
    mysql_query(conn, "CREATE TABLE sensorList (name varchar(80) not null, id int(2) not null AUTO_INCREMENT, \
        cnt int(2) not null, ave float not null, max float not null, PRIMARY KEY (id))AUTO_INCREMENT = 1;");
    mysql_query(conn, "SELECT * FROM sensorList");
  }
  
  res = mysql_store_result(conn); //mysql�� ���� ����� �����Ѵ�.
  
  while((row = mysql_fetch_row(res))){ //���� ����� ���پ� �ҷ��� �ܾ� �ϳ��ϳ��� ������.   //���� ������ 0 ������ �����Ͱ�
    if(!strcasecmp(row[0], name)){  //ù �ܾ name�� ���� ���
      sec = 0;
      break;
    }
    s_number++;
  }
  
  sprintf(s_num, "%d", s_number);
  strcat(sensor, s_num);  //sensor ���ڿ��� s_number�� ����
  
  if(sec == 0){
    sprintf(query, "INSERT INTO %s VALUES('%s', %s, NULL)", sensor, time, value);  //sensorx ���̺� ���� ����
    mysql_query(conn, query);
    
    sprintf(query, "UPDATE sensorList set cnt = cnt + 1 WHERE name = '%s'", name);  //sensorList ���̺��� cnt ���� 1 �ø�
    mysql_query(conn, query);
    sprintf(query, "UPDATE sensorList SET ave = (SELECT AVG(value) FROM %s) WHERE name = '%s'", sensor, name); //sensorList ���̺��� ave�� ��հ� ����
    mysql_query(conn, query);
    sprintf(query, "UPDATE sensorList SET max = IF(max < %s, %s, max) WHERE name = '%s'", value, value, name); //sensorList ���̺��� max�� �ִ밪 ����
    mysql_query(conn, query);
  }
  else{ //sec == 1 ���ʷ� ���� ������� ���
    sprintf(query, "CREATE TABLE %s (time varchar(15) not null, value float not null, idx int(2) not null AUTO_INCREMENT, \
        PRIMARY KEY (idx))AUTO_INCREMENT = 1;", sensor); //sensorx ���̺� ����
    mysql_query(conn, query);
    
    sprintf(query, "INSERT INTO %s VALUES('%s', %s, null)", sensor, time, value); //sensorx ���̺� ���� ����
    mysql_query(conn, query);
    
    sprintf(query, "INSERT INTO sensorList VALUES('%s', id, 1, %s, %s)", name, value, value);  //sensorList�� ���� ����
    mysql_query(conn, query);
  }
  printf("200 OK\n");  //��� ������ �����ͺ��̽��� �����ϰ� ���
}

void initdb(void){
  char buf[MAXBUF];
  char len[MAXBUF];
  int lens;
  
  sprintf(len, "%s", Getenv("CONTENT_LENGTH"));
  if(!len)
    return (-1);
  lens = atoi(len);
  
  /************database************/
  Read(STDIN_FILENO, buf, lens);  //buf���� �ҷ��´�.
  
  char name[MAXLINE];  
  char value[MAXLINE];
  char time[MAXLINE];
  
  MYSQL *conn = mysql_init(NULL);  //mysql �ʱ�ȭ
  char password[MAXLINE] = "1234";  //mysql ��й�ȣ
  
  if (mysql_real_connect(conn,"127.0.0.1", "root", password, NULL , 0, NULL, 0) == NULL){  //mysql ����
    mysql_error(conn);
  }
    
  if (mysql_query(conn, "USE PROJECT")){  //mysql�ܼ�â�� ���� ��ɾ �Է��ϴ� �Լ�, �����ϸ� TRUE �����ϸ� FALSE ��ȯ
    mysql_query(conn, "CREATE DATABASE PROJECT");  //�����ͺ��̽� ����
    mysql_query(conn, "USE PROJECT");
  }
  
  strtok(buf, "="); 
  sprintf(name, "%s", strtok(NULL, "&"));  //strtok�� �̿��Ͽ� name ����
  
  strtok(NULL, "=");
  sprintf(time, "%s", strtok(NULL, "&"));  //strtok�� �̿��Ͽ� time ����

  strtok(NULL, "=");
  sprintf(value, "%s", strtok(NULL, "&"));  //strtok�� �̿��Ͽ� value ����

  insertdatabase(conn, name, value, time); //������ ���� �����ͺ��̽��� �ִ� �Լ�
  
  mysql_close(conn);
  
}

int main(int argc, char *argv[])
{
  initdb();
  return(0);
}
