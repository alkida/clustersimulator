//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/basemsg.msg.
//

#ifndef _BASEMSG_M_H_
#define _BASEMSG_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/basemsg.msg</tt> by opp_msgc.
 * <pre>
 * message BaseMsg {
 *     int src;
 *     int dst;
 * }
 * </pre>
 */
class BaseMsg : public ::cMessage
{
  protected:
    int src_var;
    int dst_var;

  private:
    void copy(const BaseMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const BaseMsg&);

  public:
    BaseMsg(const char *name=NULL, int kind=0);
    BaseMsg(const BaseMsg& other);
    virtual ~BaseMsg();
    BaseMsg& operator=(const BaseMsg& other);
    virtual BaseMsg *dup() const {return new BaseMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getSrc() const;
    virtual void setSrc(int src);
    virtual int getDst() const;
    virtual void setDst(int dst);
};

inline void doPacking(cCommBuffer *b, BaseMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, BaseMsg& obj) {obj.parsimUnpack(b);}


#endif // _BASEMSG_M_H_