#include <stdio.h>
#include <fcntl.h>
int main()
{
	int fd;
	int val = 1234;
	int ret = 0;
	printf("=============begin test=============\n");
	fd = open("/dev/wuyu_device", O_RDWR);
	if(fd < 0){
		printf("device open failed\n");
		goto exit;
	}
	ret = ioctl(fd, 3, (void *)&val);
	printf("return of ioctl ret=%d, val = %d\n", ret, val);
	
	

exit:
	return 0;
}

