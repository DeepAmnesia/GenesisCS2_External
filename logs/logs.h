#pragma once
#include "../render/render.hpp"
#include <algorithm>
#include <ctime>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

struct logs_data_t
{
	logs_data_t(const std::string& text, const std::string& icon, const Clock::time_point time, const ImColor color)
	{
		this->m_text = text;
		this->m_icon = icon;
		this->m_color = color;
		this->m_creation_time = time;
	}

	std::string m_text{};
	std::string m_icon{};
	Clock::time_point m_creation_time{};
	float m_spacing{};
	bool m_printed{};
	ImColor m_color{};
	float m_alpha{ 0.f };
};

namespace logs_system
{
	void instance();
	void push_log(const std::string& text, const std::string& icon = "", ImColor color = { 255, 255, 255 });
	void push_log(const std::string& text, bool smth);
	inline std::deque <logs_data_t> m_logs;
};

