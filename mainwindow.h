#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

/// for JSON parsing
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

/// for list display
#include <QStandardItemModel>

/// for menu
#include "helpdialog.h"
#include "analysisdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void initialize(); /// initialize after the program beginning
    Document d; /// move from function initialize to here, the object store the total words
    int n_words; /// move to here, the number of words
    int current_row; /// the current row displayed, as an index
    QStandardItemModel *wordslistModel; /// move to here, displaying the words list
    QStandardItemModel *wordslinkModel; /// for display the linked words
    int n_thisRun = 0; /// the number of words seen this run
    int n_runForget = 0; /// the number of forgotten words this run
    int n_linkwords = 0; /// to record the current number of linked words
    QString date_q;
    void bubblesort(); /// rank the words and output it
    QString wordslist_q; /// the position of the wordslist file
    void display_word(const QString); /// display a word from QString
    HelpDialog hw;
    AnalysisDialog aw;
private slots:
    void onTableClicked(const QModelIndex &); /// click on a certain row of wordslistTableView
    void on_addNoteButton_clicked(); /// click Add Note Button
    void on_addWordButton_clicked(); /// click Add Word Button
    void on_knowButton_clicked(); /// click Known button
    void on_forgetButton_clicked(); /// click Forget button
    void on_searchButton_clicked();
    void on_linkButton_clicked();
    void pophelp(); /// slots for menu: mannual -> pop up help window
    void popanalysis();
};

#endif /// MAINWINDOW_H
