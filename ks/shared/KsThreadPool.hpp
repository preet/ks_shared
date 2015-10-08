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

#ifndef KS_THREAD_POOL_HPP
#define KS_THREAD_POOL_HPP

#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include <ks/KsGlobal.hpp>

namespace ks
{
    // ============================================================= //

    class ThreadPool final
    {
    public:
        class Task
        {
            friend class ThreadPool;

        public:
            enum class WaitStatus : u8 {
                Done=1,
                Timeout=2
            };

            Task();
            virtual ~Task();

            // No copying allowed
            Task(Task const &) = delete;
            Task & operator=(Task const &) = delete;

            bool IsStarted() const;
            bool IsRunning() const;
            bool IsCanceled() const;
            bool IsFinished() const;

            virtual void Cancel() = 0;

            // Wait on a task indefinitely.
            // NOTE: Do NOT use the WaitFor function that takes wait_ms
            // as an argument to try and wait indefinitely (ie. by setting
            // wait_ms = milliseconds::max()) as this will cause an
            // overflow (wait_for(t) == wait_until(now + t))!
            WaitStatus Wait();

            // Wait on a task for wait_ms milliseconds
            WaitStatus WaitFor(std::chrono::milliseconds wait_ms);

        protected:
            void onStarted();
            void onFinished();
            void onCanceled();
            void onEnded();

        private:
            virtual void process() = 0;

            std::atomic<bool> m_started;
            std::atomic<bool> m_running;
            std::atomic<bool> m_canceled;
            std::atomic<bool> m_finished;
            std::promise<void> m_promise;
            std::future<void>  m_future;
        };

        // ============================================================= //

        ThreadPool(uint thread_count);
        ~ThreadPool();

        // No copying or moving allowed
        ThreadPool(ThreadPool const &) = delete;
        ThreadPool& operator=(ThreadPool const &) = delete;

        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        uint GetTaskCount() const;
        void PushFront(shared_ptr<Task> task);
        void PushBack(shared_ptr<Task> task);
        void PushFront(std::vector<shared_ptr<Task>> list_tasks);
        void PushBack(std::vector<shared_ptr<Task>> list_tasks);
        uint ProcessTask();
        void Stop();
        void Resume();

    private:
        void loop();

        uint const m_thread_count;
        std::vector<std::thread> m_list_threads;
        std::list<shared_ptr<Task>> m_queue_tasks;

        std::atomic<bool> m_running;
        mutable std::mutex m_mutex;
        std::condition_variable m_wait_cond;
    };

    // ============================================================= //
}

#endif // KS_THREAD_POOL_HPP
