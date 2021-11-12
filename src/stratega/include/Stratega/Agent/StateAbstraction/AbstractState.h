#pragma once
#include <Stratega/Representation/GameState.h>
#include <vector>
#include <string>


namespace SGA
{

	class AbstractState
	{
	private:
		std::unordered_map<std::string, std::vector<double>> stateDescription;

	public:
		AbstractState()
		{

		}

		void addAttribute(std::string parameter, std::vector<double>& values)
		{
			stateDescription[parameter] = std::move(values);
		}

		/// <summary>
		/// Comparator to be used for sorting a list of abstract states.
		/// A state is considered to be smaller in case the length of its state description is shorter
		/// or in case they are equal a lexicographic sort is applied.
		/// </summary>
		/// <param name="state1"></param>
		/// <param name="state2"></param>
		/// <returns>true in case state 1 is smaller than state 2</returns>
		bool compareAbstractStates(const AbstractState& state1, const AbstractState& state2) {
			if (state1.stateDescription.size() < state2.stateDescription.size())
				return true;
			if (state1.stateDescription.size() > state2.stateDescription.size())
				return false;

			/*
			 *for (int x = 0; x < state1.stateDescription.size(); x++) {
				if (state1.stateDescription[x] == state2.stateDescription[x])
					continue;
				return state1.stateDescription[x] < state2.stateDescription[x];
			}*/

			return true;
		}

		bool operator==(const AbstractState& dt)
		{
			if (stateDescription.size() == dt.stateDescription.size())
			{
				for (auto entry : stateDescription) {
					std::string test = entry.first;

					const auto got = dt.stateDescription.find(entry.first);

					if (got == dt.stateDescription.end())
					{
					//if (dt.stateDescription.contains(entry.first)) {	// check if the same keys are contained
						if (entry.second.size() != dt.stateDescription.at(entry.first).size()) {	// check if the length per key match
							return false;
						}
					}
					else
					{
						return false;
					}
				}

				for (const auto& entry : stateDescription) {
					auto compareTo = dt.stateDescription.at(entry.first);
					for (size_t i = 0; i < entry.second.size(); i++) {
						if (entry.second[i] != compareTo[i])
							return false;
					}
				}
				return true;
			}
			return false;
		}


		friend std::ostream& operator<<(std::ostream& os, const AbstractState& dt)
		{
			for (auto entry : dt.stateDescription)
			{
				os << entry.first << ": ";
				for (auto val : entry.second)
				{
					os << val << "; ";
				}
				os << std::endl;
			}

			return os;
		}

	};
}