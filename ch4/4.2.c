#include	"unp.h"

char buf[128];

int
main(int argc, char **argv)
{
	int					sockfd, n;
	char				recvline[MAXLINE + 1];
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: a.out <IPaddress>");

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(13);	/* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("connect error");
	socklen_t len = sizeof(servaddr);
	if(getsockname(sockfd, (struct sockaddr*)&servaddr, &len)==-1)
		err_quit("failed to getsockname");
	

	char* ptr = sock_ntop((struct sockaddr*)&servaddr, len);
	printf("%s\n", ptr);

	exit(0);
}
