#pragma once
class PlayerSelector
{
public:
	int playerID;

	void OnPlayerChange(int newPlayerID)
	{
		playerID = newPlayerID;
	}
};