/*	$NetBSD: ioccom.h,v 1.11 2011/10/19 10:53:12 yamt Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1990, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ioccom.h	8.3 (Berkeley) 1/9/95
 */

#ifndef	_SYS_IOCCOM_H_
#define	_SYS_IOCCOM_H_

/*
 * Ioctl's have the command encoded in the lower word, and the size of
 * any in or out parameters in the upper word.  The high 3 bits of the
 * upper word are used to encode the in/out status of the parameter.
 *
 *	 31 29 28                     16 15            8 7             0
 *	+---------------------------------------------------------------+
 *	| I/O | Parameter Length        | Command Group | Command       |
 *	+---------------------------------------------------------------+
 */
#define	NB_IOCPARM_MASK	0x1fff		/* parameter length, at most 13 bits */
#define	NB_IOCPARM_SHIFT	16
#define	NB_IOCGROUP_SHIFT	8
#define	NB_IOCPARM_LEN(x)	(((x) >> NB_IOCPARM_SHIFT) & NB_IOCPARM_MASK)
#define	NB_IOCBASECMD(x)	((x) & ~(NB_IOCPARM_MASK << NB_IOCPARM_SHIFT))
#define	NB_IOCGROUP(x)	(((x) >> NB_IOCGROUP_SHIFT) & 0xff)

#define	NB_IOCPARM_MAX	NBPG	/* max size of ioctl args, mult. of NBPG */
				/* no parameters */
#define	NB_IOC_VOID	(unsigned long)0x20000000
				/* copy parameters out */
#define	NB_IOC_OUT		(unsigned long)0x40000000
				/* copy parameters in */
#define	NB_IOC_IN		(unsigned long)0x80000000
				/* copy parameters in and out */
#define	NB_IOC_INOUT	(NB_IOC_IN|NB_IOC_OUT)
				/* mask for IN/OUT/VOID */
#define	NB_IOC_DIRMASK	(unsigned long)0xe0000000

#define	_NB_IOC(inout, group, num, len) \
    ((inout) | (((len) & NB_IOCPARM_MASK) << NB_IOCPARM_SHIFT) | \
    ((group) << NB_IOCGROUP_SHIFT) | (num))
#define	_NB_IO(g,n)	_NB_IOC(NB_IOC_VOID,	(g), (n), 0)
#define	_NB_IOR(g,n,t)	_NB_IOC(NB_IOC_OUT,	(g), (n), sizeof(t))
#define	_NB_IOW(g,n,t)	_NB_IOC(NB_IOC_IN,	(g), (n), sizeof(t))
/* this should be _IORW, but stdio got there first */
#define	_NB_IOWR(g,n,t)	_NB_IOC(NB_IOC_INOUT,	(g), (n), sizeof(t))

#endif /* !_SYS_IOCCOM_H_ */
