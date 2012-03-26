#ifndef CASEBASEDREASONEREX_H
#include "CaseBasedReasonerEx.h"
#endif
#ifndef ABSTRACTRETAINER_H
#include "AbstractRetainer.h"
#endif
#ifndef ABSTRACTRETRIEVER_H
#include "AbstractRetriever.h"
#endif
#ifndef ABSTRACTREVISER_H
#include "AbstractReviser.h"
#endif
#ifndef ABSTRACTADAPTER_H
#include "AbstractAdapter.h"
#endif
#ifndef CASEBASEEX_H
#include "CaseBaseEx.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif

using namespace OLCBP;

void CaseBasedReasonerEx::Initialize()
{
    _retainer->ReadCaseBase();
}
//----------------------------------------------------------------------------------------------
CaseBasedReasonerEx::~CaseBasedReasonerEx()
{
    Toolbox::MemoryClean(_reviser);
    Toolbox::MemoryClean(_retainer);
    Toolbox::MemoryClean(_adapter);
    Toolbox::MemoryClean(_retriever);
}