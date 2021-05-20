#include<iostream>
#include<vector>
#include<set>
#include<queue>
#include<string.h>
#include<unordered_set>
#include<fstream>
#include<sstream>
#include<algorithm>
#include <regex>


std::string trim(std::string s) {
	std::regex e("^\\s+|\\s+$"); 
	return std::regex_replace(s, e, "");
}

class Literal {
public:
	Literal(std::string name, bool prefix): name(name), prefix(prefix) {}
	Literal(){}
	std::string getName() {
		return this->name;
	}

	bool getPrefix() {
		return this->prefix;
	}

	bool operator== (Literal& second) {
		return name == second.getName() && prefix == second.getPrefix();
	}

	bool operator<(Literal& rhs) const
	{
		return name < rhs.getName();
	}
private:
	std::string name;
	bool prefix;

};

Literal getComplement (Literal &s) {
	return Literal(s.getName(), !s.getPrefix());
}

bool isComplement(Literal& first, Literal& second) {
	if (first.getName() == second.getName() && first.getPrefix() != second.getPrefix()) return true;
	else return false;
}

class Clause {
public:
	std::vector<Literal> literals;
	bool nil = false;
	int index;
	int first = 0;
	int second = 0;
	std::vector<Literal> getWithoutLiteral(Literal remove) {
		std::vector<Literal> returnSet;
		for (auto c : literals) {
			if (c == remove) {
				continue;
			}
			returnSet.push_back(c);
		}
		return returnSet;
	}

	bool checkTautology() {
		for (auto a : literals) {
			for (auto b : literals) {
				if (a == b) continue;
				if (isComplement(a, b)) return true;
			}
		}
		return false;
	}

	bool operator== (Clause& druga) {
		int zbroj = 0;
		if (literals.size() != druga.literals.size()) return false;
		for (auto x : literals) {
			for (auto y : druga.literals) {
				if (x == y) ++zbroj;
			}
		}
		if (zbroj == literals.size()) return true;
		return false;
	}

	Clause factor() {
		std::sort(literals.begin(), literals.end());
		literals.erase(std::unique(literals.begin(), literals.end()), literals.end());
		return *this;
	}

	void addLiteral(Literal toAdd) {
		literals.push_back(toAdd);
	}

	void setIndexes(int f, int s) {
		first = f;
		second = s;
	}

	void clear() {
		literals.clear();
	}

	Clause(){}
	Clause(bool nil, int first, int second): nil(nil), first(first), second(second) { index = indexCount++; }
	Clause(std::vector<Literal> init) {
		literals = init;
		index = indexCount++;
	}
	Clause(Literal single) {
		literals.push_back(single);
		index = indexCount++;
	}
	void appendVec(std::vector<Literal> vec) {
		literals.insert(literals.end(), vec.begin(), vec.end());
	}
private:
	static int indexCount;
};


std::ostream& operator<<(std::ostream& os, Clause& dt)
{
	if (dt.nil) os << "NIL";
	else {
		for (auto x : dt.literals) {
			os << (x.getPrefix() ? "~" : "") << x.getName() << (x == dt.literals.back() ? "" : " v ");
		}
	}
	return os;
}

bool isPodskup(const Clause& left, const Clause& right) {
	int zbroj = 0;
	if (left.literals.size() < right.literals.size()) return false;
	for (auto x : left.literals) {
		for (auto y : right.literals) {
			if (x == y) ++zbroj;
		}
	}
	if (zbroj == right.literals.size()) return true;
	return false;
}



bool operator==(const Clause& left, const Clause& right) {
	int zbroj = 0;
	if (left.literals.size() != right.literals.size()) return false;
	for (auto x : left.literals) {
		for (auto y : right.literals) {
			if (x == y) ++zbroj;
		}
	}
	if (zbroj == left.literals.size()) return true;
	return false;
}

int Clause::indexCount = 1;

int main(int argc, char** argv) {
	//std::cout << "ARGC: " << argc << std::endl;
	if (argc < 3) exit(-1);
	if (argc >= 5) exit(-1);
	std::string clausePath;
	bool cooking = false;
	std::string commandsPath;
	if (std::string(argv[1]) == "resolution") {
		if (argc == 3) {
			clausePath = std::string(argv[2]);
			std::cout << clausePath << std::endl;
		}
		else exit(-1);
	}
	else if (std::string(argv[1]) == "cooking") {
		if (argc == 4) {
			clausePath = std::string(argv[2]);
			commandsPath = std::string(argv[3]);
			cooking = true;
		}
		else exit(-1);
	}
	else exit(-1);

	std::deque<Clause> clauses;
	std::ifstream CFile(clausePath);
	std::string line;
	std::vector<Literal> tempVector;
	if (!cooking) {
		while (std::getline(CFile, line))
		{
			if (line.at(0) == '#') {
				continue;
			}

			std::transform(line.begin(), line.end(), line.begin(),
				[](unsigned char c) { return std::tolower(c); });
			std::string delimiter = " v ";
			size_t pos1 = line.find(delimiter);
			if (pos1 == std::string::npos) {
				if (line.at(0) == '~') {
					tempVector.push_back(Literal(line.substr(1, line.length() - 1), true));
				}
				else {
					tempVector.push_back(Literal(line, false));
				}
				clauses.push_back(Clause(tempVector));
				tempVector.clear();
				continue;
			} // ~Butter v ~Flour v ~Milk v ~Cream v ~Parmesan v Alfredo_sauce
			while ((pos1 = line.find(delimiter)) != std::string::npos) {
				std::string temp = line.substr(0, pos1);
				line.erase(0, pos1 + delimiter.length());
				//line = line.substr(pos1 + 3, line.length() - 1);

				if (temp.at(0) == '~') {
					tempVector.push_back(Literal(temp.substr(1, temp.length() - 1), true));
				}
				else {
					tempVector.push_back(Literal(temp, false));
				}
				size_t pos1 = line.find(" v ");
			}
			if (line.at(0) == '~') {
				tempVector.push_back(Literal(line.substr(1, line.length() - 1), true));
			}
			else {
				tempVector.push_back(Literal(line, false));
			}
			clauses.push_back(Clause(tempVector));
			tempVector.clear();
		}

		//Brisanje zadnje klauzule, negiranje i dodavanje u SoS skup
		std::deque<Clause> SOSclauses;
		Clause finalClause = clauses.back();
		std::deque<Clause> finalClauses;
		for (auto c : finalClause.literals) {
			finalClauses.push_back(Literal(c.getName(), !c.getPrefix()));
		}
		int noOfFinal = finalClauses.size();
		SOSclauses = finalClauses;
		clauses.pop_back();
		int noOfClauses = clauses.size();

		std::deque<Clause> clearedClauses = clauses;
		/*
		std::deque<Clause> clearedClauses;
		for (auto x : clauses) {
			if (x.checkTautology()) continue;
			else clearedClauses.push_back(x);
		}
		*/

		clauses.clear();
		for (auto x : clearedClauses) {
			if (!x.checkTautology()) {
				clauses.push_back(x);
			}
		}

		std::deque<Clause> tempSOS;
		std::deque<Clause> helpSOS;
		std::vector<Clause> ignoreList;
		int index = 1;
		bool isFound = false;
		bool isNil = false;
		int nil1, nil2;
		Clause newClause;
		Literal foundLiteral;
		while (true) {
			for (int sosit = 0; sosit < SOSclauses.size(); ++sosit) {
				for (int it = clauses.size() - 1; it >= 0; --it) {
					if (std::find(ignoreList.begin(), ignoreList.end(), clauses[it]) != ignoreList.end()) {
						continue;
					}
					isFound = false;
					isNil = false;
					for (auto sosAtom : SOSclauses[sosit].literals) {
						for (auto atom : clauses[it].literals) {
							if (isComplement(atom, sosAtom)) {
								isFound = true;
								foundLiteral = sosAtom;
								if (clauses[it].literals.size() == 1 && SOSclauses[sosit].literals.size() == 1) {
									isNil = true;
								}
								break;
							}
						}
						if (isFound) break;
					}

					if (isNil) {
						//helpSOS.push_back(Clause(true, nil1, nil2));
						nil1 = it + 1;
						nil2 = clauses.size() + sosit + 1;
						//tempSOS.push_back(Clause(true, nil1, nil2));
						break;
					}

					if (isFound) {
						if (clauses[it].literals.size() > 1) newClause.appendVec(clauses[it].getWithoutLiteral(getComplement(foundLiteral)));
						if (SOSclauses[sosit].literals.size() > 1) newClause.appendVec(SOSclauses[sosit].getWithoutLiteral(foundLiteral));
						if (!newClause.checkTautology()) {
							bool isRedundant = false;
							for (auto x : clauses) {
								if (isPodskup(x, newClause)) {
									if (x.literals.size() > newClause.literals.size()) {
										ignoreList.push_back(x);
									}
									else {
										newClause.clear();
										isFound = false;
										isRedundant = true;
										//continue;
									}
								}
								if (isRedundant) break;
							}
							if (isRedundant) continue;

							for (auto x : SOSclauses) {
								if (isPodskup(x, newClause)) {
									if (x.literals.size() > newClause.literals.size()) {
										ignoreList.push_back(x);
									}
									else {
										newClause.clear();
										isFound = false;
										isRedundant = true;
										//continue;
									}
								}
								if (isRedundant) break;
							}
							if (isRedundant) continue;
							newClause = newClause.factor();
							newClause.setIndexes(it + 1, sosit + clauses.size() + 1);
							helpSOS.push_back(newClause);
							tempSOS.push_back(newClause);
							newClause.clear();
						}
						else {
							newClause.clear();
							isFound = false;
						}

					}
				}
				if (isNil) break;
			}
			for (auto x : SOSclauses) {
				clauses.push_back(x);
			}
			SOSclauses.clear();
			SOSclauses = tempSOS;
			for (auto x : SOSclauses) {
				clauses.push_back(x);
			}
			if (isNil) break;
			if (tempSOS.size() == 0 && !isFound) break;
			SOSclauses.clear();
			SOSclauses = tempSOS;
			tempSOS.clear();
			newClause.clear();
		}
		for (auto x : SOSclauses) {
			clauses.push_back(x);
		}

		int counter = 1;

		if (isNil) {
			std::set<int> indexSet;
			indexSet.insert(nil1);
			indexSet.insert(nil2);
			while (true) {
				int setSize = indexSet.size();
				for (auto x : indexSet) {
					int first = clauses[x - 1].first;
					int second = clauses[x - 1].second;
					if (first > 0) indexSet.insert(first);
					if (second > 0) indexSet.insert(second);
				}
				if (indexSet.size() == setSize) break;
			}
			for (int i = 0; i < clauses.size(); ++i) {
				if (indexSet.find(i + 1) != indexSet.end()) {
					if (i < noOfClauses + noOfFinal) std::cout << counter++ << ". " << clauses[i] << std::endl;
					if (i == noOfClauses + noOfFinal - 1) std::cout << "===============" << std::endl;
					if (i >= noOfClauses + noOfFinal) {
						std::cout << counter++ << ". " << clauses[i] << " (" << std::distance(indexSet.begin(), indexSet.find(clauses[i].first)) + 1 << ", " << std::distance(indexSet.begin(), indexSet.find(clauses[i].second)) + 1 << ")" << std::endl;
					}
				}
			}
			std::cout << counter++ << ". NIL (" << std::distance(indexSet.begin(), indexSet.find(nil1)) + 1 << ", " << std::distance(indexSet.begin(), indexSet.find(nil2)) + 1 << ")" << std::endl;
			std::cout << "===============" << std::endl;
			std::cout << "[CONCLUSION]: ";
			std::cout << finalClause;
			std::cout << " is true" << std::endl;
		}
		else {
			for (auto x : clearedClauses) {
				std::cout << counter++ << ". " << x << std::endl;
			}
			std::cout << "===============" << std::endl;
			std::cout << "[CONCLUSION]: ";
			std::cout << finalClause;
			std::cout << " is unknown" << std::endl;
		}
	}
	else {
		std::ifstream ComFile(commandsPath);
		while (std::getline(CFile, line))
		{
			if (line.at(0) == '#') {
				continue;
			}

			std::transform(line.begin(), line.end(), line.begin(),
				[](unsigned char c) { return std::tolower(c); });
			std::string delimiter = " v ";
			size_t pos1 = line.find(delimiter);
			if (pos1 == std::string::npos) {
				if (line.at(0) == '~') {
					tempVector.push_back(Literal(line.substr(1, line.length() - 1), true));
				}
				else {
					tempVector.push_back(Literal(line, false));
				}
				clauses.push_back(Clause(tempVector));
				tempVector.clear();
				continue;
			} // ~Butter v ~Flour v ~Milk v ~Cream v ~Parmesan v Alfredo_sauce
			while ((pos1 = line.find(delimiter)) != std::string::npos) {
				std::string temp = line.substr(0, pos1);
				line.erase(0, pos1 + delimiter.length());
				//line = line.substr(pos1 + 3, line.length() - 1);

				if (temp.at(0) == '~') {
					tempVector.push_back(Literal(temp.substr(1, temp.length() - 1), true));
				}
				else {
					tempVector.push_back(Literal(temp, false));
				}
				size_t pos1 = line.find(" v ");
			}
			if (line.at(0) == '~') {
				tempVector.push_back(Literal(line.substr(1, line.length() - 1), true));
			}
			else {
				tempVector.push_back(Literal(line, false));
			}
			clauses.push_back(Clause(tempVector));
			tempVector.clear();
		}
		std::deque<std::pair<Clause, int>> actions; //1 -> ?, 2 -> +, 3 -> -
		int act;
		while (std::getline(ComFile, line))
		{
			if (line.at(0) == '#') {
				continue;
			}

			std::transform(line.begin(), line.end(), line.begin(),
				[](unsigned char c) { return std::tolower(c); });
			char action = line.back();
			if (action == '?') act = 1;
			else if (action == '+') act = 2;
			else if (action == '-') act = 3;
			else {
				std::cout << "Kriva korisnicka naredba!" << std::endl;
				exit(-1);
			}
			line.pop_back();
			line = trim(line);
			std::string delimiter = " v ";
			size_t pos1 = line.find(delimiter);
			if (pos1 == std::string::npos) {
				if (line.at(0) == '~') {
					tempVector.push_back(Literal(line.substr(1, line.length() - 1), true));
				}
				else {
					tempVector.push_back(Literal(line, false));
				}
				actions.push_back(std::pair<Clause, int>(Clause(tempVector), act));
				tempVector.clear();
				continue;
			} // ~Butter v ~Flour v ~Milk v ~Cream v ~Parmesan v Alfredo_sauce
			while ((pos1 = line.find(delimiter)) != std::string::npos) {
				std::string temp = line.substr(0, pos1);
				line.erase(0, pos1 + delimiter.length());
				//line = line.substr(pos1 + 3, line.length() - 1);

				if (temp.at(0) == '~') {
					tempVector.push_back(Literal(temp.substr(1, temp.length() - 1), true));
				}
				else {
					tempVector.push_back(Literal(temp, false));
				}
				size_t pos1 = line.find(" v ");
			}
			if (line.at(0) == '~') {
				tempVector.push_back(Literal(line.substr(1, line.length() - 1), true));
			}
			else {
				tempVector.push_back(Literal(line, false));
			}
			actions.push_back(std::pair<Clause, int>(Clause(tempVector), act));
			tempVector.clear();
		}

		std::deque<Clause> SOSclauses;
		std::deque<Clause> clearedClauses = clauses;
		clauses.clear();
		for (auto x : clearedClauses) {
			if (!x.checkTautology()) {
				clauses.push_back(x);
			}
		}
		std::cout << "Constructed with knowledge:" << std::endl;
		for (auto x : clauses) {
			std::cout << x << std::endl;
		}
		std::cout << std::endl;
		for (auto command : actions) {
			std::cout << "User's command: ";

			if (command.second == 1) {
				std::cout << command.first << " ?" << std::endl;
				int noOfFinal = command.first.literals.size();
				int noOfClauses = clauses.size();
				for (auto c : command.first.literals) {
					SOSclauses.push_back(Clause(Literal(trim(c.getName()), !c.getPrefix())));
				}
				std::deque<Clause> tempSOS;
				std::deque<Clause> helpSOS;
				std::vector<Clause> ignoreList;
				int index = 1;
				bool isFound = false;
				bool isNil = false;
				int nil1, nil2;
				Clause newClause;
				Literal foundLiteral;
				while (true) {
					for (int sosit = 0; sosit < SOSclauses.size(); ++sosit) {
						for (int it = clauses.size() - 1; it >= 0; --it) {
							if (std::find(ignoreList.begin(), ignoreList.end(), clauses[it]) != ignoreList.end()) {
								continue;
							}
							isFound = false;
							isNil = false;
							for (auto sosAtom : SOSclauses[sosit].literals) {
								for (auto atom : clauses[it].literals) {
									if (isComplement(atom, sosAtom)) {
										isFound = true;
										foundLiteral = sosAtom;
										if (clauses[it].literals.size() == 1 && SOSclauses[sosit].literals.size() == 1) {
											isNil = true;
										}
										break;
									}
								}
								if (isFound) break;
							}

							if (isNil) {
								//helpSOS.push_back(Clause(true, nil1, nil2));
								nil1 = it + 1;
								nil2 = clauses.size() + sosit + 1;
								//tempSOS.push_back(Clause(true, nil1, nil2));
								break;
							}

							if (isFound) {
								if (clauses[it].literals.size() > 1) newClause.appendVec(clauses[it].getWithoutLiteral(getComplement(foundLiteral)));
								if (SOSclauses[sosit].literals.size() > 1) newClause.appendVec(SOSclauses[sosit].getWithoutLiteral(foundLiteral));
								if (!newClause.checkTautology()) {
									bool isRedundant = true;
									for (auto x : clauses) {
										if (isPodskup(x, newClause)) {
											if (x.literals.size() > newClause.literals.size()) {
												ignoreList.push_back(x);
												isRedundant = false;
											}
											else {
												newClause.clear();
												isFound = false;
												//isRedundant = true;
												continue;
											}
										}
									}
									if (isRedundant) {

										for (auto x : SOSclauses) {
											if (isPodskup(x, newClause)) {
												if (x.literals.size() > newClause.literals.size()) {
													ignoreList.push_back(x);
													isRedundant = false;
												}
												else {
													newClause.clear();
													isFound = false;
													//isRedundant = true;
													continue;
												}
											}
										}
									}
									if (isRedundant) continue;
									newClause = newClause.factor();
									newClause.setIndexes(it + 1, sosit + clauses.size() + 1);
									helpSOS.push_back(newClause);
									tempSOS.push_back(newClause);
									newClause.clear();
								}
								else {
									newClause.clear();
									isFound = false;
								}

							}
						}
						if (isNil) break;
					}
					for (auto x : SOSclauses) {
						clauses.push_back(x);
					}
					SOSclauses.clear();
					SOSclauses = tempSOS;
					for (auto x : SOSclauses) {
						clauses.push_back(x);
					}
					if (isNil) break;
					if (tempSOS.size() == 0 && !isFound) break;
					SOSclauses.clear();
					SOSclauses = tempSOS;
					tempSOS.clear();
					newClause.clear();
				}
				for (auto x : SOSclauses) {
					clauses.push_back(x);
				}
				/*
				for (auto x : clauses) {
					std::cout << x << std::endl;
				}
				*/

				int counter = 1;

				if (isNil) {
					std::set<int> indexSet;
					indexSet.insert(nil1);
					indexSet.insert(nil2);
					while (true) {
						int setSize = indexSet.size();
						for (auto x : indexSet) {
							int first = clauses[x - 1].first;
							int second = clauses[x - 1].second;
							if (first > 0) indexSet.insert(first);
							if (second > 0) indexSet.insert(second);
						}
						if (indexSet.size() == setSize) break;
					}
					for (int i = 0; i < clauses.size(); ++i) {
						if (indexSet.find(i + 1) != indexSet.end()) {
							if (i < noOfClauses + noOfFinal) std::cout << counter++ << ". " << clauses[i] << std::endl;
							if (i == noOfClauses + noOfFinal - 1) std::cout << "===============" << std::endl;
							if (i >= noOfClauses + noOfFinal) {
								std::cout << counter++ << ". " << clauses[i] << " (" << std::distance(indexSet.begin(), indexSet.find(clauses[i].first)) + 1 << ", " << std::distance(indexSet.begin(), indexSet.find(clauses[i].second)) + 1 << ")" << std::endl;
							}
						}
					}
					std::cout << counter++ << ". NIL (" << std::distance(indexSet.begin(), indexSet.find(nil1)) + 1 << ", " << std::distance(indexSet.begin(), indexSet.find(nil2)) + 1 << ")" << std::endl;
					std::cout << "===============" << std::endl;
					std::cout << "[CONCLUSION]: ";
					std::cout << command.first;
					std::cout << " is true" << std::endl;
				}
				else {
					for (auto x : clearedClauses) {
						std::cout << counter++ << ". " << x << std::endl;
					}
					std::cout << "===============" << std::endl;
					std::cout << "[CONCLUSION]: ";
					std::cout << command.first;
					std::cout << " is unknown" << std::endl;
				}
			}
			else if (command.second == 2) {
				std::cout << command.first << " +" << std::endl;
				bool add = true;
				for (auto x : clauses) {
					if (x == command.first) {
						add = false;
						break;
					}
				}
				if (add) {
					clauses.push_back(command.first);
					
				}
				std::cout << "Added " << command.first << std::endl;
			}
			else if (command.second == 3) {
				std::cout << command.first << " -" << std::endl;
				bool removed = false;
				std::deque<Clause>::iterator toDeleteIt;
				for (std::deque<Clause>::iterator it = clauses.begin(); it != clauses.end(); ++it) {
					if (*it == command.first) {
						toDeleteIt = it;
						removed = true;
						break;
					}
				}
				if (removed) {
					clauses.erase(toDeleteIt);
					
				}
				std::cout << "Removed " << command.first << std::endl;
			}
		}
	}
}
