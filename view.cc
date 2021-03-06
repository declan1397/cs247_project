<<<<<<< HEAD

#include "view.h"
#include "controller.h"
#include "model.h"
#include "subject.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <gtkmm.h> 

using namespace std;

View::View(Controller* c, Model* m) {
	controller_ = c;
	model_ = m;
	model_->subscribe(this);
	roundStart();
}

View::~View() {
	delete model_;
	delete controller_;
}

void View::update() {

	if (gamePhase_ == 1) { // Start Round
		roundStart();
	}

	else if (gamePhase_ == 2) { // Player Turn
		if (model_->getGameState() != 0){ // Round Finished
			roundEnd();
		}
		else { // Round not finished
			playerTurn();
		}
	}
}

void View::roundStart() {
	gamePhase_ = 2;
	cout << "A new round begins. It's player " << model_->getCurrentPlayerIndex() + 1 << "'s turn to play.\n";
	playerTurn();
}

void View::playerTurn() {

	std::vector<vector<Card> > cardTable = model_->getCardTable();
	std::vector<vector<int> > intTable = model_->getIntTable();

	if (model_->getCurrentPlayer()->getType() != 'c'){
		cout << "Cards on the table:\n";
		cout << "Clubs:";
		printClubs(cardTable, intTable);
		cout << "\nDiamonds:";
		printDiamonds(cardTable, intTable);
		cout << "\nHearts:";
		printHearts(cardTable, intTable);
		cout << "\nSpades:";
		printSpades(cardTable, intTable);
		cout << "\nYour hand:";
		printPlayerHand();
		cout << "\nLegal plays:";
		printLegalPlays();
	}
	controller_->executeCommand(receiveCommand());
}

void View::roundEnd() {
	std::vector<Player*> players = model_->getPlayers();
	int minScore = players[0]->getTotalScore();

	for (int i=0; i < players.size(); i++){
		cout << "Player " << i + 1 << "\'s discards:";
		printPlayerDiscards(players[i]);
		cout << endl;
		cout << "Player " << i + 1 << "\'s score: " << (players[i]->getTotalScore() - players[i]->getRoundScore()) << " + " << (players[i]->getRoundScore()) - (players[i]->getTotalScore() - players[i]->getRoundScore())  << " = " << players[i]->getTotalScore() - (players[i]->getTotalScore() - players[i]->getRoundScore()) << "\n";
		if (players[i]->getTotalScore() < minScore){
			minScore = players[i]->getTotalScore();
		}
	}

	if (model_->getGameState() == 2){
		for (int i=0; i < players.size(); i++){
			if (players[i]->getTotalScore() == minScore) {
				cout << "Player " << i + 1 << " wins!\n";
			}
		}
		exit(0);
	}

	else {
		gamePhase_ = 1;
		controller_->newRound();
	}

}

Command View::receiveCommand() {

	Command my_command;
	std::vector<Card> legalPlays = model_->getLegalPlays();
	std::vector<Card> playerHand = model_->getCurrentPlayer()->getHand();

	// First check if computer or human player

	if (model_->getCurrentPlayer()->getType() == 'c'){ // Computer Player

		if (model_->getLegalPlays().size() > 0){
			cout << "Player " << model_->getCurrentPlayerIndex() + 1 << " plays " << legalPlays[0] << ".\n";
			my_command = Command("play", legalPlays[0]);
		}
		else {
			cout << "Player " << model_->getCurrentPlayerIndex() + 1 << " discards " << playerHand[0] << ".\n";
			my_command = Command("discard", playerHand[0]);
		}
	}

	else { // Human Player

		bool validCommand = false;

		while (!validCommand){
			cout << ">";
			cin >> my_command;

			switch(my_command.type){
				case Command::Type::PLAY:
					if (std::find(legalPlays.begin(), legalPlays.end(), my_command.card) != legalPlays.end()){
						cout << "Player " << model_->getCurrentPlayerIndex() + 1 << " plays " << my_command.card << ".\n";
						validCommand = true;
					}
					else {
						cout << "This is not a legal play.\n";
					}
					break;
				case Command::Type::DISCARD:
					if (legalPlays.size() == 0) {
						cout << "Player " << model_->getCurrentPlayerIndex() + 1 << " discards " << my_command.card << ".\n";
						validCommand = true;
					}
					else {
						cout << "You have a legal play. You may not discard.\n";
					}
					break;
				case Command::Type::DECK:
					printDeck();
					break;
				case Command::Type::QUIT:
					exit(0);
				case Command::Type::RAGEQUIT:
					cout << "Player " << model_->getCurrentPlayerIndex() + 1 << " ragequits. A computer will now take over.\n";
					validCommand = true;
					break;
			}
		}
	}

	return my_command;
}

const void View::printDeck(){
	std::vector<Card> deck =  model_->getDeck();
	int cardsPerLine = 13;

	for (int i=0; i<deck.size(); i++){
		cout << deck[i] << " ";
		if (i%(cardsPerLine) == (cardsPerLine-1)){
			cout << "\n";
		}
	}

}

// Helper Functions

// Print Legal Plays
const void View::printLegalPlays() {
	std::vector<Card> legalPlays = model_->getLegalPlays();
	for (int i = 0; i < legalPlays.size(); i++) {
		cout << " " << legalPlays[i];
	}
	cout << "\n";
}

// Print Player Hand
const void View::printPlayerHand() {
	Player *p = model_->getCurrentPlayer();
	for (int i = 0; i < p->getHand().size(); i++) {
		cout << " " << p->getHand()[i];
	}
}

// Print Player Discards
const void View::printPlayerDiscards(Player *p) {
	for (int i = 0; i < p->getDiscards().size(); i++) {
		cout << " " << p->getDiscards()[i];
	}
}

// Print Clubs
const void View::printClubs(std::vector<vector<Card> > cardTable, std::vector<vector<int> > intTable) {
	for (int i = 1; i < 14; i++) {
		if (intTable[0][i] == 1) {
			cout << " " << rankToLetter(cardTable[0][i].rank().rank() + 1);
		}
	}
}

// Print Diamonds
const void View::printDiamonds(std::vector<vector<Card> > cardTable, std::vector<vector<int> > intTable) {
	for (int i = 1; i < 14; i++) {
		if (intTable[1][i] == 1) {
			cout << " " << rankToLetter(cardTable[1][i].rank().rank() + 1);
		}
	}
}

// Print Hearts
const void View::printHearts(std::vector<vector<Card> > cardTable, std::vector<vector<int> > intTable) {
	for (int i = 1; i < 14; i++) {
		if (intTable[2][i] == 1) {
			cout << " " << rankToLetter(cardTable[2][i].rank().rank() + 1);
		}
	}
}

// Print Spades
const void View::printSpades(std::vector<vector<Card> > cardTable, std::vector<vector<int> > intTable) {
	for (int i = 1; i < 14; i++) {
		if (intTable[3][i] == 1) {
			cout << " " << rankToLetter(cardTable[3][i].rank().rank() + 1);
		}
	}
}

// Return character rank
string rankToLetter(int rank) {
	switch (rank) {
	case 1: return "A";
	case 10: return "T";
	case 11: return "J";
	case 12: return "Q";
	case 13: return "K";
	default: return std::to_string(rank);
	}
}
=======
//
// #include "view.h"
// #include "controller.h"
// #include "model.h"
// #include "subject.h"
// #include <string>
// #include <iostream>
// #include <algorithm>
// #include <gtkmm.h>
//
// using namespace std;
//
// View::View(Controller* c, Model* m) {
// 	controller_ = c;
// 	model_ = m;
// 	model_->subscribe(this);
// 	roundStart();
// }
//
// View::~View() {
// 	delete model_;
// 	delete controller_;
// }
//
// void View::update() {
//
// 	if (gamePhase_ == 1) { // Start Round
// 		roundStart();
// 	}
//
// 	else if (gamePhase_ == 2) { // Player Turn
// 		if (model_->getGameState() != 0){ // Round Finished
// 			roundEnd();
// 		}
// 		else { // Round not finished
// 			playerTurn();
// 		}
// 	}
// }
//
// void View::roundStart() {
// 	gamePhase_ = 2;
// 	cout << "A new round begins. It's player " << model_->getCurrentPlayerIndex() + 1 << "'s turn to play.\n";
// 	playerTurn();
// }
//
// void View::playerTurn() {
//
// 	std::vector<vector<Card> > cardTable = model_->getCardTable();
// 	std::vector<vector<int> > intTable = model_->getIntTable();
//
// 	if (model_->getCurrentPlayer()->getType() != 'c'){
// 		cout << "Cards on the table:\n";
// 		cout << "Clubs:";
// 		printClubs(cardTable, intTable);
// 		cout << "\nDiamonds:";
// 		printDiamonds(cardTable, intTable);
// 		cout << "\nHearts:";
// 		printHearts(cardTable, intTable);
// 		cout << "\nSpades:";
// 		printSpades(cardTable, intTable);
// 		cout << "\nYour hand:";
// 		printPlayerHand();
// 		cout << "\nLegal plays:";
// 		printLegalPlays();
// 	}
// 	// if (getCurrentPlayer()->getType() == 'c') {
// 	// 	Command my_command = getCurrentPlayer()->makeMove(legalPlays);
// 	// 	controller_->executeCommand(my_command);
// 	// }
// 	controller_->executeCommand(receiveCommand());
// }
//
// void View::roundEnd() {
// 	// int minScore = players[0]->getTotalScore();
// 	//
// 	// for (int i=0; i < players.size(); i++){
// 	// 	cout << "Player " << i + 1 << "\'s discards:";
// 	// 	printPlayerDiscards(players[i]);
// 	// 	cout << endl;
// 	// 	cout << "Player " << i + 1 << "\'s score: " << (players[i]->getTotalScore() - players[i]->getRoundScore()) << " + " << (players[i]->getRoundScore()) - (players[i]->getTotalScore() - players[i]->getRoundScore())  << " = " << players[i]->getTotalScore() - (players[i]->getTotalScore() - players[i]->getRoundScore()) << "\n";
// 	// 	if (players[i]->getTotalScore() < minScore){
// 	// 		minScore = players[i]->getTotalScore();
// 	// 	}
// 	// }
// 	//
// 	// if (model_->getGameState() == 2){
// 	// 	for (int i=0; i < players.size(); i++){
// 	// 		if (players[i]->getTotalScore() == minScore) {
// 	// 			cout << "Player " << i + 1 << " wins!\n";
// 	// 		}
// 	// 	}
// 	// 	exit(0);
// 	// }
// 	//
// 	// else {
// 	// 	gamePhase_ = 1;
// 	// 	controller_->newRound();
// 	// }
// 	return;
// }
//
// Command View::receiveCommand() {
//
// 	Command my_command;
// 	std::vector<Card> legalPlays = model_->getLegalPlays();
// 	std::vector<Card> playerHand = model_->getCurrentPlayer()->getHand();
//
// 	// First check if computer or human player
// 	if (model_->getCurrentPlayer()->getType() == 'c') {
// 		my_command = model_->getCurrentPlayer()->makeMove(legalPlays);
// 	}
// 	else { // Human Player
//
// 		bool validCommand = false;
//
// 		while (!validCommand){
// 			cout << ">";
// 			cin >> my_command;
//
// 			switch(my_command.type){
// 				case Command::Type::PLAY:
// 					if (std::find(legalPlays.begin(), legalPlays.end(), my_command.card) != legalPlays.end()){
// 						cout << "Player " << model_->getCurrentPlayerIndex() + 1 << " plays " << my_command.card << ".\n";
// 						validCommand = true;
// 					}
// 					else {
// 						cout << "This is not a legal play.\n";
// 					}
// 					break;
// 				case Command::Type::DISCARD:
// 					if (legalPlays.size() == 0) {
// 						cout << "Player " << model_->getCurrentPlayerIndex() + 1 << " discards " << my_command.card << ".\n";
// 						validCommand = true;
// 					}
// 					else {
// 						cout << "You have a legal play. You may not discard.\n";
// 					}
// 					break;
// 				case Command::Type::DECK:
// 					printDeck();
// 					break;
// 				case Command::Type::QUIT:
// 					exit(0);
// 				case Command::Type::RAGEQUIT:
// 					cout << "Player " << model_->getCurrentPlayerIndex() + 1 << " ragequits. A computer will now take over.\n";
// 					validCommand = true;
// 					break;
// 			}
// 		}
// 	}
// 	return my_command;
// }
//
// const void View::printDeck(){
// 	std::vector<Card> deck =  model_->getDeck();
// 	int cardsPerLine = 13;
//
// 	for (int i=0; i<deck.size(); i++){
// 		cout << deck[i] << " ";
// 		if (i%(cardsPerLine) == (cardsPerLine-1)){
// 			cout << "\n";
// 		}
// 	}
//
// }
//
// // Helper Functions
//
// // Print Legal Plays
// const void View::printLegalPlays() {
// 	std::vector<Card> legalPlays = model_->getLegalPlays();
// 	for (int i = 0; i < legalPlays.size(); i++) {
// 		cout << " " << legalPlays[i];
// 	}
// 	cout << "\n";
// }
//
// // Print Player Hand
// const void View::printPlayerHand() {
// 	Player *p = model_->getCurrentPlayer();
// 	for (int i = 0; i < p->getHand().size(); i++) {
// 		cout << " " << p->getHand()[i];
// 	}
// }
//
// // Print Player Discards
// const void View::printPlayerDiscards(Player *p) {
// 	for (int i = 0; i < p->getDiscards().size(); i++) {
// 		cout << " " << p->getDiscards()[i];
// 	}
// }
//
// // Print Clubs
// const void View::printClubs(std::vector<vector<Card> > cardTable, std::vector<vector<int> > intTable) {
// 	for (int i = 1; i < 14; i++) {
// 		if (intTable[0][i] == 1) {
// 			cout << " " << rankToLetter(cardTable[0][i].rank().rank() + 1);
// 		}
// 	}
// }
//
// // Print Diamonds
// const void View::printDiamonds(std::vector<vector<Card> > cardTable, std::vector<vector<int> > intTable) {
// 	for (int i = 1; i < 14; i++) {
// 		if (intTable[1][i] == 1) {
// 			cout << " " << rankToLetter(cardTable[1][i].rank().rank() + 1);
// 		}
// 	}
// }
//
// // Print Hearts
// const void View::printHearts(std::vector<vector<Card> > cardTable, std::vector<vector<int> > intTable) {
// 	for (int i = 1; i < 14; i++) {
// 		if (intTable[2][i] == 1) {
// 			cout << " " << rankToLetter(cardTable[2][i].rank().rank() + 1);
// 		}
// 	}
// }
//
// // Print Spades
// const void View::printSpades(std::vector<vector<Card> > cardTable, std::vector<vector<int> > intTable) {
// 	for (int i = 1; i < 14; i++) {
// 		if (intTable[3][i] == 1) {
// 			cout << " " << rankToLetter(cardTable[3][i].rank().rank() + 1);
// 		}
// 	}
// }
//
// // Return character rank
// string rankToLetter(int rank) {
// 	switch (rank) {
// 	case 1: return "A";
// 	case 10: return "T";
// 	case 11: return "J";
// 	case 12: return "Q";
// 	case 13: return "K";
// 	default: return std::to_string(rank);
// 	}
// }
>>>>>>> 6f0f1d5402978ba7d10f1e1fec23f938cc281e86
