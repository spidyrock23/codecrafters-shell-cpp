#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int32_t main()
{
    auto string_conv = [&](string s)
    {
        int n = s.length();
        vector<string> fin_ans;
        string ans = "";
        string current_string = s;
        int flag = 0;
        int flag2 = 0;
        for (int i = 0; i < current_string.size(); i++)
        {
            char itr = current_string[i];
            if (!flag && itr == '\"')
            {
                flag2 ^= 1;
                continue;
            }
            if (!flag2 && itr == '\'')
            {
                flag ^= 1;
                continue;
            }
            if (itr != ' ')
            {
                if (itr != '\\')
                {
                    ans += itr;
                    continue;
                }
                if (flag || flag2)
                {
                    if (flag)
                    {
                        ans += itr;
                        continue;
                    }
                    else
                    {
                        char nxt = current_string[i + 1];
                        ans += nxt;
                        i++;
                    }
                }
                else
                {
                    char nxt = current_string[i + 1];
                    ans += nxt;
                    i++;
                }
                continue;
            }
            if (flag || flag2)
            {
                ans += itr;
            }
            else
            {
                if (ans.size())
                {
                    fin_ans.push_back(ans);
                    ans = "";
                }
            }
        }
        if (ans != "")
        {
            fin_ans.push_back(ans);
        }
        return fin_ans;
    };
    string s;
    getline(cin, s);
    auto fun = string_conv(s);
    for(auto it: fun){
        cout << it << endl;
    }
    //cout << fun.size() << endl;
    //cout << endl;
    return 0;
}