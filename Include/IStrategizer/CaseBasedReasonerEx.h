#ifndef	CASEBASEDREASONEREX_H
#define CASEBASEDREASONEREX_H

class CaseBaseEx;

namespace OLCBP
{
    class AbstractRetainer;
    class AbstractReviser;
    class AbstractAdapter;
    class AbstractRetriever;

    class CaseBasedReasonerEx
    {
    private:
        AbstractRetainer*	_retainer;
        AbstractReviser*	_reviser;
        AbstractAdapter*	_adapter;
        AbstractRetriever*	_retriever;

    public:
        inline						CaseBasedReasonerEx(AbstractRetainer* p_retainer, AbstractReviser* p_reviser, AbstractRetriever*	p_retriever, AbstractAdapter* p_adapter) : _adapter(p_adapter), _retainer(p_retainer), _retriever(p_retriever), _reviser(p_reviser) {}
        inline AbstractRetainer*	Retainer() const { return _retainer; }
        inline AbstractReviser*		Reviser() const { return _reviser; }
        inline AbstractRetriever*	Retriever() const { return _retriever; }
        inline AbstractAdapter*		Adapter() const { return _adapter; }
        void                        Initialize();
        virtual                     ~CaseBasedReasonerEx();
    };
}
#endif	// CASEBASEDREASONEREX_H