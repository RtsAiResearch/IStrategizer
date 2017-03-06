#ifndef UNREALBLOCKS_H
#define UNREALBLOCKS_H

#include <QtGui/QMainWindow>
#include "ui_UnrealBlocks.h"
#include "UnrealCompiler.h"
#include "Delegate.h"
#include "TreeModel.h"

class UnrealBlocks;

typedef Delegate<UnrealBlocks, UnrealCompiler> CompilerToEditorDelegate;

class UnrealBlocks : public QMainWindow
{
    Q_OBJECT

public:
    UnrealBlocks(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~UnrealBlocks();

private:
    Ui::UnrealBlocksClass ui;
    UnrealCompiler* m_compiler;
    TreeModel*      m_treeModel;
    vector<int>     m_linePosMapping;
    void OnCompilationEnded(const UnrealCompiler* p_sender, void* p_parameter);
    void OnCompilationError(const UnrealCompiler* p_sender, void* p_parameter);
    void GetCharacterBuffer(const QPlainTextEdit* p_editor, CharacterBuffer* p_buffer, vector<int>& p_linePosMapping);

    void WriteOutput();
    void WriteErrors();
private slots:
    void on_lstError_itemDoubleClicked(QListWidgetItem*);
    void Compile();
    void WriteParseTree();
};

#endif // UNREALBLOCKS_H
