#include <parallel_for.h>
#include <glew.h>
#include <freeglut.h>
#include "tasks.h"
#include "parallel.h"

/////////////////////////////////wall_coll//////////////////////////////////
tasks::wall_collision::wall_collision(std::vector<ball>* balls, std::vector<std::pair<ball*, ball*>>* pairs)
	:
	p_balls(balls),
	p_pairs(pairs)
{}


tbb::task* tasks::wall_collision::execute() {
	tbb::parallel_for(tbb::blocked_range<size_t>(0, p_balls->size()), parallel::wall_bounce(p_balls), tbb::auto_partitioner());
	tasks::ball_collision& bc = *new(tbb::task::allocate_root()) tasks::ball_collision(p_balls, p_pairs);

	return &bc;
}
////////////////////////////////////////////////////////////////////////////
/////////////////////////////////ball_coll//////////////////////////////////
tasks::ball_collision::ball_collision(std::vector<ball>* balls, std::vector<std::pair<ball*, ball*>>* pairs)
	:
	p_balls(balls),
	p_pairs(pairs)
{}

tbb::task* tasks::ball_collision::execute() {
	tbb::parallel_for(tbb::blocked_range<size_t>(0, p_pairs->size()), parallel::ball_bounce(p_pairs), tbb::auto_partitioner());
	tasks::draw& draw = *new(tbb::task::allocate_root()) tasks::draw(p_balls);

	return &draw;
}
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////update///////////////////////////////////
tasks::update::update(std::vector<ball>* balls, std::vector<std::pair<ball*, ball*>>* pairs, vector2d* g, const float dt)
	:
	p_balls(balls),
	p_pairs(pairs),
	p_GRAVITY(g),
	delta_t(dt)
{}

tbb::task* tasks::update::execute() {
	tbb::parallel_for(tbb::blocked_range<size_t>(0, p_balls->size()), parallel::update(p_balls, p_GRAVITY, delta_t), tbb::auto_partitioner());
	tasks::wall_collision& wc = *new(tbb::task::allocate_root()) tasks::wall_collision(p_balls, p_pairs);

	return &wc;
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////draw////////////////////////////////////
tasks::draw::draw(std::vector<ball>* balls)
	:
	p_balls(balls)
{}

tbb::task* tasks::draw::execute() {
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (ball& ball : *p_balls) {
		glBegin(GL_POLYGON);
		glColor4f(ball.color.x, ball.color.y, ball.color.z, 0.25f);

		for (int i = 0; i < NUM_POINTS; ++i) {
			float angle = i * DEGREE_TO_RAD;

			glVertex2d
			(
				ball.radius * cos(angle) + ball.center.x,	// x-coord
				ball.radius * sin(angle) + ball.center.y	// y-coord
			);
		}

		glEnd();
	}

	glutSwapBuffers();

	return nullptr;
}
////////////////////////////////////////////////////////////////////////////
