#include<bits/stdc++.h>
#include<winsock2.h>
#include"frame_system.h"
using namespace std;
using namespace frame_structure;
//定义 
WSADATA wsaData;
SOCKET sock;
sockaddr_in myaddr,downaddr;
char str[1024];
Bits bit[200];
Frames frame[100];
int myport=6666,downport=4100;
int mylen,downlen;

void init() //初始化
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
	cout<<"----------网络层----------"<<endl;
	cout<<"本地端口为："<<myport<<endl<<"下层端口为："<<downport<<endl<<endl;
} 

int main()
{
	init();
	while(1)
	{
		cout<<"选择状态：1.接收信息状态  2.发送信息状态"<<endl;
		int stats;
		cin>>stats;
		if(stats==1)
		{
			cout<<"已选择接收信息状态，正在等待接收来自数据链路层的信息..."<<endl;
			memset(str,0,sizeof(str));
			if(recvfrom(sock,str,1000,0,(sockaddr*)&myaddr,&mylen)!=-1)
			{
				cout<<"已接受到数据链路层的信息"<<endl;
				cout<<"原比特信息为："<<str<<endl;
				cout<<"转化后为：";
				decode_bts(str);
				cout<<str<<endl;
			}
		}
		else if(stats==2)
		{
			int now=0;
			cout<<"已选择发送信息状态，正在等待发送信息至数据链路层..."<<endl;
			cout<<"请输入要发送的信息:(除中文，可包含空格)";
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
			cout<<endl<<"转化为比特流后为："<<str<<endl;
			sendto(sock,str,strlen(str),0,(sockaddr*)&downaddr,downlen);
			cout<<"已发送成功，正在等待对方返回的确认信息..."<<endl;
			memset(str,0,sizeof(str));
			if(recvfrom(sock,str,1000,0,(sockaddr*)&myaddr,&mylen)!=-1)
			{
				cout<<"已接收到对方接受成功返回的确认信息!"<<endl;
				cout<<"本次信息传输完成！"<<endl;
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
