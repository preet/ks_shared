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

#include <ks/shared/KsCallbackTimer.hpp>

namespace ks
{
    CallbackTimer::CallbackTimer(ks::Object::Key const &key,
                                 shared_ptr<EventLoop> const &evloop,
                                 std::chrono::milliseconds interval_ms,
                                 std::function<void()> callback) :
        ks::Object(key,evloop),
        m_timer(make_object<Timer>(evloop)),
        m_active(false),
        m_interval_ms(interval_ms),
        m_repeating(true),
        m_callback(callback)
    {

    }

    void CallbackTimer::Init(ks::Object::Key const &,
                             shared_ptr<CallbackTimer> const &this_callback_timer)
    {
        this_callback_timer->m_timer->signal_timeout.Connect(
                    this_callback_timer,
                    &CallbackTimer::onTimeout);
    }

    CallbackTimer::~CallbackTimer()
    {

    }

    void CallbackTimer::SetRepeating(bool repeating)
    {
        m_repeating = repeating;
    }

    void CallbackTimer::SetInterval(std::chrono::milliseconds interval_ms)
    {
        m_interval_ms = interval_ms;
    }

    void CallbackTimer::Start()
    {
        m_active = true;
        m_timer->Start(m_interval_ms,false);
    }

    void CallbackTimer::Stop()
    {
        m_active = false;
        m_timer->Stop();
    }

    void CallbackTimer::onTimeout()
    {
        if(m_active) {
            if(m_repeating) {
                m_timer->Start(m_interval_ms,false);
            }
            m_callback();
        }
    }
}
