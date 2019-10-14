#pragma once
#include <task.h>
#include <vector>
#include "vectors.h"
#include "ball.h"

namespace tasks {
	/////////////////////////////////wall_coll//////////////////////////////////
	struct wall_collision : public tbb::task {
	public:
		wall_collision(std::vector<ball>* balls, std::vector<std::pair<ball*, ball*>>* pairs);
		tbb::task* execute() override;

	private:
		std::vector<ball>* p_balls;
		std::vector<std::pair<ball*, ball*>>* p_pairs;
	};
	////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////ball_coll//////////////////////////////////
	struct ball_collision : public tbb::task {
	public:
		ball_collision(std::vector<ball>* balls, std::vector<std::pair<ball*, ball*>>* pairs);
		tbb::task* execute() override;

	private:
		std::vector<ball>* p_balls;
		std::vector<std::pair<ball*, ball*>>* p_pairs; 
	};
	////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////update///////////////////////////////////
	struct update : public tbb::task{
	public:
		update(std::vector<ball>* balls, std::vector<std::pair<ball*, ball*>>* pairs, vector2d* g, const float dt);
		tbb::task* execute() override;

	private:
		std::vector<ball>* p_balls;
		std::vector<std::pair<ball*, ball*>>* p_pairs;
		vector2d* p_GRAVITY;
		float delta_t;
	};
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////draw////////////////////////////////////
	struct draw : public tbb::task {
	public:
		draw(std::vector<ball>* balls);
		tbb::task* execute() override;

	private:
		std::vector<ball>* p_balls;

		constexpr static float PI = 3.141592f;
		constexpr static float DEGREE_TO_RAD = PI / 180;
		constexpr static int NUM_POINTS = 360;
	};
	////////////////////////////////////////////////////////////////////////////
}

