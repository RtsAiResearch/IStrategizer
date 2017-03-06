#ifndef ACTION_H
#define ACTION_H

#include "InstructionSet.h"
#include "Attribute.h"

enum SemanticActionType
{
    ACTION_Null,
    ACTION_Assign,
    ACTION_Add,
    ACTION_Multiply,
};

// Actions are assumed to be simple, yet it can be implemented using 3 address instructions
template<class TParam>
class AbstractAction
{
protected:
    SemanticActionType  m_type;
    TParam*     m_param1;
    TParam*     m_param2;
    TParam*     m_param3;
    bool        m_executed;

public:
    AbstractAction(SemanticActionType p_type, TParam* p_param1, TParam* p_param2, TParam* p_param3 = NULL) :
      m_type(p_type), m_param1(p_param1), m_param2(p_param2), m_param3(p_param3), m_executed(false) {}
    virtual bool Executed() const { return m_executed; }
    void Execute() = 0;
};

template<class TParam>
class AssignAction : public AbstractAction<TParam>
{
public:
    AssignAction(TParam* p_dst, TParam* p_src) : AbstractAction(ACTION_Assign, p_dst, p_src) {}
    void Execute()
    {
        InstructionSet::Mov(*m_param1, *m_param2);
        m_executed = true;
    }
};

template<class TParam>
class AddAction : public AbstractAction<TParam>
{
public:
    AddAction(TParam* p_dst, TParam* p_src1, TParam* p_src2) : AbstractAction(ACTION_Add, p_dst, p_src1, p_src2) {}
    void Execute()
    {
        InstructionSet::Add(*m_param1, *m_param2, *m_param3);
        m_executed = true;
    }
};

template<class TParam>
class MultiplyAction : public AbstractAction<TParam>
{
public:
    MultiplyAction(TParam* p_dst, TParam* p_src1, TParam* p_src2) : AbstractAction(ACTION_Multiply, p_dst, p_src1, p_src2) {}
    void Execute()
    {
        InstructionSet::Mul(*m_param1, *m_param2, *m_param3);
        m_executed = true;
    }
};

#endif // ACTION_H
