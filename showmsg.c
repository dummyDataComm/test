/*--------- showmsg.c: Show message queue details ----*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>

/*--- Function prototypes -----------------*/
typedef struct {
    long mtype;
    int command;
    double data_size;
  } msgbuf;

typedef struct{
    long mtype;
    char mtext[100];
  } my_msg;
void mqstat_print (key_t mkey, int mqid, struct msqid_ds *mstat);
int send_message(int msg_qid, my_msg *qbuf);
int read_message(int msg_qid, long type, my_msg *qbuf);

int main (int argc , char *argv[])
{
  key_t mkey;
  int msq_id;
  struct msqid_ds   msq_status;

  long msg_type = 0;

  if (argc != 2)
  {
    fprintf (stderr, "Usage: showmsg keyval\n");
    exit(1);
  }

  my_msg sendMessage;
  my_msg recvMessage;

    /*---- Get message queue identifier ------*/
    mkey = (key_t) atoi(argv[1]);
    if ((msq_id = msgget (mkey, IPC_CREAT)) < 0)
    {
      perror ("msgget failed!");
      exit(2);
    }
    sendMessage.mtype = 1;
    sprintf(sendMessage.mtext, "hello world\n");
    //msg.mtype = 1;
    //msg.command = 10;
    //msg.data_size = 1024;

    if((send_message(msq_id, &sendMessage)) == -1) {
      perror("send mssage");
      exit(1);
    }

    /*--- get status info -----------------*/
    if (msgctl (msq_id, IPC_STAT, &msq_status) < 0) {
      perror("msgctl (get status) failed");
      exit(3);
    }
    
    mqstat_print(mkey, msq_id, &msq_status);

    if (read_message(msq_id, msg_type, &recvMessage) == -1) {
      perror("msgrcv fail");
      exit(4);
    }

    printf("message: %s", recvMessage.mtext);
    // Remove he message queue
    if (msgctl (msq_id, IPC_RMID, 0) < 0)
    {
      perror ("msgctl (remove queue) failed!");
      exit (3);
    }
    exit(0);
}

/*--------- status info print function ---------*/
void mqstat_print (key_t mkey, int mqid, struct msqid_ds *mstat)
{
  /*-- call the library function ctime ----*/
  char *ctime();

  printf ("\nKey %d, msg_qid %d\n\n", mkey, mqid);
  printf ("%d messages on queue\n\n", (int)mstat->msg_qnum);
  printf ("Last send by proc %d at %s\n",
           mstat->msg_lspid, ctime(&(mstat->msg_stime)));
  printf ("Last recv by proc %d at %s\n",
           mstat->msg_lrpid, ctime(&(mstat->msg_rtime)));
}

int send_message(int msg_qid, my_msg *qbuf) {
  int result, length;
  length = sizeof(msgbuf) - sizeof(long);
  if ( (result = msgsnd(msg_qid, qbuf, length, 0)) == -1) {
    return (-1);
  }
  return (result);
}

int read_message(int msg_qid, long type, my_msg *qbuf) {
  int result, length;
  length = sizeof(my_msg) - sizeof(long);
  if ( (result = msgrcv(msg_qid, qbuf, length, type, 0)) == -1) {
    return (-1);
  }
  return (result);
}

