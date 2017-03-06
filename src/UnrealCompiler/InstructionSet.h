#ifndef INSTRUCTIONSET_H
#define INSTRUCTIONSET_H

class InstructionSet
{
public:
    template<class T>
    static void Mov(T& p_dst, T& p_src) { p_dst = p_src; }

    template<class T>
    static void Add(T& p_dst, T& p_src1, T& p_src2) { p_dst = p_src1 + p_src2; }

    template<class T>
    static void Sub(T& p_dst, T& p_src1, T& p_src2) { p_dst = p_src1 - p_src2; }

    template<class T>
    static void Mul(T& p_dst, T& p_src1, T& p_src2) { p_dst = p_src1 * p_src2; }

    template<class T>
    static void Div(T& p_dst, T& p_src1, T& p_src2) { p_dst = p_src1 / p_src2; }

    typedef void (*IntBinaryOp3)(int&, int&, int&);
    typedef void (*IntBinaryOp2)(int&, int&);
};

#endif // INSTRUCTIONSET_H
