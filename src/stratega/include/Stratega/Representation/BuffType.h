#pragma once
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace SGA {
typedef int ParameterID;
    struct BuffType
    {
        private:
            /// <summary>
            /// Unique entity type ID
            /// </summary>
            int id;

            /// <summary>
            /// Name of this type, as defined in the YAML file.
            /// </summary>
            std::string name;
           
            std::unordered_map< ParameterID, double > additiveModifiers;
            std::unordered_map< ParameterID, double > multiplicationModifiers;

        public:
            /* Getters and setters for BuffType*/
            int getID() const { return id; }
            void setID(int newID) { this->id = newID; }

            const std::string& getName() const { return name; }
            void setName(std::string newName) { this->name = newName; }

            const std::unordered_map< ParameterID, double >& getAdditiveModifiers() const
            {
               return additiveModifiers;
            }
            void setAdditiveModifiers(const std::unordered_map< ParameterID, double >& m)
            {
               additiveModifiers = m;
            }

            const std::unordered_map< ParameterID, double >& getMultiplicationModifiers() const
            {
               return multiplicationModifiers;
            }
            void setMultiplicationModifiers(const std::unordered_map< ParameterID, double >& m)
            {
               multiplicationModifiers = m;
            }

            double getParameterWithAdditiveBuffsApplied(double baseValue, ParameterID searchedID) const 
            {
               auto itA = additiveModifiers.find(searchedID);
               if(itA != additiveModifiers.end())
               {
                  baseValue += itA->second;
               }

               return baseValue;
            }
            
            double getParameterWithOutAdditiveBuffsApplied(double baseValue, ParameterID searchedID) const
            {
               auto itA = additiveModifiers.find(searchedID);
               if(itA != additiveModifiers.end())
               {
                  baseValue -= itA->second;
               }

               return baseValue;
            }

            double getParameterWithOutMultiplicationBuffsApplied(double baseValue, ParameterID searchedID) const
            {
                double newBaseValue = baseValue;
                auto itA = multiplicationModifiers.find(searchedID);
                if (itA != multiplicationModifiers.end())
                {
                    baseValue /= itA->second;
                }
                return  baseValue - newBaseValue;
            }

            double getMultiplicationSum(double baseValue, ParameterID searchedID) const
            {
                double newBaseValue = baseValue;
                auto itA = multiplicationModifiers.find(searchedID);
                if(itA != multiplicationModifiers.end())
                {
                   baseValue *= itA->second;
                }
                return  baseValue-newBaseValue;
            }
    };
}  // namespace SGA
