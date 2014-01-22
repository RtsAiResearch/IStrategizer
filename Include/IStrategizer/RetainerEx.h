#ifndef RETAINEREX_H
#define RETAINEREX_H

#include <string>
#include "AbstractRetainer.h"
#include "RtsGame.h"

using namespace std;

namespace IStrategizer
{
    const std::string g_CaseBasePath = "IStrategizerEx.cb";

    class RetainerEx : public AbstractRetainer
    {
    private:
        string     _caseBasePath;
        bool        _caseBaseLoaded;

    protected:
        void                ExecuteCommand(RtsGame& p_RtsGame, const char* p_cmd);
    public:
                            RetainerEx(RtsGame& p_RtsGame, string p_caseBasePath);
        void             ReadCaseBase();
        void             Flush();
        void                Retain(const CaseEx* p_case);
                            ~RetainerEx();
    };
}
#endif // RETAINEREX_H
