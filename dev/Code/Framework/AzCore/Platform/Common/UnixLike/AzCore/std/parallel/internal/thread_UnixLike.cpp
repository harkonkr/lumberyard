/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include <AzCore/std/parallel/thread.h>

#include <AzCore/std/parallel/threadbus.h>

#include <sched.h>
#include <errno.h>

namespace AZStd
{
    namespace Platform
    {
        void NameCurrentThread(const char* name);
        void PreCreateSetThreadAffinity(int cpuId, pthread_attr_t& attr);
        void SetThreadPriority(int priority, pthread_attr_t& attr);
        void PostCreateThread(pthread_t tId, const char * name, int cpuId);
    }

    namespace Internal
    {
        void* thread_run_function(void* param)
        {
            thread_info* ti = reinterpret_cast<thread_info*>(param);

            Platform::NameCurrentThread(ti->m_name);

            ti->execute();

            destroy_thread_info(ti);
            EBUS_EVENT(ThreadEventBus, OnThreadExit, this_thread::get_id());
            pthread_exit(nullptr);
            return nullptr;
        }

        pthread_t create_thread(const thread_desc* desc, thread_info* ti)
        {
            pthread_attr_t attr;
            pthread_attr_init(&attr);

            size_t stackSize = 2 * 1024 * 1024;
            int priority = -1;
            int cpuId = -1;
            const char* name = "AZStd thread";
            if (desc)
            {
                if (desc->m_stackSize != -1)
                {
                    stackSize = desc->m_stackSize;
                }
                if (desc->m_priority >= sched_get_priority_min(SCHED_FIFO) && desc->m_priority <= sched_get_priority_max(SCHED_FIFO))
                {
                    priority = desc->m_priority;
                }
                if (desc->m_name)
                {
                    name = desc->m_name;
                }
                cpuId = desc->m_cpuId;

                pthread_attr_setdetachstate(&attr, desc->m_isJoinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED);

                Platform::PreCreateSetThreadAffinity(cpuId, attr);
            }

            pthread_attr_setstacksize(&attr, stackSize);

            Platform::SetThreadPriority(priority, attr);

            pthread_t tId;
            int res = pthread_create(&tId, &attr, &thread_run_function, ti);
            (void)res;
            AZ_Assert(res == 0, "pthread failed %s", strerror(errno));
            pthread_attr_destroy(&attr);

            // Platform specific post thread creation action (setting thread name on some, affinity on others)
            Platform::PostCreateThread(tId, name, cpuId);

            EBUS_EVENT(ThreadEventBus, OnThreadEnter, thread::id(tId), desc);
            return tId;
        }
    }


    thread::thread()
    {
        m_thread = native_thread_invalid_id;
    }

    thread::thread(Internal::thread_move_t<thread> rhs)
    {
        m_thread = rhs->m_thread;
        rhs->m_thread = native_thread_invalid_id;
    }

    thread::~thread()
    {
        AZ_Assert(!joinable(), "You must call detach or join before you delete a thread!");
    }

    void thread::join()
    {
        AZ_Assert(joinable(), "Thread must be joinable!");
        if (!pthread_equal(m_thread, native_thread_invalid_id))
        {
            pthread_join(m_thread, nullptr);
            m_thread = native_thread_invalid_id;
        }
    }
    void thread::detach()
    {
        AZ_Assert(joinable(), "Thread must be joinable!");
        if (!pthread_equal(m_thread, native_thread_invalid_id))
        {
            pthread_detach(m_thread);
            m_thread = native_thread_invalid_id;
        }
    }

    unsigned thread::hardware_concurrency()
    {
        return static_cast<unsigned int>(sysconf(_SC_NPROCESSORS_ONLN));
    }
}
