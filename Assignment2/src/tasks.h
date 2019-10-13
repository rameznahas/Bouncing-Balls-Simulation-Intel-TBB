#pragma once
#include <task.h>
#include <vector>
#include "vectors.h"
#include "ball.h"

namespace tasks {
	/////////////////////////////////wall_coll//////////////////////////////////
	struct wall_collision : public tbb::task {
	public:
		wall_collision(std::vector<ball>* balls, tbb::task* next);
		tbb::task* execute() override;

	private:
		std::vector<ball>* p_balls;
		tbb::task* p_next;
	};
	////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////ball_coll//////////////////////////////////
	struct ball_collision : public tbb::task {
	public:
		ball_collision(std::vector<std::pair<ball*, ball*>>* pairs, tbb::task* next);
		tbb::task* execute() override;

	private:
		std::vector<std::pair<ball*, ball*>>* p_pairs; 
		tbb::task* p_next;
	};
	////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////update///////////////////////////////////
	struct update : public tbb::task{
	public:
		update(std::vector<ball>* balls, tbb::task* next, vector2d* g, const float dt);
		tbb::task* execute() override;

	private:
		std::vector<ball>* p_balls;
		tbb::task* p_next;
		vector2d* p_GRAVITY;
		float delta_t;
	};
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////draw////////////////////////////////////
	struct draw : public tbb::task {
	public:
		draw(std::vector<ball>* balls, tbb::task* next);
		tbb::task* execute() override;

	private:
		std::vector<ball>* p_balls;
		tbb::task* p_next;

		constexpr static float PI = 3.141592f;
		constexpr static float DEGREE_TO_RAD = PI / 180;
		constexpr static int NUM_POINTS = 360;
	};
	////////////////////////////////////////////////////////////////////////////
}

