#include "rangetree.h"
void RangeTree::jsonFromFile(QString way)//считывание файла из JSON документа, на вход подается путь.
{
        QFile jsonFile;
        jsonFile.setFileName(way);
        QString test=jsonFile.fileName();
        qDebug()<<test;
        if(!jsonFile.exists())
        {
            qDebug()<<"file not exists";
        }
       if(jsonFile.open(QFile::ReadOnly))
           {
           QByteArray file=jsonFile.readAll();
            _doc = QJsonDocument::fromJson(file,&err);//считываем json
            jsonFile.close();
           }
}

void RangeTree::initAnalyze()//анализ массива инициализации в JSON документе и занесение их в словарь.
{
QJsonArray init_list=_function.value("init_list").toArray();
for(int i=0;i<init_list.count();i++)
{
    QString init_type=init_list.at(i).toObject().value("init_type").toString();
    switch(INIT_TYPE.value(init_type))
        {
        case (init_type::INT):
        {
            QString name=init_list.at(i).toObject().value("init_name").toString();//достаём имя переменной
            int ix=NULL;
            if(!init_list.at(i).toObject().value("init").isUndefined())
            ix=init_list.at(i).toObject().value("init").toInt();//достаём значение
            int_map.insert(name,ix);//вставляем в мап
            init_map.insert(name,init_type::INT);
            break;
        };
        case (init_type::INT_PTR):
        {
        QJsonArray arr;
        arr=init_list.at(i).toObject().value("init").toArray();
        if(arr.count())
        {
        QString name=init_list.at(i).toObject().value("init_name").toString();//достаём имя переменной
        QVector<int> mass;//создаём вектор
        for(int i=0;i<arr.count();i++)
        mass.append(arr.at(i).toInt());//заполняем значениями из json
        int_ptr_map.insert(name,mass);//вставляем в мап
        init_map.insert(name,init_type::INT_PTR);
        }

        break;
        }
        case (init_type::DOUBLE):
        {
        QString name=init_list.at(i).toObject().value("init_name").toString();//достаём имя переменной
        double dx=NULL;
        if(!init_list.at(i).toObject().value("init").isUndefined())
        dx=init_list.at(i).toObject().value("init").toDouble();//достаём значение
        double_map.insert(name,dx);//вставляем в мап
        init_map.insert(name,init_type::DOUBLE);
        break;
        }
        case (init_type::DOUBLE_PTR):
        {
        QJsonArray arr;
        arr=init_list.at(i).toObject().value("init").toArray();
        if(arr.count())
        {
        QString name=init_list.at(i).toObject().value("init_name").toString();//достаём имя переменной
        QVector<double> mass;//создаём вектор
        for(int i=0;i<arr.count();i++)
        mass.append(arr.at(i).toDouble());//заполняем значениями из json
        double_ptr_map.insert(name,mass);//вставляем в мап
        init_map.insert(name,init_type::DOUBLE_PTR);
        }
        break;
        }
        default:
        {
        break;
        }
        }
}
}


RangeTree::RangeTree()
{
    root=NULL;
}


RangeTree::Node::Node(QString statement,QList<Range> _true, QList<Range> _false)
{
    _statement=statement;
    true_ranges = _true;
    false_ranges = _false;
    left=NULL;
    right=NULL;
}
bool RangeTree::isEmpty()
{
    if(root==NULL)
        return false;
    return true;
}

RangeTree::Range::Range(QString name, QPair<QPair<bool, QString>, QPair<bool, QString> > range)
{
    _name=name;
    _range=range;
}

RangeTree::~RangeTree()
{
clear(root);
delete nodes.top();
nodes.pop();
root=NULL;
}

RangeTree::Node* RangeTree::rangeAnalyze(QJsonObject control,bool prevRange)
{
    QPair<bool,QString> left;
    QPair<bool,QString> right;
    QList<Range> *prev_ranges=NULL;
    QString name=NULL;
    QList<Range> *rangeIterator=NULL;
    QList<Range> trueRange;
    QList<Range> falseRange;
    int switchStatement=0;
    bool branch = 1;
    int i = 0;
    if(prevRange)
        prev_ranges=&nodes.top()->true_ranges;
    else prev_ranges=&nodes.top()->false_ranges;
    do
    {
            if(branch)
            {

                switchStatement=CONDITION_TYPE.value(control.value("condition_type").toString());
                rangeIterator=&trueRange;
            }
            else
            {

                switchStatement=CONDITION_INVERSE.value(CONDITION_TYPE.value(control.value("condition_type").toString()));
                rangeIterator=&falseRange;
            }
    switch(switchStatement)
    {
    case GREATER:
    {
        if(control.value("condition_left").isObject())
                rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
        QString name = control.value("condition_left").toString();
        foreach(Range value, *prev_ranges)
        {
        if(value._name==name)
        {
        right=value._range.second;
        break;
        }
        }
        if(control.value("condition_right").isObject())
                rangeAnalyze(control.value("condition_right").toObject(),prevRange);
        if(control.value("condition_right").isString())
        {
       left.first=0;
       left.second=control.value("condition_right").toString();
        }
        if(control.value("condition_right").isDouble())
        {
        left.first=0;
        left.second=QString::number(control.value("condition_right").toDouble());
        }
        QPair<QPair<bool,QString>,QPair<bool,QString> > pair(left,right);
        Range ranges(name,pair);
        rangeIterator->append(ranges);
        foreach(Range value, *prev_ranges )
        {
            if(value._name!=name)
            {
            QString names=value._name;
            left=value._range.first;
            right=value._range.second;
            QPair<QPair<bool,QString>,QPair<bool,QString> > pair(left,right);
            Range ranges(names,pair);
            rangeIterator->append(ranges);
            break;
            }
        }
        }
        break;
    }
    case LESS:
    {
        if(control.value("condition_left").isObject())
                rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
        QString name = control.value("condition_left").toString();
        foreach(Range value, *prev_ranges)
        {
        if(value._name==name)
        {
        left=value._range.first;
        break;
        }
        }
        if(control.value("condition_right").isObject())
                rangeAnalyze(control.value("condition_right").toObject(),prevRange);
        if(control.value("condition_right").isString())
        {
        right.first=0;
        right.second=control.value("condition_right").toString();
        }
        if(control.value("condition_right").isDouble())
        {
        right.first=0;
        right.second=QString::number(control.value("condition_right").toDouble());
        }
        QPair<QPair<bool,QString>,QPair<bool,QString> > pair(left,right);
        Range ranges(name,pair);
        rangeIterator->append(ranges);
        foreach(Range value, *prev_ranges )
        {
            if(value._name!=name)
            {
            QString names=value._name;
            left=value._range.first;
            right=value._range.second;
            QPair<QPair<bool,QString>,QPair<bool,QString> > pair(left,right);
            Range ranges(names,pair);
            rangeIterator->append(ranges);
            break;
            }
        }
    }
        break;
    }
    case EQUAL:
    {
        QString name;

        if(control.value("condition_left").isObject())
                rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
            name = control.value("condition_left").toString();
            if(control.value("condition_right").isObject())
                    rangeAnalyze(control.value("condition_right").toObject(),prevRange);
            if(control.value("condition_right").isString())
            {
            left.first=1;
            left.second=control.value("condition_right").toString();
            right.first=1;
            right.second=control.value("condition_right").toString();
            }
            if(control.value("condition_right").isDouble())
            {
                left.first=1;
                left.second=QString::number(control.value("condition_right").toDouble());
                right.first=1;
                right.second=QString::number(control.value("condition_right").toDouble());

            }
        }
        QPair<QPair<bool,QString>,QPair<bool,QString> > pair(left,right);
        Range ranges(name,pair);
        rangeIterator->append(ranges);
        foreach(Range value, *prev_ranges)
        {
            if(value._name!=name)
            {
            QString names=value._name;
            left=value._range.first;
            right=value._range.second;
            QPair<QPair<bool,QString>,QPair<bool,QString> > pair(left,right);
            Range ranges(names,pair);
            rangeIterator->append(ranges);
            break;
            }
        }
        break;
    }
    case GREATER_EQUAL:
    {
        if(control.value("condition_left").isObject())
                rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
        QString name = control.value("condition_left").toString();
        foreach(Range value, *prev_ranges)
        {
        if(value._name==name)
        {
        right=value._range.second;
        break;
        }
        }
        if(control.value("condition_right").isObject())
                rangeAnalyze(control.value("condition_right").toObject(),prevRange);
        if(control.value("condition_right").isString())
        {      
            left.second = control.value("condition_right").toString();
            left.first=1;
        }
        if(control.value("condition_right").isDouble())
        {
        left.first=1;
        left.second=QString::number(control.value("condition_right").toDouble());
        }
        QPair<QPair<bool,QString>,QPair<bool,QString> > pair(left,right);
        Range ranges(name,pair);
        rangeIterator->append(ranges);
        foreach(Range value, *prev_ranges )
        {
            if(value._name!=name)
            {
            QString names=value._name;
            left=value._range.first;
            right=value._range.second;
            QPair<QPair<bool,QString>,QPair<bool,QString> > pair(left,right);
            Range ranges(names,pair);
            rangeIterator->append(ranges);
            break;
            }
        }
        }
        break;
    }
    case LESS_EQUAL:
    {
        if(control.value("condition_left").isObject())
                rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
        QString name = control.value("condition_left").toString();
        foreach(Range value, *prev_ranges)
        {
        if(value._name==name)
        {
        left=value._range.first;
        break;
        }
        }
        if(control.value("condition_right").isObject())
                rangeAnalyze(control.value("condition_right").toObject(),prevRange);
        if(control.value("condition_right").isString())
        {

            right.second=control.value("condition_right").toString();
            right.first=1;
        }
        if(control.value("condition_right").isDouble())
        {
        right.first=1;
        right.second=QString::number(control.value("condition_right").toDouble());
        }
        QPair<QPair<bool,QString>,QPair<bool,QString> > pair(left,right);
        Range ranges(name,pair);
        rangeIterator->append(ranges);
        foreach(Range value, *prev_ranges )
        {
            if(value._name!=name)
            {
            QString names=value._name;
            left=value._range.first;
            right=value._range.second;
            QPair<QPair<bool,QString>,QPair<bool,QString> > pair(left,right);
            Range ranges(names,pair);
            rangeIterator->append(ranges);
            break;
            }
        }
    }
        break;
    }
    case NOT_EQUAL:
    {
        break;
    }
    }

    i++;
    branch = 0;
    }while(i<2);
    Node *node=new Node(control.value("condition").toString(),trueRange,falseRange);
    nodes.push(node);
    qDebug()<<control.value("condition").toString();
    qDebug()<<"true:";
    foreach(Range value, nodes.top()->true_ranges)
    {
   std::cout<<value._name.toStdString();
    if(!value._range.first.first)
        std::cout<<"(";
    else std::cout<<"[";
    std::cout<<value._range.first.second.toStdString()<<' ';
    std::cout<<value._range.second.second.toStdString();
    if(!value._range.second.first)
        std::cout<<")";
    else std::cout<<"]";
    std::cout<<'\n';
    }
    qDebug()<<"false:";
    foreach(Range value, nodes.top()->false_ranges)
    {
   std::cout<<value._name.toStdString();
    if(!value._range.first.first)
        std::cout<<"(";
    else std::cout<<"[";
    std::cout<<value._range.first.second.toStdString()<<' ';
    std::cout<<value._range.second.second.toStdString();
    if(!value._range.second.first)
        std::cout<<")";
    else std::cout<<"]";
    std::cout<<'\n';
    }
    std::cout<<'\n';
    return node;



}


void RangeTree::initRanges()
{
    QList<Range> init_ranges;
    QMapIterator<QString, int> iterator_int(int_map);
    while(iterator_int.hasNext())//в цикле заполняем словарь инициализации значений переменных, имя - (INT_MIN,INT_MAX)
    {
             iterator_int.next();
             QPair<bool, QString> left(false,"INT_MIN");
             QPair<bool, QString> right(false,"INT_MAX");
             QPair<QPair<bool, QString>, QPair<bool, QString> > pair (left,right);
             Range range(iterator_int.key(), pair);
             init_ranges.append(range);
    }
    QMapIterator<QString, double> iterator_double(double_map);
    while(iterator_double.hasNext())//в цикле заполняем словарь инициализации значений переменных, имя - (INT_MIN,INT_MAX)
    {
             iterator_int.next();
             QPair<bool, QString> left(false,"INT_MIN");
             QPair<bool, QString> right(false,"INT_MAX");
             QPair<QPair<bool, QString>, QPair<bool, QString> > pair (left,right);
             Range range(iterator_int.key(), pair);
             init_ranges.append(range);
    }
    root = new Node("init",init_ranges,init_ranges);
}

void RangeTree::clear(Node *node)
{
    if(node)
    {
        if(node->right)
       clear(node->right);
        if(node->left)
        clear(node->left);
        delete node;
    }
}

RangeTree::Node* RangeTree::controlAnalyze(QJsonObject control, Node* iter, bool branch)
{
if(nodes.size()==2)
{
    root=nodes.top();
}
iter = rangeAnalyze(control,branch);
    if(!control.value("right_child").isUndefined())
    {
Node *iterator_right = iter;
iterator_right->right=controlAnalyze(control.value("right_child").toObject(),iterator_right,1);
    }
   if(!control.value("left_child").isUndefined())
   {
Node *iterator_left = iter;
iterator_left->left=controlAnalyze(control.value("left_child").toObject(),iterator_left,0);
   }
   nodes.pop();
  return iter;
}

void RangeTree::jsonAnalyze()//анализ JSON документа
{
 if(err.errorString().toInt()==QJsonParseError::NoError)//если нормально прочитали документ
 {
    _array=QJsonValue(_doc.object().value("functions")).toArray();//достаем массив функций

    if(!_array.isEmpty())//проверяем на пустоту
    {
        _function=_array.at(_array.count()-1).toObject();//достаем функцию main
        _control = _function.value("control_structure").toObject();//приводим к обьекту
        initAnalyze();//инициализируем переменные
        initRanges();
        if(root)
        {
        nodes.push(root);
        controlAnalyze(_control,root,1);
        }
    }
 }
}


