#ifndef RANGETREE_H
#define RANGETREE_H
#include <QString>
#include <QPair>
#include <QStack>
#include <QMap>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <iostream>
class RangeTree
{
private:
    class Range
    {
    public:
        QPair<QPair<bool,QJsonValue>,QPair<bool,QJsonValue> > _range;
        Range(QPair< QPair<bool,QJsonValue>,QPair<bool,QJsonValue> >);// bool   0 - ) 1 - ]
    };

    class Node
    {
    public:
        Node(QString,QMultiMap<QString,Range>,QMultiMap<QString,Range>);
        QString _statement;
        QMultiMap<QString,Range> true_ranges;
        QMultiMap<QString,Range> false_ranges;
        Node *left;
        Node *right;
    };

    QStack<Node*> nodes;
    Node *root;
    Node *iter;


    enum statemens
    {
        FOR,
        IF,
        WHILE

    };
    enum init_type
    {
       INT,
       DOUBLE,
       INT_PTR,
       DOUBLE_PTR,
       CALL_FUNCTION
    };
    enum condition_type
    {
        EQUAL,
        NOT_EQUAL,
        AND,
        OR,
        GREATER,
        LESS_EQUAL,
        LESS,
        GREATER_EQUAL
    };
    enum operations
    {
        PLUS,
        MINUS,
        MULTIPLY,
        DIVISION,
        ASSIGMENT,
        MOD
    };
    enum class problem
    {
       PROBLEM
    };

    QMap<QString,statemens> STATEMENTS
    {
        {"FOR",statemens::FOR},
        {"IF",statemens::IF},
        {"WHILE",statemens::WHILE}
    };
    QMap<QString,init_type> INIT_TYPE
    {
        {"CALL_FUNCTION",init_type::CALL_FUNCTION},
        {"INT",init_type::INT},
        {"INT_PTR",init_type::INT_PTR},
        {"DOUBLE",init_type::DOUBLE},
        {"DOUBLE_PTR",init_type::DOUBLE_PTR}
    };
    QMap<QString,condition_type> CONDITION_TYPE
    {
        {"EQUAL",condition_type::EQUAL},
        {"NOT_EQUAL",condition_type::NOT_EQUAL},
        {"AND",condition_type::AND},
        {"OR",condition_type::OR},
        {"GREATER",condition_type::GREATER},
        {"LESS",condition_type::LESS},
        {"GREATER_EQUAL",condition_type::GREATER_EQUAL},
        {"LESS_EQUAL",condition_type::LESS_EQUAL}
    };
    QMap<condition_type,condition_type> CONDITION_INVERSE
    {
      {condition_type::NOT_EQUAL,condition_type::EQUAL},
      {condition_type::EQUAL,condition_type::NOT_EQUAL},
      {condition_type::OR,condition_type::AND},
      {condition_type::AND,condition_type::OR},
      {condition_type::LESS_EQUAL,condition_type::GREATER},
      {condition_type::GREATER,condition_type::LESS_EQUAL},
      {condition_type::GREATER_EQUAL,condition_type::LESS},
      {condition_type::LESS,condition_type::GREATER_EQUAL}
    };
    QMap<QString,operations> OPERATIONS
    {
        {"PLUS",operations::PLUS},
        {"MINUS",operations::MINUS},
        {"MULTIPLY",operations::MULTIPLY},
        {"DIVISION",operations::DIVISION},
        {"ASSIGMENT",operations::ASSIGMENT},
        {"MOD",operations::MOD}
    };
    QJsonObject _control;
    QJsonArray _array;
    QString functionName;
    QJsonDocument _doc;
    QJsonParseError err;
    QJsonObject _function;
    QMap<QString, init_type> init_map;//словарь инициализаций(если переменная инициализирована в JSON)
    QMap<QString,int> int_map;
    QMap<QString,double> double_map;
    QMap<QString,QVector<int> > int_ptr_map;
    QMap<QString,QVector<double> > double_ptr_map;
public:
    RangeTree();
    ~RangeTree();
    bool isEmpty();

    void jsonFromFile(QString);
    void initAnalyze();
    void jsonAnalyze();
    Node* controlAnalyze(QJsonObject, Node*,bool);
    Node* rangeAnalyze(QJsonObject,bool);
    void initRanges();
    void clear(Node*);
    void printRanges(Node*);
};

#endif // RANGETREE_H
