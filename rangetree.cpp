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


RangeTree::Node::Node(QString statement,QMultiMap<QString,Range> _true, QMultiMap<QString,Range> _false)
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
        return true;
    return false;
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
    QPair<bool,QJsonValue> left;
    QPair<bool,QJsonValue> right;
    QMultiMap<QString,Range> *prev_ranges;
    //QString name=NULL;
    QMultiMap<QString,Range> *rangeIterator=NULL;
    QMultiMap<QString,Range> trueRange;
    QMultiMap<QString,Range> falseRange;
    Node* multiExpr=NULL;

    int switchStatement=0;
    bool branch = 1;
    int i = 0;
    if(prevRange)
    {
        prev_ranges=&nodes.top()->true_ranges;
    }
    else
    {
         prev_ranges=&nodes.top()->false_ranges;
    }
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
            multiExpr =   rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
        QString name = control.value("condition_left").toString();
        QString name_second = NULL;
        right.first=0;
        right.second=prev_ranges->values(name).last()._range.second.second;
        if(control.value("condition_right").isObject())
             multiExpr =   rangeAnalyze(control.value("condition_right").toObject(),prevRange);
        if(control.value("condition_right").isString())
        {
       left.first=0;
       left.second=control.value("condition_right");

       name_second=control.value("condition_right").toString();
       QPair<bool,QJsonValue> left_second=prev_ranges->values(name).last()._range.first;
        if(prev_ranges->values(name).last()._range.first.second.isDouble()&&(!prev_ranges->values(name).last()._range.first.first))
           left_second.second=left.second.toInt()+2;
       QPair<bool,QJsonValue> right_second=prev_ranges->values(name).last()._range.second;
       Range ranges(qMakePair(left_second,right_second));
       rangeIterator->insert(name_second,ranges);
        }
        if(control.value("condition_right").isDouble())
        {
        left.first=0;
        if(prev_ranges->values(name).last()._range.second.second.isDouble())
        {
            if(prev_ranges->values(name).last()._range.first.second.toInt()<control.value("condition_right").toInt())
            left.second=control.value("condition_right").toInt();
            else left.second=prev_ranges->values(name).last()._range.first.second;
        }
        }
        Range ranges(qMakePair(left,right));
        rangeIterator->insert(name,ranges);
       for (QMap<QString, Range>::iterator it = prev_ranges->begin(); it != prev_ranges->end(); ++it)
       {
           if((it.key()!=name)&&it.key()!=name_second)
           {
           left=it.value()._range.first;
           right=it.value()._range.second;
           Range ranges(qMakePair(left,right));
           rangeIterator->insert(it.key(),ranges);
           }
       }
       }
         break;
    }
    case LESS:
    {
        if(control.value("condition_left").isObject())
            multiExpr =   rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
        QString name = control.value("condition_left").toString();
        QString name_second = NULL;
        left.first=0;
        left.second=prev_ranges->values(name).last()._range.first.second;
        if(control.value("condition_right").isObject())
             multiExpr =   rangeAnalyze(control.value("condition_right").toObject(),prevRange);
        if(control.value("condition_right").isString())
        {
           right.first=0;
           right.second=control.value("condition_right");
                  name_second=control.value("condition_right").toString();
                  QPair<bool,QJsonValue> left_second=prev_ranges->values(name).last()._range.first;
                  if(prev_ranges->values(name).last()._range.first.second.isDouble()&&(!prev_ranges->values(name).last()._range.first.first))
                      left_second.second=left.second.toInt()+2;
                  QPair<bool,QJsonValue> right_second=prev_ranges->values(name).last()._range.second;
                  Range ranges(qMakePair(left_second,right_second));
                  rangeIterator->insert(name_second,ranges);
        }
        if(control.value("condition_right").isDouble())
        {
           right.first=0;
           if(prev_ranges->values(name).last()._range.second.second.isDouble())
           {
               if(prev_ranges->values(name).last()._range.second.second.toInt()>control.value("condition_right").toInt())
               right.second=control.value("condition_right").toInt();
               else
               {
                   throw problem::PROBLEM;
                   right.second=prev_ranges->values(name).last()._range.second.second;
               }
           }
        }
        Range ranges(qMakePair(left,right));
        rangeIterator->insert(name,ranges);
        for (QMap<QString, Range>::iterator it = prev_ranges->begin(); it != prev_ranges->end(); ++it)
        {
            if((it.key()!=name)&&(it.key()!=name_second))
            {
            left=it.value()._range.first;
            right=it.value()._range.second;
            Range ranges(qMakePair(left,right));
            rangeIterator->insert(it.key(),ranges);
            }
        }
       }
         break;

    }
    case EQUAL:
    {
        if(control.value("condition_left").isObject())
            multiExpr =   rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
        QString name = control.value("condition_left").toString();
        left.first=1;
        if(control.value("condition_right").isObject())
             multiExpr =   rangeAnalyze(control.value("condition_right").toObject(),prevRange);
        if(control.value("condition_right").isString())
        {
           right.first=1;
           left.second=control.value("condition_right");
           right.second=control.value("condition_right");
        }
        if(control.value("condition_right").isDouble())
        {
           right.first=1;
           if(prev_ranges->values(name).last()._range.first.second.isDouble())
           {
               if(prev_ranges->values(name).last()._range.first.second.toInt()>control.value("condition_right").toInt())
                   throw problem::PROBLEM;
               if(prev_ranges->values(name).last()._range.second.second.isDouble())
               {
                   if(prev_ranges->values(name).last()._range.second.second.toInt()<control.value("condition_right").toInt())
                   throw problem::PROBLEM;
               }
               right.second=control.value("condition_right").toInt();
           }
        }
        Range ranges(qMakePair(left,right));
        rangeIterator->insert(name,ranges);
        for (QMap<QString, Range>::iterator it = prev_ranges->begin(); it != prev_ranges->end(); ++it)
        {
            if(it.key()!=name)
            {
            left=it.value()._range.first;
            right=it.value()._range.second;
            Range ranges(qMakePair(left,right));
            rangeIterator->insert(it.key(),ranges);
            }
        }
       }
         break;
    }
    case GREATER_EQUAL:
    {
        if(control.value("condition_left").isObject())
            multiExpr =   rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
        QString name = control.value("condition_left").toString();
        QString name_second = NULL;
        right.first=0;
        right.second=prev_ranges->values(name).last()._range.second.second;
        if(control.value("condition_right").isObject())
             multiExpr =   rangeAnalyze(control.value("condition_right").toObject(),prevRange);
        if(control.value("condition_right").isString())
        {
       left.first=1;
       left.second=control.value("condition_right");

       name_second=control.value("condition_right").toString();
       QPair<bool,QJsonValue> left_second=prev_ranges->values(name).last()._range.first;
       if((prev_ranges->values(name).last()._range.first.second.isDouble())&&(!prev_ranges->values(name).last()._range.first.first))
           left_second.second=left.second.toInt()+2;
       QPair<bool,QJsonValue> right_second=prev_ranges->values(name).last()._range.second;
       Range ranges(qMakePair(left_second,right_second));
       rangeIterator->insert(name_second,ranges);
        }
        if(control.value("condition_right").isDouble())
        {
        left.first=1;
        if(prev_ranges->values(name).last()._range.second.second.isDouble())
        {
            if(prev_ranges->values(name).last()._range.first.second.toInt()<control.value("condition_right").toInt())
            left.second=control.value("condition_right").toInt();
            else left.second=prev_ranges->values(name).last()._range.second.second;
        }
        }
        Range ranges(qMakePair(left,right));
        rangeIterator->insert(name,ranges);
        for (QMap<QString, Range>::iterator it = prev_ranges->begin(); it != prev_ranges->end(); ++it)
        {
            if((it.key()!=name)&&(it.key()!=name_second))
            {
            left=it.value()._range.first;
            right=it.value()._range.second;
            Range ranges(qMakePair(left,right));
            rangeIterator->insert(it.key(),ranges);
            }
        }
       }
         break;
    }
    case LESS_EQUAL:
    {
        if(control.value("condition_left").isObject())
            multiExpr =   rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
        QString name = control.value("condition_left").toString();
        QString name_second = NULL;
        left.first=0;
        left.second=prev_ranges->values(name).last()._range.first.second;
        if(control.value("condition_right").isObject())
             multiExpr =   rangeAnalyze(control.value("condition_right").toObject(),prevRange);
        if(control.value("condition_right").isString())
        {
           right.first=1;
           right.second=control.value("condition_right");

                  name_second=control.value("condition_right").toString();
                  QPair<bool,QJsonValue> left_second=prev_ranges->values(name).last()._range.first;
                   if(prev_ranges->values(name).last()._range.first.second.isDouble()&&(!prev_ranges->values(name).last()._range.first.first))
                      left_second.second=left.second.toInt()+2;
                  QPair<bool,QJsonValue> right_second=prev_ranges->values(name).last()._range.second;
                  Range ranges(qMakePair(left_second,right_second));
                  rangeIterator->insert(name_second,ranges);
        }
        if(control.value("condition_right").isDouble())
        {
           right.first=1;
           if(prev_ranges->values(name).last()._range.second.second.isDouble())
           {
               if(prev_ranges->values(name).last()._range.second.second.toInt()>control.value("condition_right").toInt())
               right.second=control.value("condition_right").toInt();
               else
               {
                   throw problem::PROBLEM;
                   right.second=prev_ranges->values(name).last()._range.second.second;
               }
           }
        }
        Range ranges(qMakePair(left,right));
        rangeIterator->insert(name,ranges);
        for (QMap<QString, Range>::iterator it = prev_ranges->begin(); it != prev_ranges->end(); ++it)
        {
            if((it.key()!=name)&&(it.key()!=name_second))
            {
            left=it.value()._range.first;
            right=it.value()._range.second;
            Range ranges(qMakePair(left,right));
            rangeIterator->insert(it.key(),ranges);
            }
        }
       }
         break;
    }
    case NOT_EQUAL:
    {
        //GREATER
        if(control.value("condition_left").isObject())
            multiExpr =   rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
        QString name = control.value("condition_left").toString();
        right.first=0;
        right.second=prev_ranges->values(name).last()._range.second.second;
        if(control.value("condition_right").isObject())
             multiExpr =   rangeAnalyze(control.value("condition_right").toObject(),prevRange);
        if(control.value("condition_right").isString())
        {
       left.first=0;
       left.second=control.value("condition_right");
        }
        if(control.value("condition_right").isDouble())
        {
        left.first=0;
        if(prev_ranges->values(name).last()._range.second.second.isDouble())
        {
            if(prev_ranges->values(name).last()._range.first.second.toInt()<control.value("condition_right").toInt())
            left.second=control.value("condition_right").toInt();
            else left.second=prev_ranges->values(name).last()._range.second.second;
        }
        }
        Range ranges(qMakePair(left,right));
        rangeIterator->insert(name,ranges);

        //LESS
        if(control.value("condition_left").isObject())
            multiExpr =   rangeAnalyze(control.value("condition_left").toObject(),prevRange);
        if(control.value("condition_left").isString())
        {
        QString name = control.value("condition_left").toString();
        left.first=0;
        left.second=prev_ranges->values(name).last()._range.first.second;
        if(control.value("condition_right").isObject())
             multiExpr =   rangeAnalyze(control.value("condition_right").toObject(),prevRange);
        if(control.value("condition_right").isString())
        {
           right.first=0;
           right.second=control.value("condition_right");
        }
        if(control.value("condition_right").isDouble())
        {
           right.first=0;
           if(prev_ranges->values(name).last()._range.second.second.isDouble())
           {
               if(prev_ranges->values(name).last()._range.second.second.toInt()>control.value("condition_right").toInt())
               right.second=control.value("condition_right").toInt();
               else
               {
                   throw problem::PROBLEM;
                   right.second=prev_ranges->values(name).last()._range.second.second;
               }
           }
        }
        Range ranges(qMakePair(left,right));
        rangeIterator->insert(name,ranges);
        for (QMap<QString, Range>::iterator it = prev_ranges->begin(); it != prev_ranges->end(); ++it)
        {
            if(it.key()!=name)
            {
            left=it.value()._range.first;
            right=it.value()._range.second;
            Range ranges(qMakePair(left,right));
            rangeIterator->insert(it.key(),ranges);
            }
        }
       }
         break;
    }
    }
    }
    i++;
    branch = 0;
    }while(i<2);
    Node *node=new Node(control.value("condition").toString(),trueRange,falseRange);
    if(multiExpr)
        multiExpr->right=node;
    nodes.push(node);
    printRanges(node);
    if(multiExpr)
        return multiExpr;
    return node;

}
void RangeTree::printRanges(Node * node)
{
    qDebug()<<node->_statement;
    qDebug()<<"true:";
    for (QMap<QString, Range>::iterator it = node->true_ranges.begin(); it != node->true_ranges.end(); ++it)
    {
   std::cout<<it.key().toStdString();
    if(!it.value()._range.first.first)
        std::cout<<"(";
    else std::cout<<"[";
    if(it.value()._range.first.second.isString())
    std::cout<<it.value()._range.first.second.toString().toStdString()<<' ';
    if(it.value()._range.first.second.isDouble())
        std::cout<<it.value()._range.first.second.toInt()<<' ';
    if(it.value()._range.second.second.isString())
    std::cout<<it.value()._range.second.second.toString().toStdString();
    if(it.value()._range.second.second.isDouble())
        std::cout<<it.value()._range.second.second.toInt();
    if(!it.value()._range.second.first)
        std::cout<<")";
    else std::cout<<"]";
    std::cout<<'\n';
    }
    qDebug()<<"false:";
    for (QMap<QString, Range>::iterator it = node->false_ranges.begin(); it != node->false_ranges.end(); ++it)
    {
   std::cout<<it.key().toStdString();
    if(!it.value()._range.first.first)
        std::cout<<"(";
    else std::cout<<"[";
    if(it.value()._range.first.second.isString())
    std::cout<<it.value()._range.first.second.toString().toStdString()<<' ';
    if(it.value()._range.first.second.isDouble())
        std::cout<<it.value()._range.first.second.toInt()<<' ';
    if(it.value()._range.second.second.isString())
    std::cout<<it.value()._range.second.second.toString().toStdString();
    if(it.value()._range.second.second.isDouble())
        std::cout<<it.value()._range.second.second.toInt();
    if(!it.value()._range.second.first)
        std::cout<<")";
    else std::cout<<"]";
    std::cout<<'\n';
    }
    std::cout<<'\n';
}


void RangeTree::initRanges()
{
    QMultiMap<QString,Range> init_ranges;
    QMapIterator<QString, int> iterator_int(int_map);
    while(iterator_int.hasNext())//в цикле заполняем словарь инициализации значений переменных, имя - (INT_MIN,INT_MAX)
    {
             iterator_int.next();
             QPair<bool, QJsonValue> left(false,INT_MIN);
             QPair<bool, QJsonValue> right(false,INT_MAX);
             QPair<QPair<bool, QJsonValue>, QPair<bool, QJsonValue> > pair (left,right);
             Range range (pair);
             init_ranges.insert(iterator_int.key(),range);
    }
    QMapIterator<QString, double> iterator_double(double_map);
    while(iterator_double.hasNext())//в цикле заполняем словарь инициализации значений переменных, имя - (INT_MIN,INT_MAX)
    {
        iterator_int.next();
        QPair<bool, QJsonValue> left(false,INT_MIN);
        QPair<bool, QJsonValue> right(false,INT_MAX);
        QPair<QPair<bool, QJsonValue>, QPair<bool, QJsonValue> > pair (left,right);
        Range range (pair);
        init_ranges.insert(iterator_int.key(),range);
    }
    root = new Node("init",init_ranges,init_ranges);
    printRanges(root);
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
RangeTree::Range::Range(QPair<QPair<bool, QJsonValue>, QPair<bool, QJsonValue> > range)
{
    _range=range;
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
        try
        {
        controlAnalyze(_control,root,1);
        }
        catch (problem)
        {
        std::cout<<"problem";
        }
        }
    }
 }
}






