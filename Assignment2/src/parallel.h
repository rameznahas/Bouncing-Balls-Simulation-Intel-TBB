#pragma once
#include <vector>
#include <parallel_for.h>
#include <blocked_range2d.h>
#include <spin_mutex.h>
#include "ball.h"

namespace parallel {
	struct init_balls {
	public:
		init_balls(std::vector<ball>* const balls, const float min_rad);
		void operator()(const tbb::blocked_range<size_t>& r) const;

	private:
		std::vector<ball>* const p_balls;
		const float MIN_RADIUS;
	};

	struct init_pairs {
	public:
		init_pairs(std::vector<std::pair<ball*, ball*>>* const pairs, std::vector<ball>* const balls);
		void operator()(const tbb::blocked_range2d<size_t, size_t>& r) const;

	private:
		std::vector<std::pair<ball*, ball*>>* const p_pairs;
		std::vector<ball>* const p_balls;
	};

	struct wall_bounce {
	public:
		wall_bounce(std::vector<ball>* const balls);
		void operator()(const tbb::blocked_range<size_t>& r) const;

	private:
		std::vector<ball>* const p_balls;
	};

	struct ball_bounce {
	public:
		ball_bounce(std::vector<std::pair<ball*, ball*>>* const pairs);
		void operator()(const tbb::blocked_range<size_t>& r) const;

	private:
		std::vector<std::pair<ball*, ball*>>* const p_pairs;
	};

	struct update {
	public:
		update(std::vector<ball>* const balls, vector2d* g, const float dt);
		void operator()(const tbb::blocked_range<size_t>& r) const;

	private:
		std::vector<ball>* const p_balls;
		vector2d* p_GRAVITY;
		float delta_t;
	};
}