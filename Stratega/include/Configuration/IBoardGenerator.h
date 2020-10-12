#pragma once
#include <Representation/Board.h>
#include <random>

class IBoardGenerator
{
public:
	virtual ~IBoardGenerator() = default;
	virtual SGA::Board generate(std::mt19937& rngEngine) const = 0;
protected:
	IBoardGenerator& operator=(const IBoardGenerator&) = default;
};