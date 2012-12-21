#include "prediction.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <ctime>

// A Prediction constructor
Prediction::Prediction(Characters characters){
	// initalize characters
	characters_ = characters;

	// add an "empty" template to template set 
	//Temp emp; emp["empty"] = -1;
	//templates_.push_back(emp);

	// add test template
	//Temp test0; test0["executable"] = 11;
	//addTemp(test0);
	Temp test; test["user"] = 8; test["executable"] = 11; test["server"] = 9;
	addTemp(test);
	Temp test2; test2["user"] = 8; test2["executable"] = 11;
	addTemp(test2);
	//Temp test3; test3["user"] = 8; test3["executable"] = 11; 
	//			test3["server"] = 9;  test3["priority"] = 123;
	//addTemp(test3);
	
	//templates_.push_back(test);
}

// TODO
// Greedy template search algorithm with 
// Input: characters
// Output: template set
void
Prediction::greedyTemplateSearch(){
	for(int i=1; i<=categories_.size(); i++ ){
		// construct template set Tc
		Tempset candidates;
		//next_combination(characters_.begin(),characters_.begin()+i,characters_.end());
		Tempset::iterator ti;
		for(ti = candidates.begin(); ti != candidates.end(); ti++){



		}

	}

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
			}
			//TODOs: how to handle arguments?
			//if((*tmpi).first == "arguments"){
				//Ca.push_back(app.getArguments());
			//}
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
		return 0.0;
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
	return means[minCI_90idx];
	// what if no prediction output?
	// return mean run time of whole history application?
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
				}
				//TODOs: how to handle arguments?
				//if((*tmpi).first == "arguments"){
					//Ca.push_back((*hi).getArguments());
				//}
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
				// create new Ca into categories_ 
				insertOneCate(Ca,Ca.size());
			}
			
			// insert application index into catehistapps_
			// check if too many applications in Ca, delete oldest hist app idx
			catehistapps_[Ca].push_back(histAppIdx);
			if(catehistapps_[Ca].size() > maxhistcapacity)
				{ catehistapps_[Ca].erase(catehistapps_[Ca].begin()); }
			
		}// End of Tempset for loop
		histAppIdx++;
	}// End of HistAppset for loop

}

// Add one template to templates_ and return the new templates_
Tempset 
Prediction::addTemp(Temp temp){
	templates_.push_back(temp);
	return templates_;
}

// Add one character<character name, position in the log> into characters_
// and return the new characters_
Characters 
Prediction::addOneCharacter(std::string name,int pos){
	characters_[name] = pos;
	return characters_;
}

// Get all characters_
Characters 
Prediction::getCharacters(){
	return characters_;
}

// Get one character value(position) with key (character name) from characters_
int
Prediction::getOneCharacter(std::string name){
	Characters::iterator ci;
	ci = characters_.find(name);
	return (*ci).second; // Error might occur if not found
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
	Characters::iterator ci;
	for (ci = characters_.begin(); ci != characters_.end(); ci++){
		std::cout << (*ci).first << ": " << (*ci).second << std::endl;
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