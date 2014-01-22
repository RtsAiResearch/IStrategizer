#include "IMViewWidget.h"
#include "IMView.h"
#include "InfluenceMap.h"

using namespace IStrategizer;

IMViewWidget::IMViewWidget(QWidget *parent)
    : QWidget(parent), m_pIM(nullptr)
{
    ui.setupUi(this);
    m_pIMView = new IMView;
    ui.gridLayout->removeItem(ui.imCanvasSpacer);
    ui.gridLayout->addWidget(m_pIMView, 0, 0, 1, 6);
}
//////////////////////////////////////////////////////////////////////////
IMViewWidget::~IMViewWidget()
{
    delete m_pIMView;
    m_pIMView = nullptr;
}
//////////////////////////////////////////////////////////////////////////
void IMViewWidget::SetIM(const InfluenceMap *p_IM)
{
    m_pIMView->SetIM(p_IM);
    m_pIM = p_IM;
}
//////////////////////////////////////////////////////////////////////////
void IMViewWidget::update()
{
    if (m_pIM != nullptr)
    {
        m_pIMView->update();
        ui.lblWorlSizeData->setText(tr("<%1, %2>").arg(m_pIM->WorldWidth()).arg(m_pIM->WorldHeight()));
        ui.lblGridSizeData->setText(tr("<%1, %2>").arg(m_pIM->GridWidth()).arg(m_pIM->GridHeight()));
        ui.lblCellSizeData->setText(tr("<%1, %2>").arg(m_pIM->CellSide()).arg(m_pIM->CellSide()));
        ui.lblNumRegObjData->setText(tr("%1 Object(s)").arg(m_pIM->RegisteredObjects().size()));
    }
}
