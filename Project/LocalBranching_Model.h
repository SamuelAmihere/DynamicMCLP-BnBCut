#pragma once
#include <ilcplex/ilocplex.h>
#include <exception>
#include <ctime>


#include "Data.h"
#include "Utils.h"
#include "MultiCutBenders_Callback.h"
#include "LocalBranching_Callback.h"
#include "LocalBranching_TwoYearCallback.h"
#include "Greedy.h"






class LocalBranching_Model
{
public:
	void SetData(const Data& newData);
	void Solve(json params);



	//Storing information about solution
	double ObjectiveValue = -1;
	ArrayXXd Solution;
	ArrayXd ValueByYear;
	map<string, int> stats;

	//Options
	bool verbose = false;
	int nTrust = 1024;
	BUDGET_TYPE budgetType = BUDGET_TYPE::Knapsack;

	

	BoolVar2D x;
	IloNumVarArray theta;

	template<typename Iter, typename RandomGenerator>
	Iter choice(Iter start, Iter end, RandomGenerator& g) {
		std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
		std::advance(start, dis(g));
		return start;
	}

private:
	Data data;


	void GetSolution(IloCplex& cplex, BoolVar2D& x) {
		for (int t = 0; t < data.T; t++) {
			for (int j_bar = 0; j_bar < data.M_bar; j_bar++) {
				Solution(t, j_bar) = cplex.getValue(x[t][j_bar]);
			}
		}
	};

	void ConvertSolution() {
		ArrayXXd temp = ArrayXXd::Constant(data.T, data.params["M"], 0.0);
		for (int t = 0; t < data.T; t++) {
			for (int j_bar = 0; j_bar < data.M_bar; j_bar++) {
				int j = data.params["station_coord"][j_bar][0];
				int k = data.params["station_coord"][j_bar][1];
				if ((Solution(t, j_bar) > 1 - EPS) && (temp(t, j) < k)) { temp(t, j) = k; }
			}
		}

		Solution = temp;
	};

};

