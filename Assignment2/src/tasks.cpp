#include <parallel_for.h>
#include <glew.h>
#include <freeglut.h>
#include "tasks.h"
#include "parallel.h"

/////////////////////////////////wall_coll//////////////////////////////////
tasks::wall_collision::wall_collision(std::vector<ball>* balls, tbb::task* next)
	:
	p_balls(balls),
	p_next(next)
{}


tbb::task* tasks::wall_collision::execute() {
	tbb::parallel_for(tbb::blocked_range<size_t>(0, p_balls->size()), parallel::wall_bounce(p_balls), tbb::auto_partitioner());

	return p_next;
}
////////////////////////////////////////////////////////////////////////////
/////////////////////////////////ball_coll//////////////////////////////////
tasks::ball_collision::ball_collision(std::vector<std::pair<ball*, ball*>>* pairs, tbb::task* next)
	:
	p_pairs(pairs),
	p_next(next)
{}

tbb::task* tasks::ball_collision::execute() {
	tbb::parallel_for(tbb::blocked_range<size_t>(0, p_pairs->size()), parallel::ball_bounce(p_pairs), tbb::auto_partitioner());

	return p_next;
}
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////update///////////////////////////////////
tasks::update::update(std::vector<ball>* balls, tbb::task* next, vector2d* g, const float dt)
	:
	p_balls(balls),
	p_next(next),
	p_GRAVITY(g),
	delta_t(dt)
{}

tbb::task* tasks::update::execute() {
	tbb::parallel_for(tbb::blocked_range<size_t>(0, p_balls->size()), parallel::update(p_balls, p_GRAVITY, delta_t), tbb::auto_partitioner());

	return p_next;
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////draw////////////////////////////////////
tasks::draw::draw(std::vector<ball>* balls, tbb::task* next)
	:
	p_balls(balls),
	p_next(next)
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

	return p_next;
}
////////////////////////////////////////////////////////////////////////////
