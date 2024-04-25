//* note of comments:
//* //* is important comments, explains something
//* /**/ is used for unit testing
//* //can be deleted, just as a remainder
//* //! is useful cout, for debugging

#include "analysisdialog.h"
#include "ui_analysisdialog.h"

//* for sort:
#include <algorithm>

AnalysisDialog::AnalysisDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnalysisDialog)
{
    ui->setupUi(this);
    this->setFixedSize(630,350);
}

AnalysisDialog::~AnalysisDialog()
{
    delete ui;
}

//* used for sort dates:
bool date_compare(Date const & a, Date const & b) {
    if (a.get_year() == b.get_year()) {
        if (a.get_month() == b.get_month()) {
            return a.get_day() < b.get_day();
        }
        else return a.get_month() < b.get_month();
    }
    else return a.get_year() < b.get_year();
}

//* plot the date path:
void AnalysisDialog::plotDatePath(Document & d_copy, int n_words) {
    Date words_dates[n_words];
    //* ========================================
    //* construct the words_dates:
    //* ========================================
    int index = 0;
    for(Value::ConstMemberIterator it = d_copy.MemberBegin(); it != d_copy.MemberEnd(); it++)
    {
        QString qdate = it->value["date"].GetString();
        words_dates[index] = Date(qdate);
        index++;
    }
    for(int i = 0; i < n_words; i++) {words_dates[i].print_date();}
    //* sort the dates
    //* reference: http://stackoverflow.com/questions/12823573/c-sorting-class-array
    std::sort(words_dates, words_dates+n_words, date_compare);
    //* print to see the sorted
    //! printf("\n--- sorted: ---\n");
    for(int i = 0; i < n_words; i++) {words_dates[i].print_date();}
    //! printf("\n--- sorted end ---\n");

    //* ========================================
    //* fill in the date counts:
    //* ========================================
    int date_width = 60; //* we'll see the data of two month
    QVector<double> keyData(date_width);
    QVector<double> valueData(date_width);
    //* we begin from the latest date:
    //* printf("\nTo date:\n");
    //* words_dates[n_words - 1].print_date();
    index = date_width - 1; //* the position of the latest
    keyData[index] = index + 1;
    valueData[index] = 0;
    valueData[index]++;
    for(int i = n_words - 2; i >= 0; i--) { //* we begin from the one before the latest date
        if(words_dates[i].equal2(words_dates[i+1])) {
            //! printf("same day:\n");
            words_dates[i].print_date();
            words_dates[i+1].print_date();
            valueData[index]++;
        }
        else if(words_dates[i].is_day_before(words_dates[i+1])) {
            //! printf("one day before:\n");
            words_dates[i].print_date();
            if(index == 0) {
                //! printf("\nFrom date:\n");
                words_dates[i].print_date();
                break;
            }
            index--;
            //! printf("index:%d\n", index);
            keyData[index] = index+1;
            valueData[index] = 0;
            valueData[index]++;
        }
        else {
            //* there are some gaps between days
            //! printf("gap day:\n");
            words_dates[i].print_date();
            //! printf("gap number: %d\n", words_dates[i].day_gap(words_dates[i+1]));
            //* Note: for in for, break and break
            //! printf("index:%d\n", index);
            if(index == 0) {
                //! printf("\nFrom date:\n");
                words_dates[i+1].print_date(); //* we already have index 0
                break;
            }
            //! printf("gap:%d ", words_dates[i].day_gap(words_dates[i+1]));
            for(int j = 0; j < words_dates[i].day_gap(words_dates[i+1]); j++) {
                //! printf("now fill the gap:\n");
                if(index == 0) {
                    //! printf("\nFrom date:\n");
                    words_dates[i].print_date();
                    break;
                }
                index--;
                //! printf("gap day:%d\n", index);
                keyData[index] = index+1;
                valueData[index] = 0;
            }
            //* the last time of fill gap:
            valueData[index] = 1;
        }
    }

    //* ========================================
    //* plot the bar plot
    //* ========================================
    QCPBars *myBars = new QCPBars(ui->customPlotDate->xAxis, ui->customPlotDate->yAxis);
    // now we can modify properties of myBars:
    myBars->setName("Bars Series 1");
    /*
    QVector<double> keyData1;
    QVector<double> valueData1;
    keyData1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    valueData1 = {2, 4, 8, 2, 3, 4, 5, 1, 2, 3};
    */
    myBars->setData(keyData, valueData);
    ui->customPlotDate->rescaleAxes();
    // give the axes some labels:
    ui->customPlotDate->xAxis->setLabel("Recent Two Months");
    ui->customPlotDate->yAxis->setLabel("Words Added");
    // set axes ranges, so we see all data:
    ui->customPlotDate->xAxis->setRange(0, 61);
    //* customPlot->yAxis->setRange(0, 1);
    ui->customPlotDate->replot();
}
