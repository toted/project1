#include<bits/stdc++.h>
#include<winsock2.h>
#include"frame_system.h"
using namespace std;
using namespace frame_structure;
//���� 
WSADATA wsaData;
SOCKET sock;
sockaddr_in myaddr,downaddr;
char str[1024];
Bits bit[200];
Frames frame[100];
int myport=6666,downport=4100;
int mylen,downlen;

void init() //��ʼ��
{
	WSAStartup(MAKEWORD(2,2),&wsaData);
	sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
	memset(str,0,sizeof(str));
	memset(&myaddr,0,sizeof(myaddr));
	memset(&downaddr,0,sizeof(downaddr));
	myaddr.sin_family=PF_INET;
	myaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	myaddr.sin_port=htons(myport);
	bind(sock,(SOCKADDR*)&myaddr,sizeof(SOCKADDR));
	downaddr.sin_family=PF_INET;
	downaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	downaddr.sin_port=htons(downport);
	mylen=sizeof(myaddr),downlen=sizeof(downaddr);
	cout<<"----------�����----------"<<endl;
	cout<<"���ض˿�Ϊ��"<<myport<<endl<<"�²�˿�Ϊ��"<<downport<<endl<<endl;
} 

int main()
{
	init();
	while(1)
	{
		cout<<"ѡ��״̬��1.������Ϣ״̬  2.������Ϣ״̬"<<endl;
		int stats;
		cin>>stats;
		if(stats==1)
		{
			cout<<"��ѡ�������Ϣ״̬�����ڵȴ���������������·�����Ϣ..."<<endl;
			memset(str,0,sizeof(str));
			if(recvfrom(sock,str,1000,0,(sockaddr*)&myaddr,&mylen)!=-1)
			{
				cout<<"�ѽ��ܵ�������·�����Ϣ"<<endl;
				cout<<"ԭ������ϢΪ��"<<str<<endl;
				cout<<"ת����Ϊ��";
				decode_bts(str);
				cout<<str<<endl;
			}
		}
		else if(stats==2)
		{
			int now=0;
			cout<<"��ѡ������Ϣ״̬�����ڵȴ�������Ϣ��������·��..."<<endl;
			cout<<"������Ҫ���͵���Ϣ:(�����ģ��ɰ����ո�)";
			getchar();
			gets(str);
			code_stb(str,bit);
			now=0;
			memset(str,0,sizeof(str));
			while(bit[now].stats)
			{
				strcat(str,bit[now].content);
				now++;
			}
			cout<<endl<<"ת��Ϊ��������Ϊ��"<<str<<endl;
			sendto(sock,str,strlen(str),0,(sockaddr*)&downaddr,downlen);
			cout<<"�ѷ��ͳɹ������ڵȴ��Է����ص�ȷ����Ϣ..."<<endl;
			memset(str,0,sizeof(str));
			if(recvfrom(sock,str,1000,0,(sockaddr*)&myaddr,&mylen)!=-1)
			{
				cout<<"�ѽ��յ��Է����ܳɹ����ص�ȷ����Ϣ!"<<endl;
				cout<<"������Ϣ������ɣ�"<<endl;
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
