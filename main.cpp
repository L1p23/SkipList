#include <iostream>
#include "SkipList.h"
using namespace std;

/*
0 -> insert  k v
1 -> delete  k
2 -> search  k &v
3 -> update  k v
4 -> dump
5 -> load
*/
struct cmp {
    bool operator ()(const string &a, const string &b) {
        return a.length() < b.length();
    }
};
int main() {
    skiplist<string, string, cmp> sl;
    string k, v;
    int op;
    while (true) {
        cin>>op;
        if (op == 0) {
            cin>>k>>v;
            cout<<sl.insert(k, v)<<endl;
        }
        else if (op == 1) {
            cin>>k;
            cout<<sl.erase(k)<<endl;
        }
        else if (op == 2) {
            cin>>k;
            cout<<sl.search(k, v)<<endl;
            cout<<k<<':'<<v<<endl;
        }
        else if (op == 3) {
            cin>>k>>v;
            cout<<sl.update(k, v)<<endl;
        }
        else if (op == 4) {
            sl.dump_file("dump_file");
        }
        else if (op == -1)
            break;
        cout<<"size: "<<sl.size()<<endl;
        sl.display();
    }
    return 0;
}