#include<bits/stdc++.h>
#include<winsock2.h>
#include<time.h>
#include"frame_system.h"
using namespace std;
using namespace frame_structure;
//���� 
WSADATA wsaData;
SOCKET sock;
sockaddr_in myaddr,downaddr,upaddr;
char str[1024];
char buf[1024]; 
Bits bit[200];
Frames frame[100];
Frames tmp;
Acks ack;
int myport=4100,downport=11100,upport=6666;
int mylen,downlen,uplen;

void init()//��ʼ�� 
{
	WSAStartup(MAKEWORD(2,2),&wsaData);
	sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	memset(str,0,sizeof(str));
	memset(&myaddr,0,sizeof(myaddr));
	memset(&downaddr,0,sizeof(downaddr));
	myaddr.sin_family=AF_INET;
	myaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	myaddr.sin_port=htons(myport);
	bind(sock,(SOCKADDR*)&myaddr,sizeof(SOCKADDR));
	downaddr.sin_family=PF_INET;
	downaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	downaddr.sin_port=htons(downport);
	upaddr.sin_family=PF_INET;
	upaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	upaddr.sin_port=htons(upport);
	mylen=sizeof(myaddr),downlen=sizeof(downaddr),uplen=sizeof(upaddr);
	cout<<"----------������·��----------"<<endl;
	cout<<"�ϲ�˿�Ϊ��"<<upport<<endl<<"���ض˿�Ϊ��"<<myport<<endl<<"�²�˿�Ϊ��"<<downport<<endl<<endl;
	cout<<"����������㽨�����ӡ���������������"<<endl;
	if(sendto(sock,"connect",8,0,(sockaddr*)&downaddr,downlen)!=-1)
	{
		cout<<"����������ӳɹ���"<<endl;
	}
	else cout<<"�����������ʧ��..."<<endl;
	recvfrom(sock,str,1000,0,(sockaddr*)&myaddr,&mylen); 
}

void clear_all()
{
	int now=0;
	while(frame[now].stats)
	{
		frame[now].stats=false;
		memset(frame[now].content,0,sizeof(frame[now].content));
		now++;
	}
	now=0;
	while(bit[now].stats)
	{
		bit[now].stats=false;
		now++;
	}
	memset(str,0,sizeof(str));
	memset(buf,0,sizeof(buf));
}

int main()
{
	init();
	while(1)
	{
		clear_all();
		cout<<"ѡ��״̬��1.������Ϣ״̬  2.������Ϣ״̬"<<endl;
		int stats;
		cin>>stats;
		if(stats==1)
		{
			int sequence=1;
			cout<<"��ѡ�������Ϣ״̬�����ڵȴ�����������������Ϣ..."<<endl;
			while(1)
			{
				memset(tmp.content,0,sizeof(tmp.content));
				int now=0;
				memset(str,0,sizeof(str));
				recvfrom(sock,str,1000,0,(sockaddr*)&myaddr,&mylen);
				cout<<"���ڽ��յ�"<<sequence<<"֡..."<<endl;
				reset_str(buf,str);
				int a=decode_stf(buf,&tmp);
				memset(buf,0,sizeof(buf));
				if(a==-1)
				{
					cout<<"������������Ҫ���ش�..."<<endl;
					memset(str,0,sizeof(str));
					strcpy(str,"011111100000000001111110");
					sendto(sock,str,strlen(str),0,(sockaddr*)&downaddr,downlen);
					continue;
				}
				else if(a==0)
				{
					frame[now].stats=true;
					strcpy(frame[now].head,tmp.head);strcpy(frame[now].sequence,tmp.sequence);
					strcpy(frame[now].content,tmp.content);strcpy(frame[now].checksum,tmp.checksum);
					strcpy(frame[now].tail,tmp.tail);
					cout<<"��ȷ�ϵ�"<<sequence<<"֡:"<<endl;
					cout<<frame[now].head<<"-"<<frame[now].sequence<<"-"<<frame[now].content<<"-"<<frame[now].checksum<<"-"<<frame[now].tail<<endl;
					memset(str,0,sizeof(str));
					strcpy(str,"0111111011111011101111110");
					sendto(sock,str,strlen(str),0,(sockaddr*)&downaddr,downlen);
					cout<<"���ν�����Ϣ���..."<<endl;
					decode_ftb(&tmp,bit);
					now=0;
					memset(str,0,sizeof(str));
					while(bit[now].stats)
					{
						strcat(str,bit[now].content);
						now++;
					}
					sendto(sock,str,strlen(str),0,(sockaddr*)&upaddr,uplen);
					cout<<"��Ϣ��ȫ�����͵�����㣬���δ������..."<<endl;
					cout<<"����س��Խ�����һ��ѡ��״̬..."<<endl; 
					getchar();
					continue;
				}
				else
				{
					frame[a-1].stats=true;
					strcpy(frame[a-1].head,tmp.head);strcpy(frame[a-1].sequence,tmp.sequence);
					strcpy(frame[a-1].content,tmp.content);strcpy(frame[a-1].checksum,tmp.checksum);
					strcpy(frame[a-1].tail,tmp.tail);
					cout<<"��ȷ�ϵ�"<<sequence<<"֡:"<<endl;
					cout<<frame[a-1].head<<"-"<<frame[a-1].sequence<<"-"<<frame[a-1].content<<"-"<<frame[a-1].checksum<<"-"<<frame[a-1].tail<<endl;
					decode_ftb(&tmp,bit);
					memset(str,0,sizeof(str));
					strcpy(str,"011111101111101101111110");
					sendto(sock,str,strlen(str),0,(sockaddr*)&downaddr,downlen);
					sequence++; 
				}
			} 
		}
		else if(stats==2)
		{
			cout<<"��ѡ������Ϣ״̬�����ڵȴ�����������������Ϣ..."<<endl;
			if(recvfrom(sock,str,1000,0,(sockaddr*)&myaddr,&mylen)!=-1)
			{
				int now=0;
				int sequence=1;
				cout<<"�ѽ��յ�������������Ϣ..."<<endl;
				code_btf(str,frame);
				while(frame[now].stats)
				{
					cout<<"���ڷ��͵�"<<sequence<<"֡:"<<endl;
					cout<<frame[now].head<<"-"<<frame[now].sequence<<"-"<<frame[now].content<<"-"<<frame[now].checksum<<"-"<<frame[now].tail<<endl;
					Sleep(0.5);
					memset(str,0,sizeof(str));
					memset(buf,0,sizeof(str));
					strcat(str,frame[now].sequence);strcat(str,frame[now].content);strcat(str,frame[now].checksum);
					change_str(buf,str);
					memset(str,0,sizeof(str));
					strcpy(str,"01111110");strcat(str,buf);strcat(str,"01111110");
					sendto(sock,str,strlen(str),0,(sockaddr*)&downaddr,downlen);
					memset(str,0,sizeof(str));
					if(recvfrom(sock,str,1000,0,(sockaddr*)&myaddr,&mylen)!=-1)
					{
						if(decode_ack(str))
						{
							cout<<"���յ���"<<sequence<<"֡��ȷ����Ϣ..."<<endl;
							now++;
							sequence++;
						}
						else
						{
							cout<<"���յ���"<<sequence<<"֡���ش���Ϣ..."<<endl;
						}
					}
				}
				strcpy(str,"11111111");
				sendto(sock,str,strlen(str),0,(sockaddr*)&upaddr,uplen);
				cout<<"�ѷ���ȷ����Ϣ�������..."<<endl;
				cout<<"������Ϣ������ɣ�"<<endl; 
			}
			else
			{
				cout<<"�����������Ϣʧ�ܣ�"<<endl;
			}
		}
		else
		{
			cout<<"����������1��������2��ȷ��һ��״̬��"<<endl;
		}
		
	}
	closesocket(sock);
	WSACleanup();
	return 0;	
}
