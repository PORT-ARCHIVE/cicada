
#include <iostream>
#include "SemiCrf.hpp"

int main(int argc, char *argv[])
{
	//std::shared_ptr<SemiCrf::Label> pLabel(new SemiCrf::Label());
	//std::shared_ptr<SemiCrf::Label> pl0(new SemiCrf::Campany());
	SemiCrf::Label l0(new SemiCrf::Campany());
	SemiCrf::Segment s0(0, 10, l0);

	SemiCrf::Label l1(new SemiCrf::Location());
	SemiCrf::Segment s1(0, 10, l1);

	
	SemiCrf::SemiCrf semicrf;
	
	return (0);
}
