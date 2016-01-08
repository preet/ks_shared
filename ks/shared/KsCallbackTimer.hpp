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

#ifndef KS_CALLBACK_TIMER_HPP
#define KS_CALLBACK_TIMER_HPP

#include <ks/KsTimer.hpp>

namespace ks
{
    class CallbackTimer final : public ks::Object
    {
    public:
        CallbackTimer(ks::Object::Key const &key,
                      shared_ptr<EventLoop> const &evloop,
                      Milliseconds interval_ms,
                      std::function<void()> callback);

        void Init(ks::Object::Key const &,
                  shared_ptr<CallbackTimer> const &);

        ~CallbackTimer();

        void SetRepeating(bool repeating);
        void SetInterval(Milliseconds interval_ms);
        void Start();
        void Stop();

    private:
        void onTimeout();

        shared_ptr<Timer> const m_timer;

        bool m_active;
        Milliseconds m_interval_ms;
        bool m_repeating;
        std::function<void()> m_callback;
    };
}

#endif // KS_CALLBACK_TIMER_HPP
