#include<bits/stdc++.h>
using namespace std;
const int maxn=4;

namespace frame_structure{
	
	int two[]={1,2,4,8,16,32,64,128,256};
	char ack1[]="11111011";
	char ack0[]="00000000";
	
	struct Bits{ //比特结构 
		char content[8+1];
		bool stats;
	}; 
	
	struct Frames{ //帧结构 
		char head[8+1];
		char sequence[8+1]; 
		char content[maxn*8+1];
		char checksum[8+1];
		char tail[8+1];
		bool stats;
	};
	
	struct Acks{ //应答机制 
		char head[8+1];
		char content[8+1];
		char tail[8+1];
	};
	
	int decode_stf(char *str,Frames *frame)//将物理层以字符串形式传过来的帧格式转化为帧，并判断是否正确
	{//返回-1表示错误，其他返回表示序号，其中序号0表示结束帧 
		int len=(strlen(str)-16)/8;
		int checksum=0;
		int sequence=0;
		int head=0;
		strcpy(frame->head,"01111110");//帧头 
		strcpy(frame->tail,"01111110"); //帧尾 
		for(int i=0;i<8;i++)//序号 
		{
			frame->sequence[i]=str[head];
			sequence+=(str[head]-'0')*two[7-i];
			checksum+=(str[head]-'0')*two[7-i];
			head++;
		}
		frame->sequence[8]='\0';
		for(int i=0;i<len;i++)//数据内容
		{
			for(int j=0;j<8;j++)
			{
				frame->content[8*i+j]=str[head];
				checksum+=(str[head]-'0')*two[7-j];
				head++;
			}
		} 
		frame->content[maxn*8]='\0';
		for(int i=0;i<8;i++)
		{
			frame->checksum[i]=str[head];
			if((str[head]-'0')^((checksum>>7)%2)!=1)return -1;
			checksum<<=1;
			head++;
		}
		frame->checksum[8]='\0';
		return sequence;
	}
	
	
	void code_stb(char *str,Bits *bit)//将网络层的字符串信息转化为比特信息，转化的方式为ASCII码 
	{ 
		int len=strlen(str);
		for(int i=0;i<len;i++)
		{
			int value=str[i];
			bit[i].stats=true;
			for(int j=0;j<8;j++)
			{
				bit[i].content[j]=(value>>7)%2+'0';
				value<<=1;
			}
			bit[i].content[8]='\0';
		}
	}
	
	
	void decode_bts(char *str)//将比特流转化为字符串信息，其中str携带原始比特流 
	{
		int now=0;
		int len=strlen(str);
		for(int i=0;i<len/8;i++)
		{
			int value=0;
			for(int j=0;j<8;j++)
			{
				value+=(str[j+i*8]-'0')*two[7-j];
			}
			str[now++]=value;
		}
		str[now]='\0';
	}
	
	
	void code_btf(char *str,Frames *frame)//将比特流转化为帧 
	{
		int len=strlen(str);
		int now=0;
		len/=8;
		if(len%maxn==0)len=len/maxn-1;
		else len/=maxn;
		for(int i=0;i<len;i++)
		{
			frame[i].stats=true;
			int sequence=1;
			int checksum=sequence;
			strcpy(frame[i].head,"01111110");//帧头 
			strcpy(frame[i].tail,"01111110");//帧尾 
			int n=sequence;
			for(int j=0;j<8;j++)//序号 
			{
				frame[i].sequence[j]=(n>>7)%2+'0';
				n<<=1;
			}
			frame[i].sequence[8]='\0';
			for(int j=0;j<maxn;j++)//内容 
			{
				for(int k=0;k<8;k++)
				{
					frame[i].content[j*8+k]=str[now];
					checksum+=two[7-k]*(str[now]-'0');
					now++;
				}
			}
			frame[i].content[maxn*8]='\0';
			for(int j=0;j<8;j++)//校验码 
			{
				frame[i].checksum[j]=(checksum>>7)%2==1?0+'0':1+'0';
				checksum<<=1;
			}
			frame[i].checksum[8]='\0';
		} 
		int a=strlen(str)/8;
		if(a%maxn==0)a=maxn;
		else a-=a/maxn*maxn;
		//最后一帧
		int checksum=0;
		frame[len].stats=true;
		strcpy(frame[len].head,"01111110");
		strcpy(frame[len].tail,"01111110");
		for(int i=0;i<8;i++)
		{
			frame[len].sequence[i]='0';
		} 
		frame[len].sequence[8]='\0';
		for(int i=0;i<a;i++)
		{
			int value=str[now];
			for(int j=0;j<8;j++)
			{
				frame[len].content[i*8+j]=str[now];
				checksum+=two[7-j]*(str[now]-'0');
				now++;
			}
		}
		frame[len].content[(a-1)*8+8]='\0';
		for(int i=0;i<8;i++)
		{
			frame[len].checksum[i]=((checksum>>7)%2)==1?0+'0':1+'0';
			checksum<<=1;
		}
		frame[len].checksum[8]='\0';
	}	
	
	
	bool decode_ack(char *str)
	{
		int head=0;//找帧头 
		while(str[head]=='1')head++;
		while(str[head++]=='0')
		{
			int one=0;
			while(str[head]=='1')one++,head++;
			if(one==6)
			{
				head++;
				break;
			}
		}
		int tail=head;//找帧尾 
		while(str[tail]=='1')tail++;
		while(str[tail++]=='0')
		{
			int one=0;
			while(str[tail]=='1')one++,tail++;
			if(one==6)
			{
				tail-=8;
				break;
			}
		}
		int one=0,zero=0;
		for(int i=head;i<=tail;i++)
		{
			if(str[i]=='1')one++;
			else zero++;
		}
		return one>zero;
	}
	
	void reset_str(char *buf,char *str)
	{
		int head=0;//找帧头 
		while(str[head]=='1')head++;
		while(str[head++]=='0')
		{
			int one=0;
			while(str[head]=='1')one++,head++;
			if(one==6)
			{
				head++;
				break;
			}
		}
		int tail=head;//找帧尾 
		while(str[tail]=='1')tail++;
		while(str[tail++]=='0')
		{
			int one=0;
			while(str[tail]=='1')one++,tail++;
			if(one==6)
			{
				tail-=8;
				break;
			}
		}
		int now=0;
		int one=0;
		for(int i=head;i<=tail;i++)
		{
			if(one==5)i++,one=0;
			if(str[i]=='1')one++;
			else one=0;
			buf[now++]=str[i];
		}
	}
	
	void change_str(char *buf,char *str)
	{
		int len=strlen(str);
		int now=0;
		int one=0;
		for(int i=0;i<len;i++)
		{
			if(one==5)buf[now++]='0',one=0;
			if(str[i]=='1')one++;
			else one=0;
			buf[now++]=str[i];
		}
		buf[now]='\0';
	}
	
	void decode_ftb(Frames *frame,Bits *bit)
	{
		int now=0;
		while(bit[now].stats)now++;
		int len=strlen(frame->content);
		for(int i=0;i<len/8;i++)
		{
			bit[now+i].stats=true;
			for(int j=0;j<8;j++)
			{
				bit[now+i].content[j]=frame->content[i*8+j];
			}
			bit[i].content[8]='\0';
		} 
	}
};
