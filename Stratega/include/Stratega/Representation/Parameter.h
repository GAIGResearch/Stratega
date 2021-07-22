#pragma once
#include <string>

namespace SGA
{
	typedef int ParameterID;
	
	struct Parameter
	{

	private:

		ParameterID id = -1;
		std::string name;
		int index = -1;

		double defaultValue = 0;
		double minValue = 0;
		double maxValue = 0;
	
	public:

		double getMinValue() const { return minValue; }
		double getMaxValue() const { return maxValue; }
		double getDefaultValue() const { return defaultValue; }
		int getIndex() const { return index; }
		const std::string& getName() const { return name; }
		ParameterID getID() const { return id; }

		void setID(int newID) { this->id = newID; }
		void setName(const std::string& newName) { this->name = newName; }
		void setIndex(int idx) { index = idx; }
		void setDefaultValue(double val) { defaultValue = val; }
		void setMinValue(double val) { minValue = val; }
		void setMaxValue(double val) { maxValue = val; }


	};
}