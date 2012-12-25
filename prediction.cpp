#include "prediction.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <ctime>
#include <fnmatch.h> // for wild card matching

// A Prediction constructor
Prediction::Prediction(Characters characters){
	// initalize characters
	characters_ = characters;
	// add test template
	Temp test0; test0["user"] = 11;
	addTemp(test0);
	
	Temp test; test["user"] = 8; test["server"] = 11;
	addTemp(test);
	
	Temp test2; test2["user"] = 8; test2["executable"] = 11; test2["arguments"] = 0;
	addTemp(test2);
	
	Temp test3; test3["user"] = 8; test3["arguments"] = 11; 
				test3["server"] = 9;  test3["priority"] = 123;
	addTemp(test3);
	/*
	Temp test4; test4["user"] = 8; test4["arguments"] = 11; 
				test4["server"] = 9;  test4["priority"] = 123; test4["executable"] = 0;
	addTemp(test4);
	*/
}

// Greedy template search algorithm
// Input: characters
// Output: template set
void
Prediction::greedyTemplateSearch(){
	int n = characters_.size();
	for(int i=1; i<n; i++ ){	
		// index vector of all combinations, do combination algorithm
		AllCombinations combs;
		combination(combs,n,i);
		// construct template set Tc (candidates)
		Tempset candidates;
		for(size_t i = 0; i < combs.size(); ++i) {
			OneCombination &one = combs[i];  Temp oneTemp;
			for(size_t j = 0; j < one.size(); ++j) 
				{ oneTemp[characters_[one[j]]] = 0; }  // 0 for dummy number, no use now.
			candidates.push_back(oneTemp);
		}// End of combination for loop
		
		// For each template in candidates, predict runtime with history app and calculate mean error
		Tempset::iterator ti; double err[candidates.size()]; int cnt=0;
		for(int i=0; i<candidates.size();i++) { err[i] = 0.0; }
		for(ti = candidates.begin(); ti != candidates.end(); ti++){
			templates_.push_back(*ti);
			// test print temp
			Temp::iterator tmpi;
			for(tmpi = (*ti).begin(); tmpi != (*ti).end(); tmpi++)
				{ std::cout << (*tmpi).first << " "; } 
			std::cout << std::endl;
			constructFromHistory(); // construct from history first
			// run prediction, be sure the history information has already in memory.
			for(int i=0;i<applications_.size();i++){
				double predict = predictRunTime(applications_[i]);
				err[cnt] += abs(applications_[i].getRuntime()-predict);
			}
			// calculate mean error
			err[cnt] = err[cnt]/applications_.size();
			std::cout << err[cnt] << std::endl;
			cnt++;
			// clean up template set and categories
			templates_.pop_back();
			categories_.clear();
			catehistapps_.clear();
		}// End of Tempset for loop
		
		// Select the lowest mean error template
		double min = std::numeric_limits<double>::max(); int idx=0;
		for(int i=0;i<candidates.size();i++){
			if(err[i] < min){
				min = err[i];
				idx = i;
			}
		}// End of comparison min mean error
		templates_.push_back(candidates[idx]);
	}
	// last combination: all characters_ in one temp
	Temp lastTemp;
	for(int i=0;i<n;i++)
		{ lastTemp[characters_[i]] = 0; }
	// add last template
	templates_.push_back(lastTemp);
	
}

// prediction algorithm
// Input: a new coming application information
// Output: run time prediction value (in minutes)
double 
Prediction::predictRunTime(Application app){
	std::vector<double> means;
	std::vector<double> CI_90s;
	// Apply templates to app, see what categories the app fall into Ca
	Tempset::iterator ti;
	for(ti = templates_.begin(); ti != templates_.end(); ti++){
		Cate Ca;
		Temp::iterator tmpi;
		// extract template and create Ca
		for(tmpi = (*ti).begin(); tmpi != (*ti).end(); tmpi++){
			if ((*tmpi).first == "user"){
				Ca.push_back(app.getUser());
			}if((*tmpi).first == "server"){
				Ca.push_back(app.getServer());
			}if((*tmpi).first == "executable"){
				Ca.push_back(app.getExecutable());
			}if((*tmpi).first == "priority"){
				Ca.push_back(app.getPriority());
			}if((*tmpi).first == "arguments"){
				//if ( wildcardMatch( app.getArguments() ) == "NULL" ) continue;
				Ca.push_back( wildcardMatch( app.getArguments() ) );
			}
		}// End of extract template for loop

		//Identify Ca whether is in categories_
		bool found = false;
		for(int i=0;i<categories_[Ca.size()].size();i++){
			if(categories_[Ca.size()][i] == Ca){
				//found! 
				found = true;
			}
		} // End of identify Ca in categories_ or not for loop

		if(!found){
			// skip prediction by this template, find next template/category
			continue;
		}

		//compute runtime estimation and a confidence interval for this category
		//minimal sample size = 2 (according to the paper description)
		const int min = 2;  int max = catehistapps_[Ca].size();
		int sampleSize=0;
		srand ( time(NULL) );
		if (max < min){
			// Check if vector size is smaller than min
			sampleSize = max;
		}else{
			sampleSize = min + (rand() % (int)(max - min + 1));
		}
		
		//Random choose sampleSize histApps from Ca
		std::random_shuffle(catehistapps_[Ca].begin(),catehistapps_[Ca].end());
		
		//Extract run times
		std::vector<double> choosenAppRunTimes;
		for(int i=0;i<sampleSize;i++){
			double runtime = applications_[catehistapps_[Ca][i]].getRuntime();
			choosenAppRunTimes.push_back(runtime);
		}
		// compute mean and CI
		double mean=0.0,std=0.0,CI_90,k=3.162;
		for(int i=0;i<choosenAppRunTimes.size();i++){
			mean += choosenAppRunTimes[i];
		}
		mean = mean/sampleSize;
		// standard deviation
		for(int i=0;i<choosenAppRunTimes.size();i++){
			std += pow((choosenAppRunTimes[i]-mean),2);
		}
		std = sqrt(std/sampleSize);
		CI_90 = k*std;

		// store CI_90 and mean for each category
		means.push_back(mean);
		CI_90s.push_back(CI_90);

	}// End of Tempset for loop

	if(means.size() == 0){
		return 0.0; // return 0 if no suitable prediction
	}
	// select the estimation with the smallest confidence interval 
	// and return run time for this app
	int minCI_90idx; double minCI_90 = std::numeric_limits<double>::max();
	for(int i=0; i<CI_90s.size(); i++){
		if(CI_90s[i] < minCI_90){
			minCI_90 = CI_90s[i];
			minCI_90idx = i; 
		}
	}
	//std::cout << "CI for 90\%: " << minCI_90 << std::endl;
	return means[minCI_90idx];
}

// construct categories from history application information
void 
Prediction::constructFromHistory(){
	// For each application
	HistAppset::iterator hi; int histAppIdx = 0;
	for(hi = applications_.begin(); hi != applications_.end(); hi++){
		// Application a: (*hi)
		// For each template, create Ca
		Tempset::iterator ti;
		for(ti = templates_.begin(); ti != templates_.end(); ti++){
			
			Cate Ca;
			Temp::iterator tmpi;
			// extract template and create Ca
			for(tmpi = (*ti).begin(); tmpi != (*ti).end(); tmpi++){
				if ((*tmpi).first == "user"){
					Ca.push_back((*hi).getUser());
				}if((*tmpi).first == "server"){
					Ca.push_back((*hi).getServer());
				}if((*tmpi).first == "executable"){
					Ca.push_back((*hi).getExecutable());
				}if((*tmpi).first == "priority"){
					Ca.push_back((*hi).getPriority());
				}if((*tmpi).first == "arguments"){ // one application, one wild card pattern now.
					Ca.push_back( wildcardMatch((*hi).getArguments()) );
				}
			}// End of extract template for loop
				
			//Identify Ca whether is in categories_
			bool found = false;
			for(int i=0;i<categories_[Ca.size()].size();i++){
				if(categories_[Ca.size()][i] == Ca){ found = true; }
			} // End of identify Ca in categories_ or not for loop
			if(!found){
				// create new Ca into categories_ 
				insertOneCate(Ca,Ca.size());
			}
			
			// insert application index into catehistapps_
			// check if too many applications in Ca, delete oldest hist app idx
			catehistapps_[Ca].push_back(histAppIdx);
			//if(catehistapps_[Ca].size() > maxhistcapacity)
			//	{ catehistapps_[Ca].erase(catehistapps_[Ca].begin()); }
			
		}// End of Tempset for loop
		histAppIdx++;
	}// End of HistAppset for loop

}

// Wild card comparison helper function
// INPUT: one cmd args
// OUTPUT: corresponding wild card pattern
// 		   "NULL" for no match at all
std::string
Prediction::wildcardMatch(std::string str){
	int ret;
	for(int i=0; i<wildcardpatterns_.size(); i++){
		ret = fnmatch(wildcardpatterns_[i].c_str(), str.c_str(),0);
		if(!ret) // found corresponding pattern
			{ return wildcardpatterns_[i]; }
	}
	// no pattern match, return "NULL"
	std::string nomatch("NULL");
	return nomatch;
}

// Add one template to templates_ and return the new templates_
Tempset 
Prediction::addTemp(Temp temp){
	templates_.push_back(temp);
	return templates_;
}

// Insert a category in Cateset map with "vecNum" characters
// Return 0 for correctness
int 
Prediction::insertOneCate(Cate category,int vecNum){
	if (category.size() != vecNum){
		std::cerr << "wrong insertion in category set!" << std::endl;
		return -1;
	}
	categories_[vecNum].push_back(category);
	return 0;
}

// Insert a history application to applications_
int
Prediction::insertOneHistApp(HistApplication histApp){
	applications_.push_back(histApp);
	return 0; // 0 for correct
}

// Print out characters, template set and category set
void
Prediction::printCharacters(){
	std::cout << "Characters:" << std::endl;
	for (int i=0;i<characters_.size();i++){
		std::cout << characters_[i] << std::endl;
	}
	std::cout << std::endl;
}

void
Prediction::printTemplates(){
	std::cout << "Template set:" << std::endl;
	Tempset::iterator ti;
	for(ti = templates_.begin(); ti != templates_.end(); ti++){
		// Maps
		Temp::iterator tempi;
		for(tempi = (*ti).begin(); tempi != (*ti).end(); tempi++){
			std::cout << (*tempi).first << " ";
		}// End of Temp for loop
		std::cout << std::endl;
	}// End of Tempset for loop
	std::cout << std::endl;
}

void
Prediction::printCategories(){
	std::cout << "Category set:" << std::endl;
	Cateset::iterator cti;
	for(cti = categories_.begin(); cti != categories_.end(); cti++){
		std::cout << "Categories with " << (*cti).first << " characters:" << std::endl;
		// Vectors of CatePerTemp
		CatePerTemp::iterator cpti;
		for(cpti = (*cti).second.begin(); cpti != (*cti).second.end(); cpti++){
			// Vectors of Cate
			Cate::iterator catei;
			for(catei = (*cpti).begin(); catei != (*cpti).end(); catei++){
				std::cout << (*catei) << " ";
			}// End of Cate for loop
			std::cout << std::endl;
			
			// print history application indexes per category
			for(int i=0;i<catehistapps_[(*cpti)].size();i++){
				std::cout << catehistapps_[(*cpti)][i] << " ";
			}
			std::cout << std::endl;
		}// End of CatePerTemp for loop
		std::cout << std::endl;
	}// End of Cateset for loop
}

//Combination helper function
/* Algorithm by Donald Knuth. */
void
Prediction::combination(AllCombinations &s, size_t n, size_t k) {
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
