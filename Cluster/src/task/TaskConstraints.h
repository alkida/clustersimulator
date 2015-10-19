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

#ifndef TASKREQUESTS_H_
#define TASKREQUESTS_H_

#include <list>
#include <string>
#include <utility>
#define CONSTRAINT_EQUAL 0
#define CONSTRAINT_NOT_EQUAL 1
#define CONSTRAINT_LESS 2
#define CONSTRAINT_GREATER 3


using namespace std;

/*taskconstraints are a list of constraints
 * each constraint has an attribute id, a value and a comparison operator
 * */

class TaskConstraints {
public:
    TaskConstraints();
    virtual ~TaskConstraints();
    void addConstraint(long attr, long val, long cmp);
    list<pair<long,pair<long,long> > >* getConstraints();
private:
    list<pair<long,pair<long,long> > > constraints;
};

#endif /* TASKREQUESTS_H_ */
