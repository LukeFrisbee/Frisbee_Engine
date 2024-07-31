#include <iostream>

#include "board.h"

int main()
{
	std::cout << "pin board!\n";
	std::cout << std::boolalpha;

	auto board = pin::Board();

	auto red_a = board.addPin(pin::PinType::Red);
	auto red_b = board.addPin(pin::PinType::Red);
	auto red_c = board.addPin(pin::PinType::Red);
	auto red_d = board.addPin(pin::PinType::Red);

	auto rope = board.addRope(red_a, red_b);
	auto ropea = board.addRope(red_c, red_d);
	// auto ropec = board.addRope(red_b,

	return EXIT_SUCCESS;
}