#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Aboutdialog.h"
#include "searchdialog.h"
#include "replacedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QColorDialog>
#include <QFontDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    textChanged = false;
    on_actionNew_triggered();
    statusLabel.setMaximumWidth(180);
    statusLabel.setText("length: " + QString::number(0) + "    lines: " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);


    statusCursorLabel.setMaximumWidth(180);
    statusCursorLabel.setText("Ln: " + QString::number(1) + "     Col: " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author = new QLabel(ui->statusbar);
    author -> setText(tr("区淑惠"));
    ui->statusbar->addPermanentWidget(author);

    ui->actionUndo ->setEnabled(false);
    ui->actionRedo ->setEnabled(false);
    ui->actionCopy ->setEnabled(false);
    ui->actionCut ->setEnabled(false);
    ui->actionPaste ->setEnabled(false);
    ui->testEdit->setShowLineNumbers(true);
    ui->actionShowLine->setChecked(true);

    QPlainTextEdit::LineWrapMode mode = ui->testEdit->lineWrapMode();
    if (mode == QTextEdit::NoWrap) {
        ui->testEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        ui->actionLineWrap->setChecked(false);
    } else {
        ui->testEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        ui->actionLineWrap->setChecked(true);
    }

    ui->actionToolbar->setChecked(true);
    ui->actionStatusbar->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();

}


void MainWindow::on_actionFind_triggered()
{
    SearchDialog dlg(this, ui->testEdit);
    dlg.exec();
}


void MainWindow::on_actionReplace_triggered()
{
    ReplaceDialog dlg(this, ui->testEdit);
    dlg.exec();
}


void MainWindow::on_actionNew_triggered()
{
    if (!userEditConfirmed())
        return;
    filePath = "";
    ui->testEdit->clear();
    this->setWindowTitle(tr("新建文本文件-编辑器"));
    textChanged = false;
}


void MainWindow::on_actionOpen_triggered()
{
    if (!userEditConfirmed())
        return;
    QString filename = QFileDialog::getOpenFileName(this, "打开文件", "D:\\PPT作业\\qt\\实验二", tr("Text files(*.txt) ;; All (*.*)"));
    if (filename.isEmpty()) {
        QMessageBox::information(this, "提示", "未选择文件打开");
        return;
    }
    ui->testEdit->clear();
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "打开文件失败");
        return;
    }
    filePath = filename;
    QTextStream in(&file);
    QString text = in.readAll();
    ui->testEdit->insertPlainText(text);
    file.close();
    this->setWindowTitle(QFileInfo(filename).absoluteFilePath());
    textChanged = false;
}


void MainWindow::on_actionSave_triggered()
{
    // 首次保存
    if (filePath.isEmpty()) {
        QString filename = QFileDialog::getSaveFileName(
                               this,
                               "保存文件",
                               "D:\\PPT作业\\qt\\实验二",
                               tr("Text files(*.txt)")
                           );

        if (filename.isEmpty()) {
            return;  // 用户取消，直接退出函数
        }

        filePath = filename;  // 记录新路径，无需提前打开文件验证
    }
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "打开文件失败：" + file.errorString());
        return;
    }

    // 写入内容到文件
    QTextStream out(&file);
    QString text = ui->testEdit->toPlainText();
    out << text;

    // 确保内容写入并关闭文件
    file.flush();
    file.close();

    // 更新窗口标题和修改状态
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
    textChanged = false;
}

void MainWindow::on_actionSaveAs_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "保存文件", "D:\\PPT作业\\qt\\实验二", tr("Text files(*.txt)"));

    // 处理用户取消保存的情况（用户点击取消按钮，filename为空）
    if (filename.isEmpty()) {
        return; // 直接退出函数，不执行后续保存操作
    }

    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "打开文件失败");
        return;
    }
    filePath = filename;
    QTextStream out(&file);
    QString text = ui->testEdit->toPlainText();
    out << text;
    file.flush();
    file.close();
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
}

void MainWindow::on_testEdit_textChanged()
{
    if (!textChanged) {
        this->setWindowTitle("*" + this->windowTitle());
        textChanged = true;
    }
    statusLabel.setText("length: " + QString::number(ui->testEdit->toPlainText().length()) + " lines: " +
                        QString::number(ui->testEdit->document()->lineCount()));
}

bool MainWindow::userEditConfirmed()
{
    if (textChanged) {
        QMessageBox msg(this);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("保存更改");
        msg.setWindowFlag(Qt::Drawer);
        msg.setText("是否保存当前的更改？");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        int r = msg.exec();
        switch (r) {
        case QMessageBox::Yes:
            if (windowTitle().replace("*", "") == tr("新建文本文件-编辑器")) {
                on_actionSaveAs_triggered();
            } else {
                on_actionSave_triggered();
            }
            return true;
        case QMessageBox::No:
            textChanged = false;
            return true;
        case QMessageBox::Cancel:
            return false;
        }
    }
    return true;
}

void MainWindow::on_actionUndo_triggered()
{
    ui->testEdit->undo();
}


void MainWindow::on_actionCut_triggered()
{
    ui->testEdit->cut();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionCopy_triggered()
{
    ui->testEdit->copy();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionPaste_triggered()
{
    ui->testEdit->paste();
}


void MainWindow::on_actionRedo_triggered()
{
    ui->testEdit->redo();
}


void MainWindow::on_testEdit_copyAvailable(bool b)
{
    ui->actionCopy->setEnabled(b);
    ui->actionCut->setEnabled(b);
}


void MainWindow::on_testEdit_redoAvailable(bool b)
{
    ui->actionRedo->setEnabled(b);
}


void MainWindow::on_testEdit_undoAvailable(bool b)
{
    ui->actionUndo->setEnabled(b);
}


void MainWindow::on_actionFontColor_triggered()
{
    QColor color = QColorDialog::getColor(QColor(currentFontColor), this, "选择字体颜色");
    if (color.isValid()) {
        currentFontColor = color.name();
        ui->testEdit->setStyleSheet(
            QString("QPlainTextEdit { color: %1; background-color: %2; }")
            .arg(currentFontColor)
            .arg(currentBgColor)
        );
        textChanged = true;
    }
}

void MainWindow::on_actionBackgroundColor_triggered()
{
    QColor color = QColorDialog::getColor(QColor(currentBgColor), this, "选择背景颜色");
    if (color.isValid()) {
        currentBgColor = color.name();
        ui->testEdit->setStyleSheet(
            QString("QPlainTextEdit { color: %1; background-color: %2; }")
            .arg(currentFontColor)
            .arg(currentBgColor)
        );
        textChanged = true;
    }
}

void MainWindow::on_actionEditorColor_triggered()
{

}


void MainWindow::on_actionLineWrap_triggered()
{
    QPlainTextEdit::LineWrapMode mode = ui->testEdit->lineWrapMode();
    if (mode == QTextEdit::NoWrap) {
        ui->testEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        ui->actionLineWrap->setChecked(true);
    } else {
        ui->testEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        ui->actionLineWrap->setChecked(false);
    }
}


void MainWindow::on_actionFont_triggered()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok)
        ui->testEdit->setFont(font);
}


void MainWindow::on_actionToolbar_triggered()
{
    bool visible = ui->toolBar->isVisible();
    ui->toolBar->setVisible(!visible);
    ui->actionToolbar->setChecked(!visible);
}


void MainWindow::on_actionStatusbar_triggered()
{
    bool visible = ui->statusbar->isVisible();
    ui->statusbar->setVisible(!visible);
    ui->actionStatusbar->setChecked(!visible);
}


void MainWindow::on_actionSelectAll_triggered()
{
    ui->testEdit->selectAll();
}


void MainWindow::on_actionExit_triggered()
{
    if (userEditConfirmed())
        exit(0);
}


void MainWindow::on_testEdit_cursorPositionChanged()
{
    int col = 0;
    int ln = 0;
    int flg = -1;
    int pos = ui->testEdit->textCursor().position();
    QString text = ui->testEdit->toPlainText();
    for (int i = 0; i < pos; i++) {
        if (text[i] == '\n') {
            ln ++;
            flg = i;
        }
    }
    flg ++;
    col = pos - flg;
    statusCursorLabel.setText("Ln: " + QString::number(ln + 1) + "     Col: " + QString::number(col + 1));
}


void MainWindow::on_actionShowLine_triggered()
{
    // 切换行号显示状态
    bool currentState = ui->testEdit->showLineNumbers();
    ui->testEdit->setShowLineNumbers(!currentState);
    // 更新按钮选中状态
    ui->actionShowLine->setChecked(!currentState);
}

