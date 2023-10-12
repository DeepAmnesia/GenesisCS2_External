#pragma once
#include <functional>
#include <deque>
#include <any>
#include <mutex>
#include <shared_mutex>
#include <DirectXMath.h>

#include "../classes/vector.hpp"
#include "../bytes/OpenSansBold.h"

#include "../libs/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../libs/imgui/imgui_internal.h"
#include "../libs/imgui/imgui_impl_dx9.h"
#include "../libs/imgui/imgui_impl_win32.h"

#include "../menu.h"

#define RAD2DEG(x) DirectX::XMConvertToDegrees( x ) 
#define DEG2RAD(x) DirectX::XMConvertToRadians( x )

extern ImFont* esp_font;
extern ImFont* menu_font;
extern ImFont* small_esp_font;
extern ImFont* font_awesome_font;
extern ImFont* icons_font;

enum DrawObjectType : int {
	DrawObject_NONE = 0,
	DrawObject_LINE = 1,
	DrawObject_RECT = 2,
	DrawObject_TEXT = 3,
	DrawObject_RECTMULTI = 4,
	DrawObject_CIRCLE3D = 5,
	DrawObject_CIRCLE2D = 6,
	DrawObject_CIRCLE2DFiled = 7,
	DrawObject_Arc = 8,
};
enum RectType_t : int
{
	RectType_Clear = 0,
	RectType_Filled = 1,
	RectType_Corners = 2
};

struct LineObject_t
{
	ImVec2 m_Min{ };
	ImVec2 m_Max{ };
	ImColor m_Color{ };
	float m_fThickness = 1.f;
};
struct RectObject_t
{
	ImVec2 m_Min{ };
	ImVec2 m_Max{ };
	ImColor m_Color{ };
	float m_fThickness = 1.f;
	float m_fRounding = 0.f;
	ImDrawCornerFlags m_tCorners = ImDrawCornerFlags_None;
	RectType_t m_RectType = RectType_Clear;
};
struct RectMultiObject_t
{
	ImVec2 m_Min{ };
	ImVec2 m_Max{ };
	ImColor m_Color1{ };
	ImColor m_Color2{ };
	ImColor m_Color3{ };
	ImColor m_Color4{ };
};
struct TextObject_t
{
	ImFont* m_Font = nullptr;
	float m_fFontSize = 14.f;
	ImVec2 m_Pos{ };
	std::string m_Text;
	ImColor m_Color{ };
	bool Outline = false;
	bool Shadow = false;
};
struct Circle3dObject_t
{
	Vector3 Pos{ };
	float Points = 16;
	ImColor Color{ };
	float Radius = 0.f;
};
struct ArcObject_t
{
	ImVec2 Pos{ };
	float Radius = 16;
	float MinAngle = 0.f;
	float MaxAngle = 0.f;
	ImColor Color;
	float Thikness;
};
struct DrawQueueObject_t
{
	DrawQueueObject_t(const DrawObjectType type, std::any&& object) : m_Type(type), m_Object(object) {};

	DrawObjectType m_Type;
	std::any m_Object;
};

class C_Renderer
{
public:
	virtual void Instance();
	virtual void SetupData();
	virtual void SwapDrawData();
	virtual void ClearDrawData();

	// renderer functions
	virtual void AddLine(ImVec2 min, ImVec2 max, ImColor col, float thickness = 1);
	virtual void AddRect(ImVec2 min, ImVec2 max, ImColor col, float thickness = 1, float rounding = 0, ImDrawCornerFlags corners = ImDrawCornerFlags_None, bool corner = false);
	virtual void AddRectFilled(ImVec2 min, ImVec2 max, ImColor col, float rounding = 0, ImDrawCornerFlags corners = ImDrawCornerFlags_None);
	virtual void AddRectFilledMultiColor(ImVec2 min, ImVec2 max, ImColor col, ImColor col2, ImColor col3, ImColor col4);
	virtual void AddText(ImFont* font, float font_size, ImVec2 pos, std::string text, ImColor color, bool outline = true, bool shadow = false);
	virtual void AddCircle3D(Vector3 position, float points, float radius, ImColor color);
	virtual void AddCircle2D(Vector3 position, float points, float radius, ImColor color);
	virtual void AddCircle2DFilled(Vector3 position, float points, float radius, ImColor color);
	virtual void AddArc(ImVec2 position, float radius, float min_angle, float max_angle, ImColor col, float thickness);
	
private:
	// draw list queue
	std::deque< DrawQueueObject_t > m_DrawListQueue;
	std::deque< DrawQueueObject_t > m_DrawListQueueSafe;

	// shared mtx
	std::shared_mutex m_DrawMutex = { };
};

inline C_Renderer* g_Renderer = new C_Renderer();