#include "logs.h"

inline float definition_in_seconds(Clock::time_point start, Clock::time_point end)
{
	return std::chrono::duration_cast<milliseconds>(end - start).count() / 1000.f;
}

void logs_system::push_log(const std::string& text, const std::string& icon, const ImColor color)
{
	m_logs.emplace_front
	(
		logs_data_t
		(
			text,								// m_text
			icon,								// m_icon
			Clock::now(),						// m_creation_time
			color								// m_color
		)
	);

	while (m_logs.size() > 100)
		m_logs.pop_back();
}

void logs_system::push_log(const std::string& text, bool smth)
{
	m_logs.emplace_front
	(
		logs_data_t
		(
			text,								// m_text
			"",									// m_icon
			Clock::now(),						// m_creation_time
			ImColor(255,255,255)				// m_color
		)
	);

	while (m_logs.size() > 100)
		m_logs.pop_back();
}

void logs_system::instance()
{

	for (int i{}; i < /*m_logs.max_size()*/ m_logs.size(); i++)
	{
		auto* logs = &m_logs[i];
		if (!logs)
			continue;

		const auto time_after_creation = definition_in_seconds(logs->m_creation_time, Clock::now());

		if (time_after_creation >= 5.0f)
		{
			m_logs.erase(m_logs.begin() + i);
			/* m_logs.clear(); */ // buffer overflow fix
		}

		if (time_after_creation < 3.f)
		{
			logs->m_alpha = std::clamp(logs->m_alpha + ImGui::GetIO().DeltaTime * 4 ,0.f, 1.f);
			logs->m_spacing = std::clamp(logs->m_spacing + ImGui::GetIO().DeltaTime * 48.f, 0.0f, 24.0f);
		}
		else 
		{
			logs->m_alpha = std::clamp(logs->m_alpha - ImGui::GetIO().DeltaTime * 4, 0.f, 1.f); 
			logs->m_spacing = std::clamp(logs->m_spacing - ImGui::GetIO().DeltaTime * 48.f, 0.0f, 24.0f);
		}

		auto spacing = 0.0f;
		if (i)
		{
			for (int i2{}; i2 < i; i2++)
				spacing += m_logs[i2].m_spacing;
		}
		ImColor clr = ImColor(logs->m_color.Value.x, logs->m_color.Value.y, logs->m_color.Value.z, logs->m_alpha);
		g_Renderer->AddText(esp_font, 16.f, ImVec2(5, spacing + 2.5f), std::string(logs->m_text).c_str(), clr, false, true);
		
	}
}