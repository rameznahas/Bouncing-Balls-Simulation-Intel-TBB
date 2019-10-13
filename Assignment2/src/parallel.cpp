#include <random>
#include "parallel.h"

/////////////////////////////////init balls/////////////////////////////////
parallel::init_balls::init_balls(std::vector<ball>* const balls, const float min_rad)
	:
	p_balls(balls),
	MIN_RADIUS(min_rad)
{}

void parallel::init_balls::operator()(const tbb::blocked_range<size_t>& r) const {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> rad(1, 3);
	std::uniform_real_distribution<float> vel(-1.f, 1.f);
	std::vector<ball>* balls = p_balls;

	for (size_t i = r.begin(); i != r.end(); ++i) {
		int test = rad(gen);
		float radius = MIN_RADIUS * test; // random radius

 		float ur_bound = radius - 1;
		float ll_bound = 1 - radius;

		std::uniform_real_distribution<float> coord(ur_bound, ll_bound); // so we dont get balls out of bounds
		vector2d center(coord(gen), coord(gen));

		int weight = (int)(radius * 100.0f);
		vector2d velocity(vel(gen), vel(gen));
		balls->at(i) = ball(radius, center, velocity, weight);
	}
}
////////////////////////////////////////////////////////////////////////////
/////////////////////////////////init pairs/////////////////////////////////
parallel::init_pairs::init_pairs(std::vector<std::pair<ball*, ball*>>* const pairs, std::vector<ball>* const balls)
	:
	p_pairs(pairs),
	p_balls(balls)
{}
tbb::spin_mutex mutex;

void parallel::init_pairs::operator()(const tbb::blocked_range2d<size_t, size_t>& r) const {
	std::vector<std::pair<ball*, ball*>>* pairs = p_pairs;
	std::vector<ball>* balls = p_balls;

	for (size_t i = r.rows().begin(); i != r.rows().end(); ++i) {
		for (size_t j = r.cols().begin(); j != r.cols().end(); ++j) {
			if (j > i) {
				tbb::spin_mutex::scoped_lock lock(mutex);
				pairs->push_back({ &balls->at(i), &balls->at(j) });
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////
/////////////////////////////////wall bounce////////////////////////////////
parallel::wall_bounce::wall_bounce(std::vector<ball>* const balls)
	:
	p_balls(balls)
{}

void parallel::wall_bounce::operator()(const tbb::blocked_range<size_t>& r) const {
	std::vector<ball>* balls = p_balls;

	for (size_t i = r.begin(); i != r.end(); ++i) {
		ball& current = balls->at(i);

		float t_wall = 1.f - current.radius;
		float b_wall = current.radius - 1.f;
		float r_wall = t_wall;
		float l_wall = b_wall;

		if (current.center.x > r_wall) {
			current.center.x = r_wall;
			current.velocity.x *= -1.f;
		}
		else if (current.center.x < l_wall) {
			current.center.x = l_wall;
			current.velocity.x *= -1.f;
		}

		if (current.center.y > t_wall) {
			current.center.y = t_wall;
			current.velocity.y *= -1.f;
		}
		else if (current.center.y < b_wall) {
			current.center.y = b_wall;
			current.velocity.y *= -1.f;
		}
	}
}
////////////////////////////////////////////////////////////////////////////
/////////////////////////////////ball bounce////////////////////////////////
parallel::ball_bounce::ball_bounce(std::vector<std::pair<ball*, ball*>>* const pairs)
	:
	p_pairs(pairs)
{}

void parallel::ball_bounce::operator()(const tbb::blocked_range<size_t>& r) const {
	std::vector<std::pair<ball*, ball*>>* pairs = p_pairs;

	for (size_t i = r.begin(); i != r.end(); ++i) {
		ball* current = pairs->at(i).first;
		ball* other = pairs->at(i).second;

		float min_dist = current->radius + other->radius;


		if (current->center.x + min_dist > other->center.x
			&& current->center.y + min_dist > other->center.y
			&& other->center.x + min_dist > current->center.x
			&& other->center.y + min_dist > current->center.y) {
			vector2d c = current->center - other->center;
			//float min_dist = current->radius + other->radius;

			// balls are close enough, but it does not mean they have collided.
			// check for ball collision.
			// if true, collision occured, handle it
			if (powf(c.x, 2.f) + powf(c.y, 2.f) <= powf(min_dist, 2.f)) {
				float distance = vector2d::magnitude(c);
				float overlap = 0.5f * (distance - current->radius - other->radius);

				vector2d dir = vector2d::normalize(c);

				current->center += -overlap * dir;
				other->center += overlap * dir;

				vector2d v = current->velocity - other->velocity;
				int m = current->mass + other->mass;
				float mag = powf(distance, 2.f);
				float dot_vc = vector2d::dot(v, c);
				float ratio = 2.f * dot_vc / (m * mag);

				current->velocity -= (other->mass * ratio * c);
				other->velocity += (current->mass * ratio * c);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////update///////////////////////////////////
parallel::update::update(std::vector<ball>* const balls, vector2d* g, const float dt)
	:
	p_balls(balls),
	p_GRAVITY(g),
	delta_t(dt)
{}

void parallel::update::operator()(const tbb::blocked_range<size_t>& r) const {
	std::vector<ball>* balls = p_balls;

	for (size_t i = r.begin(); i != r.end(); ++i) {
		ball& current = balls->at(i);
		current.velocity += delta_t * (*p_GRAVITY);
		current.center += delta_t * current.velocity;
	}
}
////////////////////////////////////////////////////////////////////////////