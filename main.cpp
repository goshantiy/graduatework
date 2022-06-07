#include <QCoreApplication>
#include <rangetree.h>

int main(int argc, char *argv[])
{
    RangeTree doc;
    doc.jsonFromFile("C:/Users/PC/Documents/graduatework/test3.json");
    doc.jsonAnalyze();
    return 0;
}
