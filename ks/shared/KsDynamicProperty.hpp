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

#ifndef KS_DYNAMIC_PROPERTY_HPP
#define KS_DYNAMIC_PROPERTY_HPP

#include <vector>
#include <algorithm>
#include <ks/KsGlobal.hpp>
#include <ks/KsSignal.hpp>

namespace ks
{
    // ============================================================= //

    // ref: http://woboq.com/blog/property-bindings-in-cpp.html

    class DynamicPropertyBase
    {
    public:
        DynamicPropertyBase(std::string name="");
        virtual ~DynamicPropertyBase();

        DynamicPropertyBase(DynamicPropertyBase const &other) = delete;
        DynamicPropertyBase(DynamicPropertyBase &&other) = delete;
        DynamicPropertyBase & operator = (DynamicPropertyBase const &) = delete;
        DynamicPropertyBase & operator = (DynamicPropertyBase &&) = delete;

        std::string const & GetName() const;
        std::vector<DynamicPropertyBase*> const & GetInputs() const;
        std::vector<DynamicPropertyBase*> const & GetOutputs() const;

        static void RemoveProperty(std::vector<DynamicPropertyBase*> &list_props,
                                   DynamicPropertyBase* rem_prop);

        static void TopologicalSort(DynamicPropertyBase* property,
                                    std::vector<DynamicPropertyBase*> &list_sorted_props);

    protected:
        void captureAsInput();
        void evaluateOutputs();
        void clearInputs();
        void clearOutputs();
        void setCurrent(DynamicPropertyBase* prop);
        virtual void evaluate() = 0;

        std::string m_name;
        bool m_capture_failed;

    private:
        void registerInput(DynamicPropertyBase* input_prop);
        virtual void resetBinding() = 0;

        std::vector<DynamicPropertyBase*> m_list_inputs;
        std::vector<DynamicPropertyBase*> m_list_outputs;

        // helper for topological sort:
        // 0: unvisited
        // 1: visited
        // 2: finished
        u8 m_vx_state;
    };

    // ============================================================= //

    template<typename T>
    class DynamicProperty : public DynamicPropertyBase
    {
    public:
        using BindingFn = std::function<T()>;
        using NotifierFn = std::function<void(T const &)>;

        DynamicProperty()
        {

        }

        DynamicProperty(T value,
                        NotifierFn notifier=NotifierFn{}) :
            m_value(std::move(value))
        {
            SetNotifier(notifier);
        }

        DynamicProperty(std::string name,
                        T value,
                        NotifierFn notifier=NotifierFn{}) :
            DynamicPropertyBase(std::move(name)),
            m_value(std::move(value))
        {
            SetNotifier(notifier);
        }

        DynamicProperty(BindingFn binding,
                        NotifierFn notifier=NotifierFn{})
        {
            SetNotifier(notifier);
            Bind(binding);
        }

        DynamicProperty(std::string name,
                        BindingFn binding,
                        NotifierFn notifier=NotifierFn{}) :
            DynamicPropertyBase(std::move(name))
        {
            SetNotifier(notifier);
            Bind(binding);
        }

        ~DynamicProperty()
        {
            clearInputs();
            clearOutputs();
        }

        T const &Get()
        {
            captureAsInput();
            return m_value;
        }

        bool GetBindingValid() const
        {
            return bool(m_binding);
        }

        void Assign(T value)
        {
            // Clear inputs and binding since
            // assignment breaks bindings
            clearInputs();

            // Save new value
            m_value = std::move(value);

            // Notify
            if(m_notifier) {
                m_notifier(m_value);
            }

            evaluateOutputs();
        }

        void Bind(BindingFn binding)
        {
            clearInputs(); // resets binding!

            m_binding = std::move(binding);

            evaluate(); // captures new inputs

            evaluateOutputs();
        }

        void operator= (T value)
        {
            Assign(std::move(value));
        }

        void operator= (BindingFn binding)
        {
            Bind(std::move(binding));
        }

        void SetName(std::string name)
        {
            m_name = std::move(name);
        }

        void SetNotifier(NotifierFn notifier)
        {
            m_notifier = std::move(notifier);
        }

    private:
        void evaluate()
        {
            if(m_binding) {
                if(m_binding_init) {
                    m_value = m_binding();

                    if(m_notifier) {
                        m_notifier(m_value);
                    }
                }
                else {
                    // capture inputs for this property
                    m_capture_failed = false;
                    setCurrent(this);
                    m_value = m_binding();
                    setCurrent(nullptr);

                    if(!m_capture_failed) {
                        m_binding_init = true;

                        if(m_notifier) {
                            m_notifier(m_value);
                        }
                    }
                    else {
                        resetBinding();
                    }
                }
            }
        }

        void resetBinding()
        {
            m_binding = BindingFn{};
            m_binding_init = false;
        }

        T m_value;
        BindingFn m_binding;
        NotifierFn m_notifier;
        bool m_binding_init{false};
    };

    // ============================================================= //
}

#endif // KS_DYNAMIC_PROPERTY_HPP
