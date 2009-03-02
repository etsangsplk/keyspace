#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "System/IO/IOProcessor.h"
#include "System/Time.h"
#include "Scheduler.h"

#define SLEEP_MSEC 20

class EventLoop : public Scheduler
{
public:
	IOProcessor*					ioproc;
	
	EventLoop()						{ ioproc = NULL; }
	EventLoop(IOProcessor* ioproc_)	{ ioproc = ioproc_; }
	
	long RunOnce()
	{
		Timer** it;
		Timer* timer;
		long long now;
		
		for (it = timers.Head(); it != NULL; it = timers.Head())
		{
			timer = *it;
			now = Now();
			
			if (timer->when <= now)
			{
				Remove(timer);
				timer->Execute();
			}
			else
				return timer->when - now;
		}
		
		return -1; // no timers to wait for
	}
	
	void Run()
	{
		long sleep;
		
		while(true)
		{
			sleep = RunOnce();
			
			if (sleep < 0)
				sleep = SLEEP_MSEC;
			
			if (ioproc)
				ioproc->Poll(sleep);
			else
				Sleep(sleep);
		}
	}
	
	void Shutdown()
	{
		Scheduler::Shutdown();
	}
};


#endif