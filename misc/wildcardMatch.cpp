/* wildcard pattern match example */
#include <iostream>
#include <fnmatch.h>

using namespace std;

int main(){
    string pattern = "send * packet(s) to * addr.";
    string test[2];
    test[0] = "send 3 packet(s) to 127.0.0.1 addr.";
    test[1] = "send sdf packet";

    int ret;
    for(int i=0;i<2;i++){
        //cout << fnmatch(pattern.c_str(), test[i].c_str(),0) << endl;
        ret = fnmatch(pattern.c_str(), test[i].c_str(),0);
        if(!ret)
            { cout << test[i] << " matches pattern: " << pattern << endl; }
        else
            { cout << "no match.\n"; }
    }
    return 0;
}
