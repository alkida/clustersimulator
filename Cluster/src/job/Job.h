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
#include "Task.h"
#include <list>

#ifndef JOB_H_
#define JOB_H_

/*
 * job has an id and a list of tasks
 * */
class Job {
public:
    Job(long id, int num_tasks);
    virtual ~Job();
    std::list<Task*> *getTasks();
    long getId();
private:
    long id;
    std::list<Task*> tasks;
    int num_tasks;
};

#endif /* JOB_H_ */
