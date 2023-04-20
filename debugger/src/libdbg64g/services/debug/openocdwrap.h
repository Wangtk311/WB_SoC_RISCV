/*
 *  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include "iclass.h"
#include "iservice.h"
#include "ihap.h"
#include "coreservices/ithread.h"
#include "coreservices/ijtag.h"
#include "coreservices/icmdexec.h"
#include "generic/tcpclient.h"
#include "../remote/gdbcmd.h"
#include <string>

namespace debugger {

class OpenOcdWrapper;

class OcdCmdResume : public ICommand {
 public:
    explicit OcdCmdResume(OpenOcdWrapper *parent, IJtag *ijtag);

    /** ICommand */
    virtual int isValid(AttributeType *args);
    virtual void exec(AttributeType *args, AttributeType *res);
};


class OpenOcdWrapper : public TcpClient {
 public:
    explicit OpenOcdWrapper(const char *name);
    virtual ~OpenOcdWrapper();

    /** IService interface */
    virtual void postinitService() override;
    virtual void predeleteService() override;

    /** TcpClient */
    virtual int processRxBuffer(const char *buf, int sz);

    virtual void resume();
    virtual void halt();

 protected:
    /** TcpClient generic methods */
    virtual void afterThreadStarted() override;


 private:
    class ExternalProcessThread : public IService,
                                  public IThread {
     public:
        ExternalProcessThread(IService *parent,
                              const char *name,
                              const char *path,
                              const char *script)
            : IService(name) {
            registerInterface(static_cast<IThread *>(this));
            AttributeType t1;
            RISCV_generate_name(&t1);
            RISCV_event_create(&eventLoopStarted_, t1.to_string());
            path_.make_string(path);
            script_.make_string(script);
            retcode_ = 0;
        }
        virtual ~ExternalProcessThread() {
            RISCV_event_close(&eventLoopStarted_);
        }
        virtual void waitToStart() { RISCV_event_wait(&eventLoopStarted_); }
        virtual int getRetCode() { return retcode_; }

     protected:
        virtual void busyLoop();
     private:
        AttributeType path_;
        AttributeType script_;
        event_def eventLoopStarted_;
        int retcode_;
    };


 private:
    AttributeType isEnable_;
    AttributeType jtag_;
    AttributeType cmdexec_;
    AttributeType pollingMs_;
    AttributeType openOcdPath_;
    AttributeType openOcdScript_;
 
    IJtag *ijtag_;
    ICmdExecutor *icmdexec_;

    event_def config_done_;
    ExternalProcessThread *openocd_;

    enum EState {
        State_Connecting,
        State_Idle,
    } estate_;

    GdbCommandGeneric gdbCmd_;
    OcdCmdResume *pcmdResume_;
};

DECLARE_CLASS(OpenOcdWrapper)

}  // namespace debugger
