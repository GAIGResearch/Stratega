#pragma warning(disable: 5045)
#pragma warning(disable: 4244)
#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>

namespace SGA
{
	
	/// <summary>
	/// Is an object that interacts with the game and responds to player input or other entities.
	/// This entity can be assigned to a player performing a set of actions per tick/turn.
	/// It has a list of parameters that store real values.
	/// If the player wants to check the <see cref="SGA::EntityType"/> it can search it using the assigned typeID.
	/// </summary>
	class Timer
	{
	private:

		/// <summary>
		/// Flag that indicates if this entity will be removed from the game.
		/// </summary>
		std::chrono::time_point<std::chrono::system_clock> startTime;
		std::chrono::time_point<std::chrono::system_clock> endTime;
		bool running = false;

		long maxTimeMs;

	public:

		/// <summary>
		/// Creates an instance of a Timer. 
		/// <summary>
		/// <param name="maxTimeMs">Max time used to check the remaining time.</param>
		Timer(long newMaxTimeMs=0) :
			maxTimeMs(newMaxTimeMs)
		{
			start();
		}

		/// <summary>
		/// Starts the timer.
		/// <summary>
		void start()
		{
			startTime = std::chrono::system_clock::now();
			running = true;
		}

		/// <summary>
		/// Stops the timer.
		/// <summary>
		void stop()
		{
			endTime = std::chrono::system_clock::now();
			running = false;
		}

		/// <summary>
		// Returns the elapsed time in milliseconds.
		/// <summary>		
		long elapsedMilliseconds() const
		{
			std::chrono::time_point<std::chrono::system_clock> tempEndTime;

			if (running)
			{
				tempEndTime = std::chrono::system_clock::now();
			}
			else
			{
				tempEndTime = endTime;
			}

			return /*static_cast<long>*/(std::chrono::duration_cast<std::chrono::milliseconds>(tempEndTime - startTime).count());
		}

		/// <summary>
		// Returns the elapsed time in seconds.
		/// <summary>
		double elapsedSeconds() const
		{
			return static_cast<double>(elapsedMilliseconds()/ static_cast<long>(1000.0));
		}
		
		/// <summary>
		// Returns the elapsed time in minutes.
		/// <summary>
		double elapsedMinutes() const
		{
			return elapsedSeconds()/60.0;
		}
		
		/// <summary>
		// Returns the elapsed time in hours.
		/// <summary>
		double elapsedHours() const
		{
			return elapsedMinutes()/60.0;
		}

		/// <summary>
		// Set the max time
		/// <summary>
		/// <param name="time">Max timer in milliseconds.</param>
		void setMaxTimeMilliseconds(long newMaxTimeMs)
		{
			maxTimeMs = newMaxTimeMs;
		}

		/// <summary>
		// Checks the remaining time.
		/// <summary>
		double remainingTimeMilliseconds() const
		{
			long diff = maxTimeMs - elapsedMilliseconds();
			if (diff < 0)
				return 0;
			else
				return static_cast<double>(diff);
		}

		/// <summary>
		// Checks if the elapsed time exceeded max time.
		/// <summary>
		bool exceededMaxTime() const
		{
			if (elapsedMilliseconds() > maxTimeMs) {
				return true;
			}
			return false;
		}

		/// <summary>
		// Checks if the elapsed time exceeded max time.
		/// <summary>
		long getMaxTimeMilliseconds() const
		{
			return maxTimeMs;
		}

		/// <summary>
		/// Returns the proportion of time completed of this timer.
		/// </summary>
		/// <returns>A proportion of the time completed. 0.0 means timer just started, while 1.0 means time is exactly complete. </returns>
		double percCompletedTime() const
		{
			return static_cast<double>(elapsedMilliseconds() / maxTimeMs);
		}
	};
}