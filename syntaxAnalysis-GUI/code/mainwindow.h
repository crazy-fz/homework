#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "solution.h"
using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

protected:
    /**
     * @brief resizes textEdits and labels after resizing the window
     */
    virtual void resizeEvent(QResizeEvent *event) override;

private slots:
    /**
     * @brief update output after changing text
     */
    void on_codeText_textChanged();

    void on_loadFile_triggered();

    void on_saveFile_triggered();

    void on_conpressSave_triggered();

private:
    Ui::MainWindow *ui;
    solution *sol;
    QString defaultSavePath;
};
#endif // MAINWINDOW_H
