/*
 * client.c为客户端代码
*/

#include "config.h"

/*readline函数实现*/
ssize_t readline(int fd, char *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = read(fd, &c,1)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}


int main(int argc , char ** argv)
{
	/*声明套接字和链接服务器地址*/
	int sockfd;
	struct sockaddr_in servaddr; // 这个结构体中详细地定义了相关的端口,地址和IP等. 此外还有很多其他等效测结构体.例如 sockaddr定义了两个内容,第一个是协议类型,一般是AF_XXX, 第二个是14字节的长度. 在实际使用当中,更常见使用其他结构体类型. 例如端口,地址,和剩余的空间.

	/*判断是否为合法输入*/
	if(argc != 2)
	{
		perror("usage:tcpcli <IPaddress>");
		exit(1);
	}//if

	/*(1) 创建套接字*/
	if((sockfd = socket(AF_INET , SOCK_STREAM , 0)) == -1) // 虽然参数比较多,但是一般第一个是固定的,第二是,如果是TCP,使用SOCK_STREAM, 是UDP,使用SOCK_DGRAM. 第三个默认取0即可.
	{
		perror("socket error");
		exit(1);
	}//if

	/*(2) 设置链接服务器地址结构*/
	bzero(&servaddr , sizeof(servaddr)); //置0,清空为0
	servaddr.sin_family = AF_INET; // 结构体中赋值 默认的协议.
	servaddr.sin_port = htons(PORT);  // 结构体中赋值相关的端口.
	if(inet_pton(AF_INET , argv[1] , &servaddr.sin_addr) < 0) // 结构体中赋值相关的ip部分
	{
		printf("inet_pton error for %s\n",argv[1]);
		exit(1);
	}//if

	/*(3) 发送链接服务器请求*/
	if( connect(sockfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0) // 都是结构体,这里进行了相关的转换. 地址大小是很类似的.
	{
		perror("connect error");
		exit(1);
	}//if

	/*(4) 消息处理*/
	char sendline[MAX_LINE] , recvline[MAX_LINE];
	while(fgets(sendline , MAX_LINE , stdin) != NULL)	//从指定的流当中读取相关的信息.三个参数分别是,数组起始地址,数组长度,读取的流. 一般可以从文件或者键盘中进行读取.  比gets有时候更安全.
	{
		write(sockfd , sendline , strlen(sendline));

		if(readline(sockfd , recvline , MAX_LINE) == 0)
		{
			perror("server terminated prematurely");
			exit(1);
		}//if

		if(fputs(recvline , stdout) == EOF)
		{
			perror("fputs error");
			exit(1);
		}//if
	}//while

	/*(5) 关闭套接字*/
	close(sockfd);
}
