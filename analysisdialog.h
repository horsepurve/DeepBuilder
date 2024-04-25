#ifndef ANALYSISDIALOG_H
#define ANALYSISDIALOG_H

/// no use for now
#include <QDialog>

/// for JSON parsing
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

#include <iostream>
using namespace std;

namespace Ui {
class AnalysisDialog;
}

class AnalysisDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnalysisDialog(QWidget *parent = 0);
    ~AnalysisDialog();
    void plotDatePath(Document &, int);

private:
    Ui::AnalysisDialog *ui;
};

//* for date parsing:
class Date {
public:
    Date(QString s) {
        //* eg: s: "2016-01-01"
        year = s.split("-")[0].toInt();
        month = s.split("-")[1].toInt();
        day = s.split("-")[2].toInt();
    }
    Date() {year = 0; month = 0; day = 0;}
    //* reference: http://stackoverflow.com/questions/550428/an-odd-c-error-test-cpp15-error-passing-const-as-this-argument-of
    int get_year() const {return year;}
    int get_month() const {return month;}
    int get_day() const {return day;}
    void print_date() {
        //! cout << year << "-" << month << "-" << day << ".   ";
    }

    bool equal2(Date d) {
        if(year == d.get_year() && month == d.get_month() && day == d.get_day())
            return true;
        else
            return false;}

    bool is_day_before(Date d) {
        if(year != d.get_year()) {
            if(year == d.get_year() - 1 && month == 12 && d.get_month() == 1 && day == 31 && d.get_day() == 1) return true;
            else return false;
        } else {
            if(month == d.get_month()) {if(day == d.get_day() -1) return true; else return false;}
            else {
                if(month == 1 && d.get_month() == 2 && day == 31 && d.get_day() == 1) return true;
                if(year%4==0 && year%100!=0 || year%400==0){
                    if(month == 2 && d.get_month() == 3 && day == 29 && d.get_day() == 1) return true;
                } else {
                    if(month == 2 && d.get_month() == 3 && day == 28 && d.get_day() == 1) return true;
                }
                if(month == 3 && d.get_month() == 4 && day == 31 && d.get_day() == 1) return true;
                if(month == 4 && d.get_month() == 5 && day == 30 && d.get_day() == 1) return true;
                if(month == 5 && d.get_month() == 6 && day == 31 && d.get_day() == 1) return true;
                if(month == 6 && d.get_month() == 7 && day == 30 && d.get_day() == 1) return true;
                if(month == 7 && d.get_month() == 8 && day == 31 && d.get_day() == 1) return true;
                if(month == 8 && d.get_month() == 9 && day == 31 && d.get_day() == 1) return true;
                if(month == 9 && d.get_month() == 10 && day == 30 && d.get_day() == 1) return true;
                if(month == 10 && d.get_month() == 11 && day == 31 && d.get_day() == 1) return true;
                if(month == 11 && d.get_month() == 12 && day == 30 && d.get_day() == 1) return true;
                return false;
            }
        }
    }

    int day2all_day() {
        int february;
        if(year%4==0 && year%100!=0 || year%400==0) february = 29;
        else february = 28;
        switch(month) {
        case 1: return day;
        case 2: return 31+day;
        case 3: return 31+february+day;
        case 4: return 31+february+31+day;
        case 5: return 31+february+31+30+day;
        case 6: return 31+february+31+30+31+day;
        case 7: return 31+february+31+30+31+30+day;
        case 8: return 31+february+31+30+31+30+31+day;
        case 9: return 31+february+31+30+31+30+31+31+day;
        case 10: return 31+february+31+30+31+30+31+31+30+day;
        case 11: return 31+february+31+30+31+30+31+31+30+31+day;
        case 12: return 31+february+31+30+31+30+31+31+30+31+30+day;
        default: printf("error!\n");
        }
    }

    int day_gap(Date d) {
        //* TODO: calculate precise day gap here, but now, there should be words added every month, otherwise error may occur
        //* sequential two years:
        int year_days = 0;
        if(d.get_year()%4==0 && d.get_year()%100!=0 || d.get_year()%400==0) year_days = 366;
        else year_days = 365;
        if(d.get_year() - this->get_year() == 1) return year_days + d.day2all_day() - this->day2all_day();
        else return d.day2all_day() - this->day2all_day();
    }

private:
    int year;
    int month;
    int day;
};

#endif // ANALYSISDIALOG_H
