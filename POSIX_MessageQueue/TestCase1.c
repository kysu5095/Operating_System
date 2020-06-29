#include "TestCase1.h"

void* Tc1ThreadSender(void* param)
{
	pmqd_t mq,pmq;
	char msg[MAX_MSG_LEN];
	int i;

	mq=pmq_open("mq2",O_CREAT,O_RDWR,NULL);
	pmq=pmq_open("mq1",O_CREAT,O_RDWR,NULL);

	for(i=1;i<=5;i++){
		memset(msg,0,MAX_MSG_LEN);
		sprintf(msg,"%dth message",i);
		pmq_send(mq,msg,MAX_MSG_LEN,0);
	}

	memset(msg,0,MAX_MSG_LEN);
	sprintf(msg,"send complete");
	pmq_send(pmq,msg,MAX_MSG_LEN,0);

	pmq_close(mq);
	pmq_close(pmq);
	return NULL;
}

void* Tc1ThreadReceiver(void* param)
{
	pmqd_t mq,pmq;
	char msg[MAX_MSG_LEN];
	int i;
	int msg_prio;

	mq=pmq_open("mq2",O_CREAT,O_RDWR,NULL);
	pmq=pmq_open("mq1",O_CREAT,O_RDWR,NULL);

	for(i=1;i<=5;i++){
		memset(msg,0,MAX_MSG_LEN);
		pmq_receive(mq,msg,MAX_MSG_LEN,&msg_prio);
		printf("prio: %d | %s\n",msg_prio,msg);
	}

	memset(msg,0,MAX_MSG_LEN);
	sprintf(msg,"receive complete");
	pmq_send(pmq,msg,MAX_MSG_LEN,0);

	pmq_close(mq);
	pmq_close(pmq);
	return NULL;
}

void TestCase1(void)
{
	thread_t tid[2];
	pmqd_t mq;
	char msg[MAX_MSG_LEN];
	unsigned int i;

	mq=pmq_open("mq1",O_CREAT,O_RDWR,NULL);

	thread_create(&tid[0], NULL, 1, (void*)Tc1ThreadSender,0);

	memset(msg,0,MAX_MSG_LEN);
	pmq_receive(mq,msg,MAX_MSG_LEN,&i);
	printf("%s\n",msg);

	thread_create(&tid[1], NULL, 1, (void*)Tc1ThreadReceiver,0);

	memset(msg,0,MAX_MSG_LEN);
	pmq_receive(mq,msg,MAX_MSG_LEN,&i);
	printf("%s\n",msg);

	pmq_close(mq);
	return ;
}
