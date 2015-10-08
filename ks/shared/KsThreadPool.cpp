/*
   Copyright (C) 2015 Preet Desai (preet.desai@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <ks/shared/KsThreadPool.hpp>
#include <ks/KsLog.hpp>

namespace ks
{
    // ============================================================= //

    ThreadPool::Task::Task() :
        m_started(false),
        m_running(false),
        m_canceled(false),
        m_finished(false),
        m_future(m_promise.get_future())
    {
        // empty
    }

    ThreadPool::Task::~Task()
    {
        // empty
    }

    bool ThreadPool::Task::IsStarted() const
    {
        return m_started;
    }

    bool ThreadPool::Task::IsRunning() const
    {
        return m_running;
    }

    bool ThreadPool::Task::IsCanceled() const
    {
        return m_canceled;
    }

    bool ThreadPool::Task::IsFinished() const
    {
        return m_finished;
    }

    ThreadPool::Task::WaitStatus ThreadPool::Task::Wait()
    {
        // We assume that m_future is valid since we set
        // it during construction and never call get() on it
        m_future.wait();
        return WaitStatus::Done;
    }

    ThreadPool::Task::WaitStatus
    ThreadPool::Task::WaitFor(std::chrono::milliseconds wait_ms)
    {
        // We assume that m_future is valid since we set
        // it during construction and never call get() on it

        auto fs = m_future.wait_for(wait_ms);

        if(fs == std::future_status::ready) {
            return WaitStatus::Done;
        }

        return WaitStatus::Timeout;
    }

    void ThreadPool::Task::onStarted()
    {
        m_started = true;
        m_running = true;
    }

    void ThreadPool::Task::onFinished()
    {
        m_finished = true;
    }

    void ThreadPool::Task::onCanceled()
    {
        m_canceled = true;
    }

    void ThreadPool::Task::onEnded()
    {
        m_running = false;
        m_promise.set_value();
    }

    // ============================================================= //

    ThreadPool::ThreadPool(uint thread_count) :
        m_thread_count(thread_count),
        m_running(false)
    {
        this->Resume();
    }

    ThreadPool::~ThreadPool()
    {
        this->Stop();
//        LOG.Trace() << "STOPPED ALL THREADS";
    }

    uint ThreadPool::GetTaskCount() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return static_cast<uint>(m_queue_tasks.size());
    }

    void ThreadPool::PushFront(shared_ptr<Task> task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Add work to shared queue
        m_queue_tasks.push_front(std::move(task));

        // TODO (m_mutex can be unlockd before notifying)

        // Wake one thread from the pool
        m_wait_cond.notify_one();
    }

    void ThreadPool::PushBack(shared_ptr<Task> task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Add work to shared queue
        m_queue_tasks.push_back(std::move(task));

        // TODO (m_mutex can be unlockd before notifying)

        // Wake one thread from the pool
        m_wait_cond.notify_one();
    }

    void ThreadPool::PushFront(std::vector<shared_ptr<Task>> list_tasks)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Add work to shared queue
        m_queue_tasks.insert(
                    m_queue_tasks.begin(),
                    std::make_move_iterator(list_tasks.begin()),
                    std::make_move_iterator(list_tasks.end()));

        // TODO (m_mutex can be unlockd before notifying)

        // Wake all threads
        m_wait_cond.notify_all();
    }

    void ThreadPool::PushBack(std::vector<shared_ptr<Task>> list_tasks)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Add work to shared queue
        m_queue_tasks.insert(
                    m_queue_tasks.end(),
                    std::make_move_iterator(list_tasks.begin()),
                    std::make_move_iterator(list_tasks.end()));

        // TODO (m_mutex can be unlockd before notifying)

        // Wake all threads
        m_wait_cond.notify_all();
    }

    uint ThreadPool::ProcessTask()
    {
        shared_ptr<Task> task;
        uint tasks_remaining = 0;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if(m_queue_tasks.empty()) {
                return 0;
            }

            // Take a task to process
            task = std::move(m_queue_tasks.front());
            m_queue_tasks.pop_front();
            tasks_remaining = m_queue_tasks.size();
        }

        task->process();
        return tasks_remaining;
    }

    void ThreadPool::Stop()
    {
        if(m_running) {
            m_running = false;
            m_wait_cond.notify_all();

            for(auto & thread : m_list_threads) {
                thread.join();
            }
            m_list_threads.clear();
        }
    }

    void ThreadPool::Resume()
    {
        if(!m_running) {
            m_running = true;
            for(uint i=0; i < m_thread_count; i++) {
                m_list_threads.emplace_back(&ThreadPool::loop,this);
            }
        }
    }

    void ThreadPool::loop()
    {
        while(m_running)
        {
            // acquire lock
            std::unique_lock<std::mutex> lock(m_mutex);

            while(m_running && m_queue_tasks.empty()) {
                // wait while there are no tasks to process
                m_wait_cond.wait(lock);
            }
            // wake-up automatically reacquires lock

            if(!m_running) {
                return;
            }

            shared_ptr<Task> task = std::move(m_queue_tasks.front());
            m_queue_tasks.pop_front();
            lock.unlock();

            task->process();
        }
    }

    // ============================================================= //
}
