#include <iostream>
#include <regex.h>
#include <string>

using namespace std;

int main(){
    char* pattern = "^a[[:alnum:]]";
    char* test[2];
    test[0] = "abc";
    test[1] = "bcd";
    regex_t regex;
    int ret;
    char msgbuf[1024];
    // compile regular expression with string pattern
    if( regcomp(&regex, pattern, 0) ){
        cerr << "Fail to compile.\n";
        return 1;
    }
    
    // execute regular expression
    for (int i=0; i<2; i++){
        ret = regexec(&regex, test[i], 0, NULL, 0);
        if(!ret){
            cout << test[i] <<" match regular expression: " << pattern << endl;
        }else if(ret == REG_NOMATCH){
            cout << "no match.\n";
        }else{
            regerror(ret, &regex, msgbuf, sizeof(msgbuf));
            cerr << "regex match failed: " << msgbuf << endl;
            return 1;
        }
    }
    regfree(&regex);
    return 0;
}
