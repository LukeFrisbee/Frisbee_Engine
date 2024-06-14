#include <cassert>

#include "board.h"
#include <iostream>

namespace pin {

	const Pin& Board::addPin(PinType type)
	{
		assert(type != Invalid && "Cannot add pin of Invalid!");

		if (type < 100) {
			Pin pin = Pin(type, pins_[type].size());
			pins_[type].push_back(0);
			return pin;
		}
		else if (type == White) {
			Pin pin = Pin(type, whites_.size());
			whites_.push_back(std::vector<Pin>());
			return pin;
		}

		Pin pin = Pin(Invalid, 0);
		return pin;
	}

	PinType Board::checkRopeType(const Pin& a, const Pin& b)
	{
		if (a.type_ == Invalid || b.type_ == Invalid) {
			return Invalid;
		}
		else if (a.type_ == White && b.type_ == White) {
			return Invalid;

		}
		else if (a.type_ == White) {
			if (whites_[a.id_].size() > 0 && whites_[a.id_][0].type_ != b.type_) {
				return Invalid;
			}
			return White;
		}
		else if (b.type_ == White) {
			if (whites_[b.id_].size() > 0 && whites_[b.id_][0].type_ != a.type_) {
				return Invalid;
			}
			return White;
		}
		else if (a.type_ == White || b.type_ == White) {
			return White;
		}
		else if (a.type_ == Red && b.type_ == Red) {
			return Red;
		}
		else if (a.type_ == Green && b.type_ == Green) {
			return Green;
		}
		else if (a.type_ == Blue && b.type_ == Blue) {
			return Blue;
		}

		return Invalid;
	}

	std::list<Rope>::const_iterator Board::addRope(Pin& a, Pin& b)
	{
		auto ropeType = checkRopeType(a, b);
		assert(ropeType != Invalid && "Cannot create a rope with an invalid type");

		if (ropeType < 100) {
			pins_[static_cast<int>(a.type_)][a.id_] += 1;
			pins_[static_cast<int>(b.type_)][b.id_] += 1;
		}
		else if (ropeType == White) {
			if (a.type_ == White)
				_whiteConnectPin(a, b);
			else
				_whiteConnectPin(b, a);
		}


		Rope rope = Rope(a, b);
		ropes_.push_back(rope);

		//_printPins();
		_checkIfSolved();

		return std::prev(ropes_.end());
	}

	bool Board::_checkIfSolved() {
		for (auto& type : pins_) {
			for (auto& count : type) {
				if (count <= 0)
					return false;
			}
		}

		//std::cout << "Solved!\n";
		return true;
	}

	void Board::removeRope(std::list<Rope>::const_iterator ropeIter)
	{
		Rope rope = *ropeIter;
		auto ropeType = checkRopeType(rope.pinA_, rope.pinB_);

		if (ropeType < 100) {
			pins_[static_cast<int>(rope.pinA_.type_)][rope.pinA_.id_] -= 1;
			pins_[static_cast<int>(rope.pinB_.type_)][rope.pinB_.id_] -= 1;
		}
		else if (ropeType == White) {
			if (rope.pinA_.type_ == White)
				_whiteDisconnectPin(rope.pinA_, rope.pinB_);
			else
				_whiteDisconnectPin(rope.pinB_, rope.pinA_);
		}

		if (ropeIter != ropes_.end()) {
			ropes_.erase(ropeIter);
		}

		_printPins();
	}

	void Board::_whiteConnectPin(const Pin& w_pin, const Pin& o_pin)
	{
		if (w_pin.type_ != White)
			return;
		
		whites_[w_pin.id_].push_back(o_pin);
		if (whites_[w_pin.id_].size() > 1) {
			for (auto& pin : whites_[w_pin.id_]) {
				pins_[pin.type_][pin.id_] += 1;
			}
		}
	}

	void Board::_whiteDisconnectPin(const Pin& w_pin, const Pin& o_pin)
	{
		if (w_pin.type_ != White)
			return;
		
		auto it = std::find(whites_[w_pin.id_].begin(), whites_[w_pin.id_].end(), o_pin);
		assert(it != whites_[w_pin.id_].end() && "Cannot remove a white disconnect a pin that wasn't connected");
		
		if (whites_[w_pin.id_].size() > 2) {
			pins_[o_pin.type_][o_pin.id_] -= 1;
		}
		else {
			for (auto& pin : whites_[w_pin.id_]) {
				pins_[pin.type_][pin.id_] -= 1;
			}
		}
		
		whites_[w_pin.id_].erase(std::remove(whites_[w_pin.id_].begin(), whites_[w_pin.id_].end(), o_pin), whites_[w_pin.id_].end());
	}

	void Board::_printPins()
	{
		std::cout << "\nPins:\n";
		for (auto& vec : pins_) {
			for (auto& connectedCount : vec) {
				std::cout << connectedCount << " ";
			}
			std::cout << '\n';
		}

		std::cout << "Whites:\n";
		for (auto& vec : whites_) {
			for (auto& pin : vec) {
				std::cout << pin.type_ << "/" << pin.id_ << " ";
			}
			std::cout << '\n';
		}
	}
}