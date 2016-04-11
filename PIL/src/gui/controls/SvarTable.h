#ifndef SVARTABLE_H
#define SVARTABLE_H

#include <QTableWidget>
#include <QtCore>
#include <QtGui>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QHeaderView>

#include <list>
#include <set>

#include <base/Svar/Svar.h>

class SvarWidget;

/**
 * @brief The SvarTable class
 * | type | name | value |
 */
struct SvarElement
{
    SvarElement(const QString& t,const QString& n,const QString& v,
                bool l=false,bool m=false)
        :type(t),name(n),value(v),loadFromFile(l),modified(m){}
    QString type;
    QString name;
    QString value;
    bool    loadFromFile;
    bool    modified;
};

class SvarTable : public QTableWidget
{
    Q_OBJECT

    friend class SvarWidget;

public:
    explicit SvarTable(QWidget *parent=NULL,Svar &svar_ins=svar);

    virtual  ~SvarTable(){}

    void load(QString filt_string="");
    void load(std::list<SvarElement>& sl);
    void setValue(int row,int col,QString val);
    Svar& getSvar(){return m_svar;}

protected slots:
    void act_cellChanged(int row, int column);

private:
    Svar&    m_svar;
    std::set<QString> modified;
    bool     loading_flag;
};

class SvarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SvarWidget(QWidget *parent=NULL,Svar &svar_ins=svar);
    virtual ~SvarWidget(){}

    void setupUI();

protected slots:
    void btnLoad_clicked(bool checked=false);
    void edtFilter_editingFinished(void);

public:
    QLineEdit* m_edtFilter;
    SvarTable m_table;
};
#endif // SVARTABLE_H
