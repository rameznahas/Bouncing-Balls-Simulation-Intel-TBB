#include <string>
#include <glew.h>
#include <freeglut.h>
#include <task.h>
#include "bouncing_balls_sim.h"
#include "parallel.h"
#include "tasks.h"

#define WWIDTH 800
#define WHEIGHT 800
#define UPDATE_FREQ 1.f / 30
#define BALL_COUNT 10

bouncing_balls_sim::bouncing_balls_sim(int* argc, char **argv)
	:
	balls(*argc > 1 ? std::stoi(argv[1]) : BALL_COUNT),
	GRAVITY(0.f, -1.5f),
	previous_t(0),
	current_t(0),
	delta_t(UPDATE_FREQ)
{
	glutInit(argc, argv);
	glutInitWindowPosition(-1, -1);
	glutInitWindowSize(WWIDTH, WHEIGHT);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA);
	glutCreateWindow("Bouncing Balls Simulation");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

/*
	The control thread task.
*/
void bouncing_balls_sim::update() {
	//update current clock time
	current_t = clock();
	delta_t = (float)(current_t - previous_t) / CLOCKS_PER_SEC;

	// don't draw if delta_t is faster than 30 fps
	if (delta_t < UPDATE_FREQ) return;

	// store last draw time
	previous_t = current_t;

	// send command to worker threads (throught intel tbb's task scheduler)
	// to start the computation for the next frame
	tasks::update& update_ = *new(tbb::task::allocate_root()) tasks::update(&balls, &pairs, &GRAVITY, delta_t);
	tbb::task::spawn_root_and_wait(update_);
}

void bouncing_balls_sim::start(void(*callback)()) {
	init();

	glutDisplayFunc(callback);
	glutIdleFunc(callback);
	glutMainLoop();
}

/*
	Initialization of simulation.
		- Creates balls with random radius, centers and velocities.
		- Caches all the unique ball pairs (optimization).
		- Creates the worker threads and assigns their tasks.
*/
void bouncing_balls_sim::init() {
	size_t size = balls.size();

	// create and cache random balls in parallel
	tbb::parallel_for(tbb::blocked_range<size_t>(0, size), parallel::init_balls(&balls, MIN_RADIUS), tbb::auto_partitioner());
	// create and cache the unique pair of balls in parallel
	tbb::parallel_for(tbb::blocked_range2d<size_t, size_t>(0, size, 0, size), parallel::init_pairs(&pairs, &balls), tbb::auto_partitioner());
}
