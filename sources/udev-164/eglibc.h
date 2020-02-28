#ifndef UDEV_EGLIBC_H
#define UDEV_EGLIBC_H

#ifndef le16toh
#  define le16toh(x) (x)
#endif

extern long int syscall(long int __sysno, ...);
static inline int inotify_init1(int flags) {return syscall(360, flags);}

#define SOCK_CLOEXEC 02000000
#define IN_CLOEXEC   02000000

#endif /* UDEV_EGLIBC_H */
