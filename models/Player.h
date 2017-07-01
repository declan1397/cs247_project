#ifndef PLAYER_H
#define PLAYER_H

#include "Card.h"

class Player {
	public:
		char getType(); // Returns the type of player: 'h' - human | 'c' - computer

		std::vector<Card> getHand(); // Returns the hand vector
		std::vector<Card> getDiscards(); // Returns the discard vector

		void emptyHand(); // Used to completely empty hand / discards
		void dealCard(Card); // Insert Card into hand

		void discard(string card); // Remove from hand / add to discards
		void play(string card); // Remove from hand

		int getRoundScore(); // Returns player score
		int getTotalScore(); // Returns player score
		void setRoundScore(int score); // Sets player score
		void setTotalScore(int score); // Sets player score

		// Helper Functions
		int getCardIndex(string card); //  Returns index of card in hand

		// TODO: Check for certain card

	private:
		std::vector<Card> hand_;
		std::vector<Card> discards_;
		int roundscore_;
		int totalscore_;
		char type_;

}; // Player

#endif