//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef TASKCONSTRAINTSDISTR_H_
#define TASKCONSTRAINTSDISTR_H_

#include "TaskConstraints.h"
#include <omnetpp.h>
#include <map>


class TaskConstraintsDistr {
public:
    TaskConstraintsDistr(bool synth, double probconstr, double probsat);
    virtual ~TaskConstraintsDistr();
    TaskConstraints* constraintsForTask(long jobid, long taskid);
private:
    std::map<std::pair<long,long>,TaskConstraints*> constraints;
    bool synth;
    double probconstr;
    double probsat;
};

#endif /* TASKCONSTRAINTSDISTR_H_ */
