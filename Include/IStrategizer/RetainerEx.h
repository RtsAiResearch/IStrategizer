#ifndef RETAINEREX_H
#define RETAINEREX_H

#ifndef ABSTRACTRETAINER_H
#include "AbstractRetainer.h"
#endif

#include <string>
using namespace std;

namespace OLCBP
{
    class RetainerEx : public AbstractRetainer
    {
    private:
        string	    _caseBasePath;
        bool        _caseBaseLoaded;

    protected:
        void                ExecuteCommand(const char* p_cmd);
    public:
                            RetainerEx(string p_caseBasePath);
        void	            ReadCaseBase();
        void	            Flush();
        void                Retain(const CaseEx* p_case);
                            ~RetainerEx();
    };
}
#endif // RETAINEREX_H