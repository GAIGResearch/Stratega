#pragma once
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace SGA {
    typedef int BuffTypeID;
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
           
            std::unordered_map< BuffTypeID, double > additiveModifiers;
            std::unordered_map< BuffTypeID, double > substractModifiers;
            std::unordered_map< BuffTypeID, double > multiplicationModifiers;
            std::unordered_map< BuffTypeID, double > divideModifiers;

        public:
            /* Getters and setters for BuffType*/

            int getID() const { return id; }
            void setID(int newID) { this->id = newID; }

            const std::string& getName() const { return name; }
            void setName(std::string newName) { this->name = newName; }

            const std::unordered_map< BuffTypeID, double >& getAdditiveModifiers() const
            {
               return additiveModifiers;
            }
            void setAdditiveModifiers(const std::unordered_map< BuffTypeID, double >& m)
            {
               additiveModifiers = m;
            }

            const std::unordered_map< BuffTypeID, double >& getSubstractModifiers() const
            {
               return substractModifiers;
            }
            void setSubstractModifiers(const std::unordered_map< BuffTypeID, double >& m)
            {
               substractModifiers = m;
            }

            const std::unordered_map< BuffTypeID, double >& getMultiplicationModifiers() const
            {
               return multiplicationModifiers;
            }
            void setMultiplicationModifiers(const std::unordered_map< BuffTypeID, double >& m)
            {
               multiplicationModifiers = m;
            }

            const std::unordered_map< BuffTypeID, double >& getDivideModifiers() const
            {
               return divideModifiers;
            }
            void setDivideModifiers(const std::unordered_map< BuffTypeID, double >& m)
            {
               divideModifiers = m;
            }
    };
}  // namespace SGA
