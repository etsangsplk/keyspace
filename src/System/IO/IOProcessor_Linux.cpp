#include "System/Platform.h"
#ifdef PLATFORM_LINUX

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include "System/IO/IOProcessor.h"
#include "System/Common.h"
#include "System/Log.h"
#include "System/Time.h"

#define EPOLL_EVENT_SIZE	1024
#define	MAX_EVENTS			256
#define PIPEOP				'p'


class PipeOp : public IOOperation
{
public:
	PipeOp()
	{
		type = PIPEOP;
		pipe[0] = pipe[1] = -1;
	}
	
	~PipeOp()
	{
		Close();
	}
	
	void Close()
	{
		if (pipe[0] >= 0)
		{
			close(pipe[0]);
			pipe[0] = -1;
		}
		if (pipe[1] >= 0)
		{
			close(pipe[1]);
			pipe[1] = -1;
		}
	}
	
	int			 	pipe[2];
	CFunc::Callback	callback;
};

// this is the singleton object
static IOProcessor	ioproc;

static int			epollfd;
static PipeOp		filePipeOp;
static PipeOp		asyncPipeOp;
static IOOperation*	canceledOps;

static bool			AddEvent(int fd, uint32_t filter, IOOperation* ioop);
static bool			AddAio(FileOp* ioop);

static void			ProcessFileOp();
static void			ProcessAsyncOp();
static void			ProcessTCPRead(struct epoll_event* ev);
static void			ProcessTCPWrite(struct epoll_event* ev);
static void			ProcessUDPRead(struct epoll_event* ev);
static void			ProcessUDPWrite(struct epoll_event* ev);

IOProcessor* IOProcessor::Get()
{
	return &ioproc;
}

static void IOProc_sigev_thread_handler(union sigval value)
{
	FileOp* fileop = (FileOp *) value.sival_ptr;
	IOOperation* prev = NULL;
	ssize_t ret;

	for (IOOperation* f = canceledOps; f; f = f->next)
	{
		if (f == fileop)
		{
			// unlink fileop from canceled ops list
			if (prev)
				prev->next = f->next;
			else
				canceledOps = f->next;
				
			f->next = NULL;
			return;
		}
		prev = f;
	}

	ret = write(filePipeOp.pipe[1], &fileop, sizeof(FileOp *));
	
	Log_Message("ret = %d", ret);
}

bool /*IOProcessor::*/InitPipe(PipeOp &pipeop, CFunc::Callback callback)
{
	if (pipe(pipeop.pipe) < 0)
	{
		Log_Errno();
		return false;
	}

	if (pipeop.pipe[0] < 0 || pipeop.pipe[1] < 0)
		return false;
	
	fcntl(pipeop.pipe[0], F_SETFD, FD_CLOEXEC);
	fcntl(pipeop.pipe[0], F_SETFL, O_NONBLOCK);
	fcntl(pipeop.pipe[1], F_SETFL, O_NONBLOCK);

	if (!AddEvent(pipeop.pipe[0], EPOLLIN | EPOLLOUT | EPOLLONESHOT, &pipeop))
		return false;
	
	pipeop.callback = callback;

	return true;
}

bool IOProcessor::Init()
{
	canceledOps = NULL;
	
	epollfd = epoll_create(EPOLL_EVENT_SIZE);
	if (epollfd < 0)
	{
		Log_Errno();
		return false;
	}
	
	if (!InitPipe(filePipeOp, ProcessFileOp))
		return false;
	
	if (!InitPipe(asyncPipeOp, ProcessAsyncOp))
		return false;

	return true;
}

void IOProcessor::Shutdown()
{
	close(epollfd);
	asyncPipeOp.Close();
	filePipeOp.Close();
}

bool IOProcessor::Add(IOOperation* ioop)
{
	uint32_t	filter;
	
	if (ioop->type == FILE_READ || ioop->type == FILE_WRITE)
	{
		return AddAio((FileOp*) ioop);
	}
	else
	{
		filter = 0;
		if (ioop->type == TCP_READ || ioop->type == UDP_READ)
		{
			filter |= EPOLLIN;
		}
		else if (ioop->type == TCP_WRITE || ioop->type == UDP_WRITE)
		{
			filter |= EPOLLOUT;
		}
		if (ioop->type == TCP_READ | ioop->type == TCP_WRITE)
		{
			//filter |= EPOLLET | EPOLLONESHOT;
			filter |= EPOLLONESHOT;
		}
		
		return AddEvent(ioop->fd, filter, ioop);
	}
}

bool AddAio(FileOp* fileop)
{
	memset(&(fileop->cb), 0, sizeof(struct aiocb));
	
	fileop->cb.aio_fildes	= fileop->fd;
	fileop->cb.aio_offset	= fileop->offset;
	fileop->cb.aio_buf		= fileop->data.buffer;

	// On Linux SIGEV_SIGNAL does not seem to work
	// therefore we use threads for async notification.		
	fileop->cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
	fileop->cb.aio_sigevent.sigev_value.sival_ptr = fileop;
	fileop->cb.aio_sigevent.sigev_notify_attributes = NULL;
	fileop->cb.aio_sigevent.sigev_notify_function = IOProc_sigev_thread_handler;
	Log_Message("fileop = %p\n", fileop->cb.aio_sigevent.sigev_value.sival_ptr);

	if (fileop->type == FILE_READ)
	{
		fileop->cb.aio_nbytes	= fileop->nbytes;

		if (aio_read(&(fileop->cb)) < 0)
		{
			Log_Errno();
			return false;
		}
	} else
	{
		fileop->cb.aio_nbytes = fileop->data.length;
		
		if (aio_write(&(fileop->cb)) < 0)
		{
			Log_Errno();
			return false;
		}
	}
	
	fileop->active = true;
	return true;
}

bool AddEvent(int fd, uint32_t event, IOOperation* ioop)
{
	int					nev;
	struct epoll_event	ev;
	
	if (epollfd < 0)
	{
		Log_Message("epollfd < 0");
		return false;
	}

	ev.events = event;
	ev.data.ptr = ioop;
	
	// add our interest in the event
	nev = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);

    // If you add the same fd to an epoll_set twice, you
    // probably get EEXIST, but this is a harmless condition.
	if (nev < 0)
	{
		if (errno == EEXIST)
		{
			nev = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
		}

		if (nev < 0)
		{
			Log_Errno();
			return false;
		}
	}
	
	if (ioop)
		ioop->active = true;
	
	return true;
}

bool /*IOProcessor::*/Remove(FileOp* fileop)
{
	int ret;
	
	ret = aio_cancel(fileop->fd, &fileop->cb);
	if (ret == AIO_ALLDONE)
		return true;
	
	if (ret == AIO_CANCELED)
	{
		fileop->next = canceledOps;
		canceledOps = fileop;
		return true;
	}
	
	return false;
}

bool IOProcessor::Remove(IOOperation* ioop)
{
	int				nev;
	struct epoll_event	ev;
	
	if (epollfd < 0)
	{
		Log_Message("eventfd < 0");
		return false;
	}

	if (ioop->type == FILE_READ || ioop->type == FILE_WRITE)
	{
		// TODO Remove(FileOp*)
		return Remove((FileOp*) ioop);
	}
	
	if (ioop->type == TCP_READ || ioop->type == UDP_READ)
		ev.events = EPOLLIN;
	else
		ev.events = EPOLLOUT;

	ev.data.fd = ioop->fd;
	
	// delete event
	nev = epoll_ctl(epollfd, EPOLL_CTL_DEL, ioop->fd, &ev);
	if (nev < 0)
	{
		Log_Errno();
		return false;
	}
	
	ioop->active = false;
	
	return true;
}

bool IOProcessor::Poll(int sleep)
{
	
	long long					called;
	int							i, nevents, wait;
	static struct epoll_event	events[MAX_EVENTS];
	IOOperation*				ioop;
	
	called = Now();
	
	do
	{
		wait = sleep - (Now() - called);
		if (wait < 0) wait = 0;
		
		nevents = epoll_wait(epollfd, events, SIZE(events), wait);
		
		if (nevents < 0)
		{
			Log_Errno();
			return false;
		}
		
		for (i = 0; i < nevents; i++)
		{
			ioop = (IOOperation *) events[i].data.ptr;
			
			ioop->active = false;
			
			if (ioop && ioop->type == PIPEOP)
			{
				PipeOp* pipeop = (PipeOp*) ioop;
				pipeop->callback();
				
				if (!AddEvent(pipeop->pipe[0], EPOLLIN, pipeop))
					return false;
				
				continue;
			}
			
			if (ioop && ioop->type == TCP_READ && (events[i].events & EPOLLIN))
				ProcessTCPRead(&events[i]);
			else if (ioop && ioop->type == TCP_WRITE && (events[i].events & EPOLLOUT))
				ProcessTCPWrite(&events[i]);
			else if (ioop && ioop->type == UDP_READ && (events[i].events & EPOLLIN))
				ProcessUDPRead(&events[i]);
			else if (ioop && ioop->type == UDP_WRITE && (events[i].events & EPOLLOUT))
				ProcessUDPWrite(&events[i]);
		}
	} while (nevents > 0 && wait > 0);
	
	return true;
}

bool IOProcessor::Complete(Callable* callable)
{
	Log_Trace();
	
	int nwrite;
	
	nwrite = write(asyncPipeOp.pipe[1], &callable, sizeof(Callable*));
	if (nwrite < 0)
	{
		Log_Errno();
		return false;
	}
	
	return true;
}

void ProcessFileOp()
{
	int	size;
	FileOp	*ops[256];
	FileOp	*fileop;
	int	pipefd;
	int	numBytes;
	int	count;
	int	i;

	Log_Message("");
	
	while (1)
	{
		pipefd = filePipeOp.pipe[0];
		size = read(pipefd, ops, sizeof(ops) * sizeof(FileOp *));
		count = size / sizeof(FileOp *);
		
		for (i = 0; i < count; i++)
		{
			fileop = ops[i];
			numBytes = aio_return(&fileop->cb);
			if (numBytes == EINPROGRESS)
				continue;
			
			Call(fileop->onComplete);
		}
		
		if ((size_t) size < SIZE(ops))
			break;
	}
}

#define MAX_CALLABLE 256	
void ProcessAsyncOp()
{
	Log_Trace();

	Callable* callables[MAX_CALLABLE];
	int nread;
	int count;
	int i;
	
	while (true)
	{
		nread = read(asyncPipeOp.pipe[0], callables, SIZE(callables));
		count = nread / sizeof(Callable*);
		
		for (i = 0; i < count; i++)
			Call(callables[i]);
		
		if (count < (int) SIZE(callables))
			break;
	}
}

void ProcessTCPRead(struct epoll_event* ev)
{
	int			readlen, nread;
	TCPRead*	tcpread;
	
	tcpread = (TCPRead*) ev->data.ptr;

	if (tcpread->listening)
	{
		Call(tcpread->onComplete);
		return;
	}
	
	if (tcpread->requested == IO_READ_ANY)
		readlen = tcpread->data.size - tcpread->data.length;
	else
		readlen = tcpread->requested - tcpread->data.length;
	
	if (readlen <= 0)
		return;

	//Log_Message(rprintf("Calling read() to read %d bytes", readlen));
	nread = read(tcpread->fd, tcpread->data.buffer + tcpread->data.length, readlen);
	if (nread < 0)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			ioproc.Add(tcpread);
		else
		{
			Log_Errno();
			Call(tcpread->onClose);
		}
	}
	else if (nread == 0)
	{
		Call(tcpread->onClose);
	}
	else
	{
		tcpread->data.length += nread;
		if (tcpread->requested == IO_READ_ANY || 
			tcpread->data.length == tcpread->requested)
			Call(tcpread->onComplete);
		else
			ioproc.Add(tcpread);
	}
}

void ProcessTCPWrite(struct epoll_event* ev)
{
	int			writelen, nwrite;
	TCPWrite*	tcpwrite;
	
	tcpwrite = (TCPWrite*) ev->data.ptr;

	// this indicates check for connect() readyness
	if (tcpwrite->data.length == 0)
	{
		sockaddr_in sa;
		socklen_t	socklen;
		
		nwrite = getpeername(tcpwrite->fd, (sockaddr*) &sa, &socklen);
		if (nwrite < 0)
			Call(tcpwrite->onClose);
		else
			Call(tcpwrite->onComplete);
	
		return;
	}

	writelen = tcpwrite->data.length - tcpwrite->transferred;
	if (writelen <= 0)
	{
		ASSERT_FAIL();
	}
	
	//Log_Message(rprintf("Calling write() to write %d bytes", writelen));
	nwrite = write(tcpwrite->fd, tcpwrite->data.buffer, writelen);
	if (nwrite < 0)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			ioproc.Add(tcpwrite);
		else
		{
			Log_Errno();
			Call(tcpwrite->onClose);
		}
	} 
	else if (nwrite == 0)
	{
		Call(tcpwrite->onClose);
	}
	else
	{
		tcpwrite->transferred += nwrite;
		if (tcpwrite->transferred == tcpwrite->data.length)
			Call(tcpwrite->onComplete);
		else
			ioproc.Add(tcpwrite);
	}
}

void ProcessUDPRead(struct epoll_event* ev)
{
	int			salen, nread;
	UDPRead*	udpread;

	udpread = (UDPRead*) ev->data.ptr;
	
	//Log_Message(rprintf("Calling recvfrom() to read max %d bytes", udpread->size));

	salen = sizeof(udpread->endpoint.sa);

	do {
		nread = recvfrom(udpread->fd, udpread->data.buffer, udpread->data.size, 0,
				(sockaddr*)&udpread->endpoint.sa, (socklen_t*)&salen);
	
		if (nread < 0)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				ioproc.Add(udpread); // try again
			else
			{
				Log_Errno();
				Call(udpread->onClose);
			}
		} 
		else if (nread == 0)
		{
			Call(udpread->onClose);
		}
		else
		{
			udpread->data.length = nread;
			Call(udpread->onComplete);
		}
	} while (nread > 0);
}

void ProcessUDPWrite(struct epoll_event* ev)
{
	int			nwrite;
	UDPWrite*	udpwrite;

	udpwrite = (UDPWrite*) ev->data.ptr;

	//Log_Message(rprintf("Calling sendto() to write %d bytes", udpwrite->data));
	nwrite = sendto(udpwrite->fd, udpwrite->data.buffer + udpwrite->offset, udpwrite->data.length - udpwrite->offset, 0,
				(const sockaddr*)&udpwrite->endpoint.sa, sizeof(udpwrite->endpoint.sa));

	if (nwrite < 0)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			ioproc.Add(udpwrite); // try again
		else
		{
			Log_Errno();
			Call(udpwrite->onClose);
		}
	}
	else if (nwrite == 0)
	{
		Call(udpwrite->onClose);
	}
	else
	{
		if (nwrite == udpwrite->data.length - udpwrite->offset)
		{
			Call(udpwrite->onComplete);
		} else
		{
			udpwrite->offset += nwrite;
			Log_Message("sendto() datagram fragmentation");
			ioproc.Add(udpwrite); // try again
		}
	}
}

#endif // PLATFORM_LINUX
