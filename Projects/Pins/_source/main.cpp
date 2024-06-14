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

	auto green_a = board.addPin(pin::PinType::Green);
	auto green_b = board.addPin(pin::PinType::Green);
	auto blue_a = board.addPin(pin::PinType::Blue);
	auto blue_b = board.addPin(pin::PinType::Blue);
	
	auto white_a = board.addPin(pin::PinType::White);
	auto white_b = board.addPin(pin::PinType::White);
	
	// Red
	auto ropeRed = board.addRope(red_a, red_b);
	board.addRope(white_a, red_a);
	auto wRope = board.addRope(white_a, red_b);

	// Green
	board.addRope(green_a, green_b);

	// Blue
	auto bRope = board.addRope(blue_a, blue_b);
	auto waRope = board.addRope(white_a, red_c);

	board.removeRope(bRope);
	board.removeRope(ropeRed);
	board.removeRope(waRope);
	board.removeRope(wRope);


	return EXIT_SUCCESS;
}