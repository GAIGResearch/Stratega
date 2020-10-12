#pragma once
#include <iostream>
#include <string>
#include <algorithm>

namespace SGA {
	class StatSummary
	{
	private:
		std::string name;
		double sum;
		double multi;
		double sumsq;
		double min;
		double max;

		double mean;
		double median;
		double sd;

		double lastAdded;

		int numberStats;
		bool valid;

	public:
		StatSummary();
		StatSummary(std::string name);
		~StatSummary();


		void reset();

		double getMax();

		double getMin();

		double getMean();

		double sumSquareDiff();


		void computeStats();

		double getSd();

		int getNumberStats();

		double stdErr();

		void add(StatSummary ss);

		void add(double d);

		double getMulti();

		double getLastAdded();

		void discountLast(double discount);


		std::string String();

		double getSum();


	};


}
