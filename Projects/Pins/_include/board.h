#pragma once

#include <vector>
#include <list>

#include <unordered_map>

namespace pin {
	enum PinType {
		Red = 0,
		Green,
		Blue,
		White = 100,
		Invalid = 1000
	};

	// PIN
	class Pin {
	public: 
		bool operator==(const Pin& other) const {
			return (type_ == other.type_ && id_ == other.id_);
		}

		Pin& operator=(const Pin& other) {
			if (this != &other) {
				this->type_ = other.type_;
				this->id_ = other.id_;
			}
			return *this;
		}

	protected:
		Pin(PinType type, unsigned int id) : type_(type), id_(id) {}
		PinType type_{};
		unsigned int id_{};

		friend class Rope;
		friend class Board;
	};

	// ROPE
	class Rope {
	public:
		bool operator==(const Rope& other) const {
			return (pinA_ == other.pinA_ && pinB_ == other.pinB_);
		}

		Rope& operator=(const Rope& other) {
			if (this != &other) {
				this->pinA_ = other.pinA_;
				this->pinB_ = other.pinB_;
			}
			return *this;
		}

	private:
		friend class Board;

		Rope(Pin& a, Pin& b) : pinA_(a), pinB_(b) {}
		Pin& pinA_;
		Pin& pinB_;
	};

	// BOARD
	class Board {
	
	public:
		Board() {};

		const Pin& addPin(PinType type);

		PinType checkRopeType(const Pin& a, const Pin& b);
		std::list<Rope>::const_iterator addRope(Pin& a, Pin& b);
		void removeRope(std::list<Rope>::const_iterator ropeIter);

		Board(const Board&) = delete;
		Board& operator=(const Board&) = delete;
		Board(Board&&) = delete;
		Board& operator=(Board&&) = delete;

	private:
		void _whiteConnectPin(const Pin& w_pin, const Pin& o_pin);
		void _whiteDisconnectPin(const Pin& w_pin, const Pin& o_pin);
		bool _checkIfSolved();
		void _printPins();

		std::vector<std::vector<unsigned int>> pins_ = std::vector<std::vector<unsigned int>>(3);
		std::vector<std::vector<Pin>> whites_;

		std::list<Rope> ropes_;
	};
}