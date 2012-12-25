/* Algorithm by Donald Knuth. */

//////////////////////////////////////////////////////////////////////////
#include <vector> // you only need this one for the class to work
//////////////////////////////////////////////////////////////////////////

#include <iostream> // for output only
#include <string> // for output only
using namespace std;

//////////////////////////////////////////////////////////////////////////
typedef vector<size_t> OneCombination;
typedef vector<OneCombination> AllCombinations;

class Combination {
public:
	static void calculate(AllCombinations &s, size_t n, size_t k) {
		s.clear(); // clear if you forgot
	
		static OneCombination c;
		if(c.size() < k+3) c.resize(k+3);
	
		size_t i, j = k, x;	
	
		// do the initialination
		for(i = 1; i <= k; ++i) c[i] = i;
		c[k+1] = n+1;
		c[k+2] = 0;
	
		do {
			add_one_combination(c, k, s);
	
			if(j > 0) {
				x = j + 1;
				incr(c, x, j);
				continue;
			}
		
			if(c[1] + 1 < c[2]) {
				c[1] += 1;
				continue;
			}
			j = 2;
		
			do {
				c[j-1] = j-1;
				x = c[j]+1;
				if(x == c[j+1]) {
					++j;
					continue;
				}
				else break;
			} while(1);
		
			if(j > k) break;
		
			incr(c, x, j);
		} while(1);
	}

private:
	inline static void incr(OneCombination &c, size_t &x, size_t &j) {
		c[j] = x;
		j--;
	}

	inline static void add_one_combination(OneCombination &c, size_t k, AllCombinations &s) {
		s.push_back(OneCombination(k,0));
		OneCombination &comb = s.back();

		for(size_t i = k; i >= 1; --i) comb[i-1] = c[i] - 1;
	}
};
//////////////////////////////////////////////////////////////////////////

int main(int, char**)
{
	AllCombinations result;
	size_t n, k;

	vector<string> example;
	example.push_back("A");
	example.push_back("B");
	example.push_back("C");
	example.push_back("D");
	example.push_back("E");
	example.push_back("F");
	example.push_back("G");
	
	cout << "Enter n = "; 
	cin >> n;
	cout << "Enter k = "; 
	cin >> k;	
	
	/***********************************/
	Combination::calculate(result, n, k);
	/***********************************/
	
	for(size_t i = 0; i < result.size(); ++i) {
		OneCombination &one = result[i];
		for(size_t j = 0; j < one.size(); ++j) {
			cout << example[one[j]];
		}
		cout << '\n';
	}
}
