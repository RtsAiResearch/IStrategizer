#ifndef RETAINEREX_H
#define RETAINEREX_H

#include "AbstractRetainer.h"
#include "IStrategizerEx.h"
#include <string>

namespace IStrategizer
{
#define  CASEBASE_FILENAME "IStrategizerEx.cb"
#define CASEBASE_IO_WRITE_PATH (ENGINE_IO_WRITE_DIR + CASEBASE_FILENAME)
#define CASEBASE_IO_READ_PATH (ENGINE_IO_READ_DIR + CASEBASE_FILENAME)

    class RetainerEx : public AbstractRetainer
    {
    private:
        bool m_caseBaseLoaded;

    protected:
        void ExecuteCommand(const char* p_cmd);

    public:
        RetainerEx();
        void ReadCaseBase();
        void Flush();
        void Retain(_In_ CaseEx* pCase);
        ~RetainerEx();
    };
}
#endif // RETAINEREX_H
