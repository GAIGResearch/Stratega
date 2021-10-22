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
            std::unordered_map< ParameterID, double > substractModifiers;
            std::unordered_map< ParameterID, double > multiplicationModifiers;
            std::unordered_map< ParameterID, double > divideModifiers;

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

            const std::unordered_map< ParameterID, double >& getSubstractModifiers() const
            {
               return substractModifiers;
            }
            void setSubstractModifiers(const std::unordered_map< ParameterID, double >& m)
            {
               substractModifiers = m;
            }

            const std::unordered_map< ParameterID, double >& getMultiplicationModifiers() const
            {
               return multiplicationModifiers;
            }
            void setMultiplicationModifiers(const std::unordered_map< ParameterID, double >& m)
            {
               multiplicationModifiers = m;
            }

            const std::unordered_map< ParameterID, double >& getDivideModifiers() const
            {
               return divideModifiers;
            }
            void setDivideModifiers(const std::unordered_map< ParameterID, double >& m)
            {
               divideModifiers = m;
            }

            double getParameterWithAdditiveBuffsApplied(double baseValue, ParameterID id) const 
            {
               auto itA = additiveModifiers.find(id);
               if(itA != additiveModifiers.end())
               {
                  baseValue += itA->second;
               }

               return baseValue;
            }
            
            double getParameterWithOutAdditiveBuffsApplied(double baseValue, ParameterID id) const 
            {
               auto itA = additiveModifiers.find(id);
               if(itA != additiveModifiers.end())
               {
                  baseValue -= itA->second;
               }

               return baseValue;
            }

            double getParameterWithOutMultiplicationBuffsApplied(double baseValue, ParameterID id) const
            {
               auto itA = multiplicationModifiers.find(id);
               if(itA != multiplicationModifiers.end())
               {
                  baseValue /= itA->second;
               }
               return baseValue;
            }

            double getParameterWithMultiplicationBuffsApplied(double baseValue, ParameterID id) const
            {
               auto itA = multiplicationModifiers.find(id);
               if(itA != multiplicationModifiers.end())
               {
                  baseValue *= itA->second;
               }
               return baseValue;
            }
    };
}  // namespace SGA
