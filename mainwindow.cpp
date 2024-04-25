/*
 * Author: Chunwei Ma
 * Coding started date: 2016-1-17
 * v1.0 finished date: 2016-1-18, used a weekend to do it
 * v1.1 beginning date: 2016-12-23, this version, we will add more functionalities, such as search and display, statistics of data path, etc.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

/// for JSON parsing
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

/// for testing
#include <iostream>
#include <string>
using namespace std;

/// for file reading
#include <QFile>
#include <QTextStream>

/// for list display
#include <QStandardItemModel>
#include <QDataWidgetMapper>

/// for pop up message box
#include <QMessageBox>

/// for files
#include <QDir>
#include <QFile>
#include <QDateTime>

/// for utility
#include "utility.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // this->setFixedSize(590,465);
    this->setFixedSize(660,506);
    /// to do after the program beginning
    initialize(); /// initialize after the program beginning

    ui->addEdit->setPlaceholderText(QStringLiteral("Search/add link/Note/Words here..."));
    /// set hot key
    ui->addWordButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W)); /// for add word
    ui->searchButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S)); /// for search word
}

MainWindow::~MainWindow()
{
    bubblesort();
    delete ui;
}

void MainWindow::initialize() /// initialize after the program beginning
{
    /// configuration file
    QFile config_f("./config.json");
    config_f.open(QIODevice::ReadOnly);
    QTextStream config_i(&config_f);
    QString config_q = config_i.readAll();
    char *config_c;
    QByteArray bac = config_q.toLatin1();
    config_c = bac.data();
    Document dc;
    dc.Parse(config_c);
    // QString wordslist_q = dc["wordslist"].GetString(); /// move wordslist_q to the head file
    wordslist_q = dc["wordslist"].GetString();

    /// open wordslist file
    // QFile wordslistFile("./wordslist.json");
    QFile wordslistFile(wordslist_q);
    wordslistFile.open(QIODevice::ReadOnly);

    QTextStream in(&wordslistFile);
    QString wordslist = in.readAll();

    /// backup file
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd-hh-mm-ss");
    date_q = time.toString("yyyy-MM-dd");
    if (QFile::exists("./backup/wordslist-backup.json"))
    {
        QFile::remove("./backup/wordslist-backup.json");
    }
    wordslistFile.copy("./backup/wordslist-backup.json"); // "./backup/wordslist-"+str+".json"
    wordslistFile.close();
    /// remove the original file
    wordslistFile.remove();

    /// QString to char*
    char *wordslist_c;
    QByteArray ba = wordslist.toLatin1();
    wordslist_c = ba.data();

    /// paring JSON to DOM
    /// Document d; has been move to class member
    d.Parse(wordslist_c);

    /// display the words list
    wordslistModel = new QStandardItemModel; /// the definition has been move to the .h file as class member
    wordslinkModel = new QStandardItemModel; /// the linked words list at the same time constructed, although no data in it
    /// table head
    wordslistModel->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("Words")));
    /// link the model with tableview
    ui->wordsListTableView->setModel(wordslistModel);
    /// fixed column width
    ui->wordsListTableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    /// select the total row
    ui->wordsListTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    /// no edit allowed
    ui->wordsListTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    /// num of words/row
    n_words = 0;
    for(Value::ConstMemberIterator it = d.MemberBegin(); it != d.MemberEnd(); it++)
    {
        wordslistModel->setItem(n_words,0,new QStandardItem(it->name.GetString()));
        /// display in the center
        wordslistModel->item(n_words,0)->setTextAlignment(Qt::AlignCenter);
        n_words++;
    }

    /// total words number label
    ui->nTotalLabel->setText(QString::number(n_words));

    /// display a certain word: its label and Note etc.
    connect(ui->wordsListTableView,SIGNAL(clicked(const QModelIndex &)),this, SLOT(onTableClicked(const QModelIndex &)));
    /// for link tableview slot
    connect(ui->linkTableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &))); /// just the same slot, it works!

    /// slots for the menu:
    connect(ui->actionMannual, SIGNAL(triggered(bool)), this, SLOT(pophelp()));
    connect(ui->actionDate_path, SIGNAL(triggered(bool)), this, SLOT(popanalysis()));
}

/// get a word from QString, and then display it
void MainWindow::display_word(const QString search_q) {
    // QString celltext = index.data().toString();
    // current_row= index.row(); /// note: index.row begins with 0
    /// QString to char*
    QByteArray ba = search_q.toLatin1();
    const char *c = ba.data();

    /// set word label and font size
    ui->wordLabel->setText(search_q);
    QFont ft_label;
    ft_label.setPointSize(13);
    ft_label.setBold(true);
    ui->wordLabel->setFont(ft_label);

    /// eg: word = d["apple"]
    Value& word = d[c];
    Value& noteArray = word["Note"];

    /// display the seen, known, forget, and importance
    Value& n_seen = word["seen"];
    Value& n_forget = word["forget"];
    Value& n_importance = word["importance"];
    ui->nSeenLabel->setText(QString::number(n_seen.GetInt()));
    ui->nForgetLabel->setText(QString::number(n_forget.GetInt()));
    ui->nImpoLabel->setText(QString::number(n_importance.GetDouble()));

    /// display Note in the noteEdit
    QString s_note = "";
    s_note = s_note+word["date"].GetString()+"\n";
    for(SizeType i = 0; i < noteArray.Size(); i++)
    {
        Value& a_note = noteArray[i];
        s_note=s_note+"<< "+a_note.GetString()+"\n";
    }
    ui->noteEdit->setPlainText(s_note);
    QFont ft_edit;
    ft_edit.setPointSize(10);
    ft_edit.setFamily("Microsoft YaHei UI");
    ui->noteEdit->setFont(ft_edit);

    /// display the linked words in linkTableView
    /// to display the link of current word, first clear the content of the wordslinkmodel
    wordslinkModel->clear();
    n_linkwords = 0;
    Value& linkArray = word["link"];
    for(SizeType i = 0; i < linkArray.Size(); i++)
    {
        Value& a_link = linkArray[i];
        wordslinkModel->setItem(i,0,new QStandardItem(a_link.GetString()));
        wordslinkModel->item(i,0)->setTextAlignment(Qt::AlignCenter);
        n_linkwords++;
    }
    /// the link words tableview, control the format at here
    wordslinkModel->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("link")));
    ui->linkTableView->setModel(wordslinkModel);
    ui->linkTableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->linkTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->linkTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

void MainWindow::onTableClicked(const QModelIndex & index) /// click on a certain row of wordslistTableView
{
    if(index.isValid())
    {
        QString celltext = index.data().toString();
        current_row= index.row(); /// note: index.row begins with 0
        display_word(celltext);
    }
}

void MainWindow::on_addNoteButton_clicked() /// add a new note to the note array
{
    QString new_note = ui->addEdit->toPlainText();
    /// empty check
    if(new_note.trimmed().isEmpty())
    {
        QMessageBox message(QMessageBox::Warning,"Warning","No new note typing",QMessageBox::Ok,NULL);
        message.exec();
    }
    else{
        /// QString to char*
        char *new_note_c;
        QByteArray ba = new_note.toLatin1();
        new_note_c = ba.data();
        /// get label
        QString word_label = ui->wordLabel->text();
        char *word_label_c;
        ba = word_label.toLatin1();
        word_label_c = ba.data();
        /// get word from d, and its notes
        Value& word = d[word_label_c];
        Value& noteArray = word["Note"];
        /// add new note to the word
        /// allocator
        Document::AllocatorType& allocator = d.GetAllocator();
        /// new string Value
        Value note_string(kStringType);
        /// must set the string like this
        note_string.SetString(new_note_c,allocator);
        /// add to array
        noteArray.PushBack(note_string,allocator);
        /// clear the note edit
        ui->addEdit->clear();
    }
}

void MainWindow::on_addWordButton_clicked()
{
    QString new_word = ui->addEdit->toPlainText();
    /// empty check
    if(new_word.trimmed().isEmpty())
    {
        QMessageBox message(QMessageBox::Warning,"Warning","No new word typing",QMessageBox::Ok,NULL);
        message.exec();
    }
    if(new_word.indexOf("@",0) == -1)
    {
        QMessageBox message;
        message.setText("Notice your typing, format: apple@great company.");
        message.exec();
    }
    else{
        /// input format: word@Note. eg: apple@a kind of fruit.
        QByteArray ba = new_word.split("@").at(0).toLatin1();
        char *new_word_c = ba.data();
        ba = new_word.split("@").at(1).toLatin1();
        char *new_wordnote_c = ba.data();

        //* whether this word has already existed:
        if(d.HasMember(new_word_c)) {
            QMessageBox message;
            message.setText("This word has already existed.");
            message.exec();
        } else {

        /// construct the new object
        Value new_word_str(kStringType);
        Value new_wordnote_str(kStringType);
        Value new_wordnote_arr(kArrayType);
        Value link_arr(kArrayType);
        Value date_str(kStringType);
        Value word_detail_obj(kObjectType);
        Document::AllocatorType& allocator = d.GetAllocator();
        new_word_str.SetString(new_word_c,allocator);
        new_wordnote_str.SetString(new_wordnote_c,allocator);
        new_wordnote_arr.PushBack(new_wordnote_str,allocator);

        /// date
        ba = date_q.toLatin1();
        char *date_c = ba.data();
        /// complete the detail of the word
        word_detail_obj.AddMember("Note",new_wordnote_arr,allocator);
        word_detail_obj.AddMember("seen",1,allocator); /// v1.0: 0
        word_detail_obj.AddMember("known",0,allocator);
        word_detail_obj.AddMember("forget",1,allocator); /// v1.0: 0
        word_detail_obj.AddMember("importance",1.0,allocator); /// v1.0: 0.0
        word_detail_obj.AddMember("link",link_arr,allocator);
        date_str.SetString(date_c,allocator);
        word_detail_obj.AddMember("date",date_str,allocator);
        /// add the new word to d
        d.AddMember(new_word_str,word_detail_obj,allocator);
        /// clear the input box
        ui->addEdit->clear();
        /// display new word in the wordslist
        wordslistModel->setItem(n_words,0,new QStandardItem(new_word.split("@").at(0))); /// must use QString instead of char*, otherwise weird error occurs
        /// display in the center
        wordslistModel->item(n_words,0)->setTextAlignment(Qt::AlignCenter);
        n_words++;

        /// total words number label
        ui->nTotalLabel->setText(QString::number(n_words));
        }
    }
}

void MainWindow::on_knowButton_clicked()
{
    /// get label to word
    QString word_label = ui->wordLabel->text();
    /// empty check
    if(!word_label.trimmed().isEmpty())
    {
    char *word_label_c;
    QByteArray ba = word_label.toLatin1();
    word_label_c = ba.data();
    Value& word = d[word_label_c];
    word["seen"].SetInt(word["seen"].GetInt()+1);
    word["known"].SetInt(word["known"].GetInt()+1);
    double impo_d = (double)word["forget"].GetInt()/(double)word["seen"].GetInt();
    word["importance"].SetDouble(impo_d);
    n_thisRun++;
    ui->nRunLabel->setText(QString::number(n_thisRun));
    ui->nRunForgetLabel->setText(QString::number(n_runForget));
    }
}

void MainWindow::on_forgetButton_clicked()
{
    /// get label to word
    QString word_label = ui->wordLabel->text();
    /// empty check
    if(!word_label.trimmed().isEmpty())
    {
    char *word_label_c;
    QByteArray ba = word_label.toLatin1();
    word_label_c = ba.data();
    Value& word = d[word_label_c];
    word["seen"].SetInt(word["seen"].GetInt()+1);
    word["forget"].SetInt(word["forget"].GetInt()+1);
    double impo_d = (double)word["forget"].GetInt()/(double)word["seen"].GetInt();
    word["importance"].SetDouble(impo_d);
    n_thisRun++;
    n_runForget++;
    ui->nRunLabel->setText(QString::number(n_thisRun));
    ui->nRunForgetLabel->setText(QString::number(n_runForget));
    }
}

void MainWindow::on_searchButton_clicked()
{
    QString search_q = ui->addEdit->toPlainText();
    /// empty check
    if(search_q.trimmed().isEmpty())
    {
        QMessageBox message(QMessageBox::Warning,"Warning","No search typing",QMessageBox::Ok,NULL);
        message.exec();
    }
    else{
        /// QString to char*
        char *search_c;
        QByteArray ba = search_q.toLatin1();
        search_c = ba.data();
        if(d.HasMember(search_c))
        {
            /*
            QMessageBox msgBox;
            msgBox.setText("You have seen the word.");
            msgBox.exec();
            */
            display_word(search_q); /// have searched the word, and then display it
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("It is a new word.");
            msgBox.exec();
        }
    }
}

void MainWindow::on_linkButton_clicked()
{
    QString link_q = ui->addEdit->toPlainText();
    /// empty check
    if(link_q.trimmed().isEmpty())
    {
        QMessageBox message(QMessageBox::Warning,"Warning","No link word typing",QMessageBox::Ok,NULL);
        message.exec();
    }
    else{
        /// QString to char*
        char *link_c;
        QByteArray ba = link_q.toLatin1();
        link_c = ba.data();
        if(d.HasMember(link_c))
        {
            /// get label
            QString word_label = ui->wordLabel->text();
            char *word_label_c;
            ba = word_label.toLatin1();
            word_label_c = ba.data();
            /// comparation check
            if(strcmp(word_label_c,link_c) == 0) /// notice: strcmp
            {

                QMessageBox msgBox;
                msgBox.setText("The same word, no linking.");
                msgBox.exec();
            }
            else
            {
                /// get word from d
                Value& word = d[word_label_c];
                Value& linkArray = word["link"];
                /// add new link, current: word_label_c --> link_c
                Document::AllocatorType& allocator = d.GetAllocator();
                Value link_string(kStringType);
                link_string.SetString(link_c,allocator);
                linkArray.PushBack(link_string,allocator);
/// protect link_c here
                char link_c_protected[strlen(link_c)];
                strcpy(link_c_protected,link_c);
                ui->addEdit->clear();
/// !!! very strange, after clearing addEdit, the link_c disappears !!!

                /// display the new added link in the link tableview
                wordslinkModel->setItem(n_linkwords,0,new QStandardItem(link_q));
                wordslinkModel->item(n_linkwords,0)->setTextAlignment(Qt::AlignCenter);
                n_linkwords++;
/**/
                /// opposite link
                /// link_c --> word_label_c
                Value& op_word = d[link_c_protected];
                Value& op_linkArray = op_word["link"];
                Value op_link_string(kStringType);
                op_link_string.SetString(word_label_c,allocator);
                op_linkArray.PushBack(op_link_string,allocator);
/**/
            }
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Please add the word first.");
            msgBox.exec();
        }

    }

}

void MainWindow::bubblesort()
{
    // QFile file("./wordslist.json");
    QFile file(wordslist_q);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << "{" << endl;

    string s_words[n_words];
    double s_importance[n_words];
    int s_seen[n_words];
    int i = n_words-1;
    for(Value::ConstMemberIterator it = d.MemberBegin(); it != d.MemberEnd(); it++)
    {
        s_words[i] = it->name.GetString();
        s_importance[i] = it->value["importance"].GetDouble();
        s_seen[i] = it->value["seen"].GetInt();
        i--;

    }

    for(i = n_words-1; i>0; i--)
    {
        for(int j = 0; j < i; j++)
        {
            if(s_importance[j] > s_importance[j+1] || ( (s_importance[j] == s_importance[j+1])&&(s_seen[j] < s_seen[j+1]) ))  /// Notice!! == or = !!
            {

                string s_temp = s_words[j];
                s_words[j] = s_words[j+1];
                s_words[j+1] = s_temp;
                double d_temp = s_importance[j];
                s_importance[j] = s_importance[j+1];
                s_importance[j+1] = d_temp;
                int i_temp = s_seen[j];
                s_seen[j] = s_seen[j+1];
                s_seen[j+1] = i_temp;
            }
        }

        Value& word = d[s_words[i].c_str()];
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        word.Accept(writer);
        stream << QString::fromStdString("\""+s_words[i]+"\":") << buffer.GetString() << "," << endl;
    }

    Value& word = d[s_words[i].c_str()];
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    word.Accept(writer);
    stream << QString::fromStdString("\""+s_words[i]+"\":") << buffer.GetString() << endl;
    stream << "}";
    file.close();
}

void MainWindow::pophelp() { /// slots for menu: mannual -> pop up help window
    hw.show();
}

void MainWindow::popanalysis() {
    aw.show();
    Document d_copy;
    /// reference: http://stackoverflow.com/questions/22707814/perform-a-copy-of-document-object-of-rapidjson
    d_copy.CopyFrom(d, d_copy.GetAllocator());
    aw.plotDatePath(d_copy, n_words);
}



