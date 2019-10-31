#include<bits/stdc++.h>
#include<winsock2.h>
#include<time.h>
#include"frame_system.h"
using namespace std;
using namespace frame_structure;
//定义 
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

void init()//初始化 
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
	cout<<"----------数据链路层----------"<<endl;
	cout<<"上层端口为："<<upport<<endl<<"本地端口为："<<myport<<endl<<"下层端口为："<<downport<<endl<<endl;
	cout<<"正在与物理层建立连接········"<<endl;
	if(sendto(sock,"connect",8,0,(sockaddr*)&downaddr,downlen)!=-1)
	{
		cout<<"与物理层连接成功！"<<endl;
	}
	else cout<<"与物理层连接失败..."<<endl;
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
		cout<<"选择状态：1.接收信息状态  2.发送信息状态"<<endl;
		int stats;
		cin>>stats;
		if(stats==1)
		{
			int sequence=1;
			cout<<"已选择接收信息状态，正在等待接收来自物理层的信息..."<<endl;
			while(1)
			{
				memset(tmp.content,0,sizeof(tmp.content));
				int now=0;
				memset(str,0,sizeof(str));
				recvfrom(sock,str,1000,0,(sockaddr*)&myaddr,&mylen);
				cout<<"正在接收第"<<sequence<<"帧..."<<endl;
				reset_str(buf,str);
				int a=decode_stf(buf,&tmp);
				memset(buf,0,sizeof(buf));
				if(a==-1)
				{
					cout<<"数据有误，正在要求重传..."<<endl;
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
					cout<<"已确认第"<<sequence<<"帧:"<<endl;
					cout<<frame[now].head<<"-"<<frame[now].sequence<<"-"<<frame[now].content<<"-"<<frame[now].checksum<<"-"<<frame[now].tail<<endl;
					memset(str,0,sizeof(str));
					strcpy(str,"0111111011111011101111110");
					sendto(sock,str,strlen(str),0,(sockaddr*)&downaddr,downlen);
					cout<<"本次接收信息完成..."<<endl;
					decode_ftb(&tmp,bit);
					now=0;
					memset(str,0,sizeof(str));
					while(bit[now].stats)
					{
						strcat(str,bit[now].content);
						now++;
					}
					sendto(sock,str,strlen(str),0,(sockaddr*)&upaddr,uplen);
					cout<<"信息已全部发送到网络层，本次传输结束..."<<endl;
					cout<<"输入回车以进入下一次选择状态..."<<endl; 
					getchar();
					continue;
				}
				else
				{
					frame[a-1].stats=true;
					strcpy(frame[a-1].head,tmp.head);strcpy(frame[a-1].sequence,tmp.sequence);
					strcpy(frame[a-1].content,tmp.content);strcpy(frame[a-1].checksum,tmp.checksum);
					strcpy(frame[a-1].tail,tmp.tail);
					cout<<"已确认第"<<sequence<<"帧:"<<endl;
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
			cout<<"已选择发送信息状态，正在等待接收来自网络层的信息..."<<endl;
			if(recvfrom(sock,str,1000,0,(sockaddr*)&myaddr,&mylen)!=-1)
			{
				int now=0;
				int sequence=1;
				cout<<"已接收到来自网络层的信息..."<<endl;
				code_btf(str,frame);
				while(frame[now].stats)
				{
					cout<<"正在发送第"<<sequence<<"帧:"<<endl;
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
							cout<<"已收到第"<<sequence<<"帧的确认信息..."<<endl;
							now++;
							sequence++;
						}
						else
						{
							cout<<"已收到第"<<sequence<<"帧的重传信息..."<<endl;
						}
					}
				}
				strcpy(str,"11111111");
				sendto(sock,str,strlen(str),0,(sockaddr*)&upaddr,uplen);
				cout<<"已发送确认信息至传输层..."<<endl;
				cout<<"本次信息传输完成！"<<endl; 
			}
			else
			{
				cout<<"接收网络层信息失败！"<<endl;
			}
		}
		else
		{
			cout<<"请输入数字1或者数字2来确定一种状态！"<<endl;
		}
		
	}
	closesocket(sock);
	WSACleanup();
	return 0;	
}
