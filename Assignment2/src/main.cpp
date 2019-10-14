#include "bouncing_balls_sim.h"

bouncing_balls_sim* p_sim;

int main(int argc, char **argv) {
	bouncing_balls_sim sim(&argc, argv);
	p_sim = &sim;
	p_sim->start([]() {
		p_sim->update();
	});

	return 0;
}