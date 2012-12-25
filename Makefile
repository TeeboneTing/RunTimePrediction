all:
	#g++ -g -Wall prediction.cpp application.cpp tracehandle.cpp -o bin/prediction
	g++ -O3 prediction.cpp application.cpp tracehandle.cpp -o bin/prediction