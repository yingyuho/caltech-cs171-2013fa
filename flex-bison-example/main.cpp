#include <iostream>
#include <list>
#include "data.h"

using namespace std;

list<Record>* parse_test(istream &datafile);

ostream& operator<<(ostream &o, Record &r) {
    o << "{ " << r.num << " ";

    list<Record>::iterator i;
    for(i = r.data->begin(); i != r.data->end(); i++)
        o << *i << " ";
    o << "}";
}

int main()
{
    list<Record>* rs = parse_test(cin);
    list<Record>::iterator i;

    for(i = rs->begin(); i != rs->end(); i++)
        cout << *i << endl;

    return 0;
}
