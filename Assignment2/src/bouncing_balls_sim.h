#pragma once
#include <vector>
#include <time.h>
#include "ball.h"

class bouncing_balls_sim {
public:
	bouncing_balls_sim() = default;
	bouncing_balls_sim(int* argc, char **argv);
	void start(void(*callback)());
	void update();

private:
	void init();

	std::vector<ball> balls;
	std::vector<std::pair<ball*, ball*>> pairs;

	vector2d GRAVITY;

	clock_t previous_t;
	clock_t current_t;
	float delta_t;

	constexpr static float MIN_RADIUS = 0.05f;
};
