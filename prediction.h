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
typedef std::map<std::string, int> Characters;
typedef std::map<std::string, int> Temp;
typedef std::list<Temp> Tempset;

// Define application information set from history log
typedef std::vector<HistApplication> HistAppset;

// Define category set types
typedef std::vector<std::string> Cate;			 // One category, many HistApplications
typedef std::vector<Cate> CatePerTemp; 			 // Categories for one templates
typedef std::map<int, CatePerTemp> Cateset;		 // All category sets
typedef std::map<Cate,std::vector<int> > CateHistAppset;// <one category,index in HistAppset>

//Capacity boundary for constructFromHistory()
const int maxhistcapacity = 50 ;


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
	Characters addOneCharacter(std::string,int); // Add one character<character name, position in the log> into characters_
												 // and return the new characters_
	Characters getCharacters();					 // Get all characters_
	int getOneCharacter(std::string);	 		 // Get one character value(position) with key (character name) from characters_
	Tempset addTemp(Temp);						 // Add one template to templates_ and return the new templates_

	int insertOneCate(Cate,int);				 // Insert a category into the specified number of vector in Cateset map

	int insertOneHistApp(HistApplication);		 // Insert a history application to applications_

	HistApplication getOneHistApp(int idx) { return applications_[idx]; }
	Tempset getTemps() { return templates_; }
private:
	Characters characters_;						 //Characters that templates could use
	Tempset templates_; 			 			 //Template set
	Cateset categories_;						 //Category set
	HistAppset applications_;					 //Application information from history log
	CateHistAppset catehistapps_;				 //History Application index (HistAppset) for one category
	
	// combination helper function
	template<class RandIt>
	bool next_combination(RandIt first, RandIt mid, RandIt last){
    	typedef typename std::iterator_traits< RandIt >::value_type value_type;
    	std::sort(mid, last, std::greater< value_type >() );
    	while(std::next_permutation(first, last)){
   		    if(std::adjacent_find(first, mid, std::greater< value_type >() ) == mid){
        	    return true;
        	}
        	std::sort(mid, last, std::greater< value_type >() );
        }
    	return false;
	}

};