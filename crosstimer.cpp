#include "crosstimer.h"



CrossTimer::CrossTimer():
    timerId(no_timer)
{

}

CrossTimer::CrossTimer(CrossTimer &&other):
    timerId(other.timerId),
    next(other.next),
    period(other.period),
    handler(other.handler),
    isRunning(other.isRunning)
{

}

CrossTimer::CrossTimer(CrossTimer::timer_id id, CrossTimer::Timestamp next, CrossTimer::Duration period, CrossTimer::handle_type handler):
    timerId(id),
    next(next),
    period(period),
    handler(handler)
{

}

CrossTimer::~CrossTimer()
{
    TimerThread::global().clearTimer(timerId);
}

void CrossTimer::start(CrossTimer::millisec msPeriod, CrossTimer::handle_type handler)
{

    next = Clock::now() +Duration(msPeriod);
    period = Duration(msPeriod);
    this->handler = handler;
    timerId = TimerThread::global().addTimer(this);
}

void CrossTimer::singleShot(CrossTimer::millisec msDelay, CrossTimer::handle_type handler)
{
    next = Clock::now() +Duration(msDelay);
    period = Duration(0);
    this->handler = handler;
    timerId = TimerThread::global().addTimer(this);
}

void CrossTimer::stop()
{
    TimerThread::global().clearTimer(timerId);
}

CrossTimer::TimerThread &CrossTimer::TimerThread::global()
{
    static CrossTimer::TimerThread thread;
    return thread;
}

CrossTimer::TimerThread::TimerThread():
    nextId(no_timer),
    done(false)
{

}

CrossTimer::TimerThread::~TimerThread()
{
    ScopeLock lock(sync);
    if(workerThread.joinable())
    {
        done = true;
        lock.unlock();
        wakeUp.notify_all();
        workerThread.join();
    }

}

CrossTimer::timer_id CrossTimer::TimerThread::addTimer(CrossTimer::millisec msDelay, CrossTimer::millisec msPeriod, CrossTimer::handle_type handler)
{
//    ScopeLock lock(sync);

//    if(!workerThread.joinable())
//        workerThread = std::thread(&CrossTimer::TimerThread::timerThreadWork,this);

//    ++nextId;
//    active.emplace(nextId,timer);
    return 0;
}

CrossTimer::timer_id CrossTimer::TimerThread::addTimer(CrossTimer *timer)
{
    ScopeLock lock(sync);

    if(!workerThread.joinable())
        workerThread = std::thread(&CrossTimer::TimerThread::timerThreadWork,this);

    ++nextId;
    auto iter = active.emplace(nextId,timer);

    Queue::iterator place = queue.emplace(*(iter.first->second));

    bool needNotify = (place == queue.begin());
    lock.unlock();
    if(needNotify)
        wakeUp.notify_all();
    return nextId;
}

bool CrossTimer::TimerThread::clearTimer(CrossTimer::timer_id id)
{
    ScopeLock lock(sync);
    auto iter = active.find(id);
    return destroyImpl(lock,iter,true);
}

std::size_t CrossTimer::TimerThread::size() const
{
    ScopeLock lock(sync);
    return active.size();
}

bool CrossTimer::TimerThread::empty() const
{
    ScopeLock lock(sync);
    return active.empty();
}

void CrossTimer::TimerThread::timerThreadWork()
{
    ScopeLock lock(sync);
    while (!done)
    {
        if(queue.empty())
        {
            wakeUp.wait(lock,[=]{return done || !queue.empty();});
            continue;
        }

        auto queueHead = queue.begin();
        CrossTimer &timer = *queueHead;
        auto now = Clock::now();
        if(now > timer.next)
        {
            queue.erase(timer);

            timer.isRunning = true;
            lock.unlock();
            timer.handler();
            lock.lock();

            if(timer.isRunning)
            {
                timer.isRunning = false;

                if(timer.period.count() > 0)
                {
                    timer.next = timer.next + timer.period;
                    queue.emplace(timer);
                }
                else
                {
                    active.erase(timer.timerId);
                }
            }
            else
            {
                timer.waitCond->notify_all();
                active.erase(timer.timerId);
            }
        }
        else
        {
            wakeUp.wait_until(lock,timer.next);
        }
    }
}

bool CrossTimer::TimerThread::destroyImpl(CrossTimer::ScopeLock &lock, TimerMap::iterator i, bool notify)
{
    if(i == active.end())
        return false;
    CrossTimer * timer = i->second;

    if(timer->isRunning)
    {
        timer->isRunning = false;
        timer->waitCond.reset(new Conditionval);
        timer->waitCond->wait(lock);
    }
    else
    {
        queue.erase(*timer);
        active.erase(i);
        if(notify)
        {
            lock.unlock();
            wakeUp.notify_all();
        }

    }
    return true;
}
