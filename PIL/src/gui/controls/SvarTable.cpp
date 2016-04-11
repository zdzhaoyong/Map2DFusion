#include "SvarTable.h"

#include <base/debug/debug_config.h>
#include <base/Svar/Scommand.h>

using namespace std;

SvarTable::SvarTable(QWidget *parent,Svar &svar_ins)
    : QTableWidget(parent), m_svar(svar_ins)
{
    setObjectName((QString::fromUtf8("SvarTable")));

    setColumnCount(3);
    setColumnWidth(0, 60);
    setColumnWidth(1, 270);
    setColumnWidth(2, 440);

    QStringList slHeader;
    slHeader.append("Type");
    slHeader.append("Name");
    slHeader.append("Value");
    setHorizontalHeaderLabels(slHeader);

    connect(this, SIGNAL(cellChanged(int,int)),
            this, SLOT(act_cellChanged(int,int)));

    load();
}

void SvarTable::setValue(int row,int col,QString val)
{
    if(item(row,col)!=NULL)
    {
        item(row,col)->setText(val);
    }
    else
    {
        QTableWidgetItem* item=new QTableWidgetItem();
        item->setText(val);
        setItem(row,col,item);
    }
}

void SvarTable::load(std::list<SvarElement>& sl)
{
    setRowCount(sl.size());
    int row=0;
    loading_flag=true;

    int r,g,b;
    for(list<SvarElement>::iterator it=sl.begin();it!=sl.end();it++,row++)
    {
        const SvarElement& ele=*it;
        setValue(row,0,ele.type);
        setValue(row,1,ele.name);
        setValue(row,2,ele.value);
        if(m_svar.exist(ele.name.toStdString()))
            item(row,0)->setBackgroundColor(QColor(50,255,50));
        if(modified.count(ele.name))
            item(row,1)->setBackgroundColor(QColor(0,255,255));
        item(row,0)->setFlags(Qt::ItemIsEditable);//|
        item(row,1)->setFlags(Qt::ItemIsEditable);//|Qt::ItemIsEditable
    }
    loading_flag=false;
}

void SvarTable::load(QString filt_string)
{
    bool dofilt=false;
    if(filt_string.size()>0) dofilt=true;
    clearContents();
    std::list<SvarElement> sl;

    //load Int
    SvarWithType<int>::DataMap i_data=m_svar.i.get_data();

    for(SvarWithType<int>::DataIter it=i_data.begin(),iend=i_data.end();it!=iend;it++)
    {
        const QString& name=QString::fromStdString(it->first);
        if(dofilt)
            if(name.indexOf(filt_string,0,Qt::CaseInsensitive)<0) continue;
        sl.push_back(SvarElement("Int",
                                 name,
                                 QString("%1").arg(it->second)));
    }

    //load Double
    SvarWithType<double>::DataMap d_data=m_svar.d.get_data();
    for(SvarWithType<double>::DataIter it=d_data.begin(),iend=d_data.end();it!=iend;it++)
    {
        const QString& name=QString::fromStdString(it->first);
        if(dofilt)
            if(name.indexOf(filt_string,0,Qt::CaseInsensitive)<0) continue;
        sl.push_back(SvarElement("Double",
                                 name,
                                 QString("%1").arg(it->second)));
    }

    //load String
    SvarWithType<string>::DataMap s_data=m_svar.s.get_data();
    for(SvarWithType<string>::DataIter it=s_data.begin(),iend=s_data.end();it!=iend;it++)
    {
        const QString& name=QString::fromStdString(it->first);
        if(dofilt)
            if(name.indexOf(filt_string,0,Qt::CaseInsensitive)<0) continue;
        sl.push_back(SvarElement("String",
                                 name,
                                 QString::fromStdString(it->second)));
    }

    //load Svar
    if(svar.GetInt("Svar.ShowInTable",1))
    {
        Svar::SvarMap sv_data=m_svar.get_data();
        for(Svar::SvarIter it=sv_data.begin();it!=sv_data.end();it++)
        {
            const QString& name=QString::fromStdString(it->first);
            if(dofilt)
                if(name.indexOf(filt_string,0,Qt::CaseInsensitive)<0) continue;
            sl.push_back(SvarElement("Svar",
                                     name,
                                     QString::fromStdString(it->second)));
        }
    }

    if(svar.GetInt("Scommand.ShowInTable",0))
    {
        SvarWithType<CallbackVector>::DataMap cb_data=SvarWithType<CallbackVector>::instance().get_data();
        for(SvarWithType<CallbackVector>::DataIter it=cb_data.begin();it!=cb_data.end();it++)
        {
            const QString& name=QString::fromStdString(it->first);
            if(dofilt)
                if(name.indexOf(filt_string,0,Qt::CaseInsensitive)<0) continue;
            sl.push_back(SvarElement("Scommand",
                                     name,
                                     QString("%1").arg(it->second.size())));
        }
    }

    load(sl);
}

void SvarTable::act_cellChanged(int row, int column)
{
    if(loading_flag) return;
    if(!(item(row,0)&&item(row,1)&&item(row,2))) return;
    const QString& type=item(row,0)->text();
    const string& name=item(row,1)->text().toStdString();
    if(name=="") return ;
    bool ok=false;
    if(type=="Int")
    {
        if(!m_svar.i.exist(name)) return ;
        int value=item(row,2)->text().toInt(&ok);
        if(ok)
            m_svar.i[name]=value;
        else
            item(row,2)->setBackgroundColor(QColor(0xFF,0,0));
    }
    else if(type=="Double")
    {
        if(!m_svar.d.exist(name)) return ;

        double value=item(row,2)->text().toDouble(&ok);
        if(ok)
            m_svar.d[name]=value;
        else
            item(row,2)->setBackgroundColor(QColor(0xFF,0,0));
    }
    else if(type=="String")
    {
        if(!m_svar.s.exist(name)) return ;
        string value=item(row,2)->text().toStdString();
        m_svar.s[name]=value;
    }
    else if(type=="Svar")
    {
        if(!m_svar.exist(name)) return ;
        string value=item(row,2)->text().toStdString();
        m_svar.insert(name,value,true);
    }
    else return;
    modified.insert(item(row,1)->text());
    item(row,1)->setBackgroundColor(QColor(0,255,255));
}

SvarWidget::SvarWidget(QWidget *parent,Svar &svar_ins)
    :m_table(this,svar_ins),QWidget(parent)
{
    setupUI();
}

void SvarWidget::setupUI()
{
    // set widget
    setWindowTitle("Svar Editor");
    setMinimumSize(850, 700);

    //botton pannel
    QWidget *btnPannel = new QWidget(this);
    btnPannel->setMinimumSize(QSize(100,40));

    QLabel *labFilter = new QLabel(btnPannel);
    labFilter->setObjectName(QString::fromUtf8("labFilter"));
    labFilter->setGeometry(QRect(20, 11, 37, 16));
    labFilter->setText("Filter:");

    m_edtFilter = new QLineEdit(btnPannel);
    m_edtFilter->setObjectName(QString::fromUtf8("edtFilter"));
    m_edtFilter->setGeometry(QRect(58, 6, 330, 27));
    m_edtFilter->setFocus();

    QPushButton* m_btnLoad = new QPushButton(btnPannel);
    m_btnLoad->setObjectName(QString::fromUtf8("btnLoad"));
    m_btnLoad->setGeometry(QRect(400, 6, 85, 27));
    m_btnLoad->setText("Refresh");
    m_btnLoad->setToolTip("Reload variables");

//    QPushButton* m_btnSave = new QPushButton(btnPannel);
//    m_btnSave->setObjectName(QString::fromUtf8("btnSave"));
//    m_btnSave->setGeometry(QRect(602, 6, 85, 27));
//    m_btnSave->setText("Save");
//    m_btnSave->setToolTip("Save to file");


    // vbox layout
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    verticalLayout->addWidget(btnPannel);
    verticalLayout->addWidget(&m_table);

    // setup signals/slots
    connect(m_btnLoad, SIGNAL(clicked(bool)),
            this, SLOT(btnLoad_clicked(bool)));
    connect(m_edtFilter, SIGNAL(editingFinished()),
            this, SLOT(edtFilter_editingFinished()));
}

void SvarWidget::btnLoad_clicked(bool checked)
{
    edtFilter_editingFinished();
    m_table.hasFocus();
}

void SvarWidget::edtFilter_editingFinished(void)
{
    QString text=m_edtFilter->text();
    if(scommand.Call(text.toStdString()))
    {
        m_edtFilter->setText("");
    }
    else
    {
        if(text.indexOf("=",0,Qt::CaseInsensitive)>0)
        {
            m_table.getSvar().ParseLine(text.toStdString());
            m_edtFilter->setText("");
        }
        else
            m_table.load(text);
    }
}
