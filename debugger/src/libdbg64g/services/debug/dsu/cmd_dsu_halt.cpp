/*
 *  Copyright 2018 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "cmd_dsu_halt.h"
#include "debug/dsumap.h"

namespace debugger {

CmdDsuHalt::CmdDsuHalt(uint64_t dmibar, ITap *tap)
    : ICommand("halt", dmibar, tap) {

    briefDescr_.make_string("Stop simulation");
    detailedDescr_.make_string(
        "Description:\n"
        "    Stop simulation.\n"
        "Example:\n"
        "    halt\n"
        "    stop\n"
        "    s\n");
}


int CmdDsuHalt::isValid(AttributeType *args) {
    AttributeType &name = (*args)[0u];
    if (!cmdName_.is_equal(name.to_string())
        && !name.is_equal("break")
        && !name.is_equal("stop")
        && !name.is_equal("s")) {
        return CMD_INVALID;
    }
    if (args->size() != 1) {
        return CMD_WRONG_ARGS;
    }
    return CMD_VALID;
}

void CmdDsuHalt::exec(AttributeType *args, AttributeType *res) {
    res->attr_free();
    res->make_nil();
    //CrGenericRuncontrolType runctrl;
    //uint64_t addr_dmcontrol = -1;//DSUREGBASE(csr[CSR_runcontrol]);
    //runctrl.val = 0;
    //runctrl.bits.req_halt = 1;
    //tap_->write(addr_dmcontrol, 8, runctrl.u8);
}

}  // namespace debugger
