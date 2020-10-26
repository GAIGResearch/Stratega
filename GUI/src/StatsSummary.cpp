#include <algorithm>
#include <StatsSummary.h>



StatSummary::~StatSummary()
{
}

StatSummary::StatSummary() {
	this->name = "";
	numberStats = 0;
	sum = 0;
	multi = 0;
	sumsq = 0;

	min = +INFINITY;
	max = -INFINITY;

	valid = false;
}
StatSummary::StatSummary(std::string name) {
	this->name = name;
	numberStats = 0;
	sum = 0;
	sumsq = 0;

	min = +INFINITY;
	max = -INFINITY;

	valid = false;
}

void StatSummary::reset() {
	numberStats = 0;
	sum = 0;
	sumsq = 0;

	min = +INFINITY;
	max = -INFINITY;

	valid = false;
}

double StatSummary::getMax() {
	return max;
}

double StatSummary::getMin() {
	return min;
}

double StatSummary::getMulti() {
	return multi;
}

double StatSummary::getMean() {
	if (valid)
		computeStats();

	return mean;
}

double StatSummary::sumSquareDiff() {
	return sumsq - numberStats * getMean() * getMean();
}


void StatSummary::computeStats() {
	if (!valid) {
		mean = sum / numberStats;
		double num = sumsq - (numberStats * mean * mean);
		if (num < 0) {
			// avoids tiny negative numbers possible through imprecision
			num = 0;
		}


		sd = std::sqrt(num / ((double)numberStats - 1));

		valid = true;
	}
}

double StatSummary::getSd() {
	if (!valid)
		computeStats();
	return sd;
}

int StatSummary::getNumberStats() {
	return numberStats;
}

double StatSummary::stdErr() {
	return getSd() / std::sqrt(numberStats);
}

void StatSummary::add(StatSummary ss) {
	// implications for Watch?
	numberStats += ss.numberStats;
	sum += ss.sum;
	sumsq += ss.sumsq;
	multi += ss.multi;
	max = std::max(max, ss.max);
	min = std::min(min, ss.min);
	valid = false;
	lastAdded = ss.lastAdded;
}

void StatSummary::add(double d) {
	numberStats++;
	sum += d;
	sumsq += d * d;
	multi *= d;
	min = std::min(min, d);
	max = std::max(max, d);
	valid = false;
	lastAdded = d;
}



double StatSummary::getLastAdded() { return lastAdded; }

void StatSummary::discountLast(double discount) { lastAdded *= discount; }


//std::string StatSummary::String() {
//	std::string s = (name == "") ? "" : name;
//	s += " min = " + std::to_string(getMin()) + "\n" +
//		" max = " + std::to_string(getMax()) + "\n" +
//		" ave = " + std::to_string(getMean()) + "\n" +
//		" sd  = " + std::to_string(getSd()) + "\n" +
//		" se  = " + std::to_string(stdErr()) + "\n" +
//		" sum  = " + std::to_string(sum) + "\n" +
//		" sumsq  = " + std::to_string(sumsq) + "\n" +
//		" n   = " + std::to_string(numberStats);
//	return s;

//}

double StatSummary::getSum() {
	return sum;
}
