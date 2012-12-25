/*
	Prediction:
	a class that predict run times from history information
	
	Teebone Ding
	2012 DEC 14
	Graduate Institute of Networking and Multimedia
	National Taiwan University
	Taipei, Taiwan

*/

#include <string>
#include <map>
#include <vector>
#include <list>
#include <utility>
#include "application.h"

// Define template set types
typedef std::vector<std::string> Characters;
typedef std::map<std::string, int> Temp;
typedef std::vector<Temp> Tempset;

// Define application information set from history log
typedef std::vector<HistApplication> HistAppset;

// Define category set types
typedef std::vector<std::string> Cate;			 // One category, many HistApplications
typedef std::vector<Cate> CatePerTemp; 			 // Categories for one templates
typedef std::map<int, CatePerTemp> Cateset;		 // All category sets
typedef std::map<Cate,std::vector<int> > CateHistAppset;// <one category,index in HistAppset>

//Capacity boundary for constructFromHistory()
//const int maxhistcapacity = 1000 ;

//Combination types
typedef std::vector<size_t> OneCombination;			 // combination indexes 
typedef std::vector<OneCombination> AllCombinations;  // vector of combination indexes

class Prediction{
public:
	//Constructor
	Prediction(Characters);				 		 // Constructor of class Prediction

	//Algorithms
	void greedyTemplateSearch(); 				 // Greedy template search algorithm with 
												 // Input: characters
												 // Output: template set

	double predictRunTime(Application app);
												 // prediction algorithm
												 // Input: a new coming application information
												 // Output: run time prediction value (in minutes)

	void constructFromHistory();				 // construct categories from history application information

	//print helper functions				 	 // Print out characters, template set and category set
	void printCategories();
	void printCharacters();
	void printTemplates();
	
	//Data structure functions
	Tempset addTemp(Temp);						 // Add one template to templates_ and return the new templates_
	int insertOneCate(Cate,int);				 // Insert a category into the specified number of vector in Cateset map
	int insertOneHistApp(HistApplication);		 // Insert a history application to applications_
	HistApplication getOneHistApp(int idx) { return applications_[idx]; }
	void addOneWildCard(std::string str) { wildcardpatterns_.push_back(str); }

private:
	Characters characters_;						 //Characters that templates could use
	Tempset templates_; 			 			 //Template set
	Cateset categories_;						 //Category set
	HistAppset applications_;					 //Application information from history log
	CateHistAppset catehistapps_;				 //History Application index (HistAppset) for one category
	std::vector<std::string> wildcardpatterns_;  //Command line arguments categories

	// combination helper function
	void combination(AllCombinations &s, size_t n, size_t k);
	
	inline void incr(OneCombination &c, size_t &x, size_t &j) { c[j] = x; j--; }

	inline void add_one_combination(OneCombination &c, size_t k, AllCombinations &s) {
		s.push_back(OneCombination(k,0));
		OneCombination &comb = s.back();

		for(size_t i = k; i >= 1; --i) comb[i-1] = c[i] - 1;
	}

	// wild card comparison helper function
	std::string wildcardMatch(std::string);
};