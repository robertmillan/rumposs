#include <sys/syscall.h>

#define real_open(a,b,c)			syscall(SYS_open,(a),(b),(c))
#define real_ioctl(a,b,c,d,e,f,g,h,i,j)		syscall(SYS_ioctl,(a),(b),(c),(d),(e),(f),(g),(h),(i),(j))
#define real_read(a,b,c)			syscall(SYS_read,(a),(b),(c))
#define real_write(a,b,c)			syscall(SYS_write,(a),(b),(c))
#define real_close(a)				syscall(SYS_close,(a))
