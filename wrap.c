#define _GNU_SOURCE 1

#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>		/* open */
#include <unistd.h>

#include <rump/rump.h>
#include <rump/rump_syscalls.h>

#include "real.h"

extern int _oss_ioctl (int fd, unsigned long int request, ...);

static inline void *
zalloc (size_t size)
{
  void *ptr = malloc (size);
  if (ptr)
    memset (ptr, 0, size);
  return ptr;
}

#define ARRAY_SIZE(array)		(sizeof (array) / sizeof ((array)[0]))

/* Simple implementation of a hash table. This function retrieves the head
   element of the corresponding table entry for a given hash.  */
#define HASH_HEAD(table,hash)		((table)[(hash) % ARRAY_SIZE(table)])

struct fdmap
{
  int fd;
  int rump_fd;
  struct fdmap *next;
};

static struct fdmap *fdlist[16];
static int fd_count = 0;
static pthread_mutex_t fdlist_mutex = PTHREAD_MUTEX_INITIALIZER;

#include "open.c"

#define open		open64
#include "open.c"
#undef open

static struct fdmap *
find_rump_fdmap (int fd)
{
  struct fdmap *s;

  for (s = HASH_HEAD (fdlist, fd); s; s = s->next)
    if (s->fd == fd)
      return s;

  return NULL;
}

int
ioctl (int fd, unsigned long int request, ...)
{
  va_list ap;
  struct fdmap *s;
  unsigned long int args[8];
  int i, ret;

  /* It appears this is the only portable way to implement a vararg
     stub. The only alternatives I can think of require assembly code. */
  va_start(ap, request);
  for (i = 0; i < ARRAY_SIZE(args); i++)
    args[i] = va_arg(ap, unsigned long int);

  s = find_rump_fdmap (fd);
  if (s)
    ret = _oss_ioctl (s->rump_fd, request,
		      args[0], args[1], args[2], args[3],
		      args[4], args[5], args[6], args[7]);
  else
    ret = real_ioctl (fd, request,
		      args[0], args[1], args[2], args[3],
		      args[4], args[5], args[6], args[7]);

  va_end (ap);

  return ret;
}

ssize_t
read (int fd, void *buf, size_t n)
{
  struct fdmap *s = find_rump_fdmap (fd);
  if (! s)
    return real_read (fd, buf, n);

  return rump_sys_read (s->rump_fd, buf, n);
}

ssize_t
write (int fd, const void *buf, size_t n)
{
  struct fdmap *s = find_rump_fdmap (fd);
  if (! s)
    return real_write (fd, buf, n);

  return rump_sys_write (s->rump_fd, buf, n);
}

int
close (int fd)
{
  struct fdmap *s = find_rump_fdmap (fd);
  if (! s)
    return real_close (fd);

  if (rump_sys_close (s->rump_fd) == -1)
    return -1;

  /* Remove fdmap from list.  */
  pthread_mutex_lock (& fdlist_mutex);
  if (s == HASH_HEAD (fdlist, fd))
    {
      HASH_HEAD (fdlist, fd) = HASH_HEAD (fdlist, fd)->next;
    }
  else
    {
      struct fdmap *i;
      for (i = HASH_HEAD (fdlist, fd); i; i = i->next)
	if (i->next == s)
	  {
	    i->next = i->next->next;
	    break;
	  }
    }
  fd_count--;
  pthread_mutex_unlock (& fdlist_mutex);
  free (s);

  /* Close the dummy socket (it was a /dev/null handle).  */
  real_close (fd);

  assert (fd_count >= 0);

  return 0;
}
