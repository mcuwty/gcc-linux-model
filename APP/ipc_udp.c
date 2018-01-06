/****************************************************************************************
 * 文件名  ：ipc_udp.c
 * 描述    ：udp进程间通信，用于linux上与本地程序进行通信
 * 开发平台：linux
 * 开发工具：gcc-4.8.4/arm-linux-gnueabihf-gcc-4.9.4
 ***************************************************************************************/
#include "ipc_udp.h"

struct sockaddr_in ipc_udp_from;
int ipc_udp_net_fd  = -1;			//进程间通信的socket
pthread_t ipc_pthread;				//进程间通信线程

void *ipc_udp_pthread(void* pro)
{
	pro = pro;

	int fd = socket(AF_INET, SOCK_DGRAM, 0);    
	if(fd==-1)    
	{    
	    printf("Init ipc_udp sockfd error !\n");
		exit(1);
	}  

	struct sockaddr_in addr;    
	addr.sin_family = AF_INET;    
	addr.sin_port = htons(8080);    
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");  
	
	int r;    
	r = bind(fd,(struct sockaddr*)&addr,sizeof(addr));    
	if(r==-1)    
	{    
	    perror("Bind ipc port error !\n");    
	    close(fd);    
	    exit(1);    
	}
	printf("Bind ipc address successful!\n");
				
	ipc_udp_net_fd = fd;		//记录该句柄
  
	fd_set read_fds;
	struct timeval timeout = {0,0};
	unsigned char net_buf[1024];
    
	socklen_t len;    
	len = sizeof(ipc_udp_from);  

	while(1)    
	{    
	    if (ipc_udp_net_fd != -1)
		{
			FD_ZERO(&read_fds);
			FD_SET(ipc_udp_net_fd, &read_fds);
			timeout.tv_sec = TIME_OUT_VOLUE;

			int ret = select(ipc_udp_net_fd + 1, &read_fds, NULL, NULL, &timeout);
			switch ( ret )
			{
				case -1:						//错误
					printf("Network socket select error !\n");
					break;
				case 0:							//超时
					printf("Network socket select timeout !\n");
					break;
				default :						//有可读的数据
				{
					if (FD_ISSET(ipc_udp_net_fd, &read_fds)) 
					{
						r = recvfrom(ipc_udp_net_fd,net_buf,sizeof(net_buf)-1,0,(struct sockaddr*)&ipc_udp_from,&len);    
						if(r>0)    
						{
							int i;
							for(i=0;i<r;i++)
							{
								putchar(net_buf[i]);
							}
						}
					}
					break;
				}
			}
		}
		else
			usleep(100*1000);					//非阻塞的线程必须要加休眠，否则会占满CPU	
	}
}

void Init_ipc_udp_pthread(void)
{
	int ret;

	//创建udp进程间通信线程
	ret=pthread_create(&ipc_pthread,NULL,ipc_udp_pthread,NULL);
	if(ret!=0)
	{
		printf ("Create ipc_udp_pthread pthread error!\n");
		exit (1);
	}
	printf("Init ipc_udp_pthread pthread ok !\n");
}
