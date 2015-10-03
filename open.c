#include <err.h>

int
open(const char *file, int flags, ...)
{
  struct fdmap *s;
  char *new_file;

  if (strncmp (file, "/dev/audio", 10) &&
      strncmp (file, "/dev/dsp", 8) &&
      strncmp (file, "/dev/sound", 10) &&
      strncmp (file, "/dev/mixer", 10))
    {
      /* Not for us */

      va_list ap;
      int ret;
      va_start (ap, flags);
      ret = real_open (file, flags, va_arg (ap, int));
      va_end (ap);
      return ret;
    }

  if (!strncmp (file, "/dev/dsp", 8))
    {
      if (asprintf (&new_file, "/dev/sound%s", file + sizeof("/dev/dsp") - 1) == -1)
	err(1, "asprintf");
    }
  else
    new_file = strdup (file);
  if (! new_file)
    goto fail0;

  s = zalloc (sizeof (*s));
  if (! s)
    goto fail1;

  /* This is a bit tricky... this file descriptor isn't actually
     going to be used (since we intercept syscalls), but we need
     it to identify the RUMP fd later.  We can't assign an
     arbitrary number because we need to garantee there won't be
     a collision with those assigned by the kernel.  */
  s->fd = real_open ("/dev/null", O_RDONLY, 0);
  if (s->fd == -1)
    goto fail2;

  assert (fd_count >= 0);
  if (fd_count == 0)
    {
      if (rump_init () != 0)
	goto fail3;
    }

  /* O_CREAT in this context doesn't make any sense, therefore
     we don't need to worry about the third argument. */
  s->rump_fd = rump_sys_open (new_file, flags);
  if (s->rump_fd == -1)
    goto fail3;

  pthread_mutex_lock (& fdlist_mutex);
  s->next = HASH_HEAD (fdlist, s->fd);
  HASH_HEAD (fdlist, s->fd) = s;
  fd_count++;
  pthread_mutex_unlock (& fdlist_mutex);

  return s->fd;

 fail3:
  real_close (s->fd);
 fail2:
  free (s);
 fail1:
  free (new_file);
 fail0:
  return -1;
}
