#include "Radar.h"
#include "../configs/cheat_cfg.hpp"

Vector3 RevolveCoordinatesSystem(float RevolveAngle, Vector3 OriginPos, Vector3 DestPos)
{
	Vector3 ResultPos;
	if (RevolveAngle == 0)
		return DestPos;
	ResultPos.x = OriginPos.x + (DestPos.x - OriginPos.x) * cos(RevolveAngle * M_PI / 180) + (DestPos.y - OriginPos.y) * sin(RevolveAngle * M_PI / 180);
	ResultPos.y = OriginPos.y - (DestPos.x - OriginPos.x) * sin(RevolveAngle * M_PI / 180) + (DestPos.y - OriginPos.y) * cos(RevolveAngle * M_PI / 180);
	return ResultPos;
}


void Base_Radar::SetRange(const float& Range)
{
	this->RenderRange = Range;
}

void Base_Radar::SetCrossColor(const ImColor& Color)
{
	this->CrossColor = Color;
}

void Base_Radar::SetPos(const Vector3& Pos)
{
	this->Pos = Pos;
}

void Base_Radar::SetSize(const float& Size)
{
	this->Width = Size;
}

float Base_Radar::GetSize()
{
	return this->Width;
}

Vector3 Base_Radar::GetPos()
{
	return this->Pos;
}

void Base_Radar::SetProportion(const float& Proportion)
{
	this->Proportion = Proportion;
}

void Base_Radar::SetDrawList(ImDrawList* DrawList)
{
	this->DrawList = DrawList;
}

void Base_Radar::AddPoint(const Vector3& LocalPos, const float& LocalYaw, const Vector3& Pos, ImColor Color, int Type, float Yaw)
{
	Vector3 PointPos;
	float Distance;
	float Angle;

	this->LocalYaw = LocalYaw;

	Distance = sqrt(pow(LocalPos.x - Pos.x, 2) + pow(LocalPos.y - Pos.y, 2));

	Angle = atan2(Pos.y - LocalPos.y, Pos.x - LocalPos.x) * 180 / M_PI;
	Angle = (this->LocalYaw - Angle) * M_PI / 180;

	Distance = Distance / this->Proportion * this->RenderRange * 2;

	PointPos.x = this->Pos.x + Distance * sin(Angle);
	PointPos.y = this->Pos.y - Distance * cos(Angle);

	// Circle range
	//Distance = sqrt(pow(this->Pos.x - PointPos.x, 2) + pow(this->Pos.y - PointPos.y, 2));
	//if (Distance > this->RenderRange)
	//	return;

	// Rectangle range

	if (PointPos.x < this->Pos.x - RenderRange || PointPos.x > this->Pos.x + RenderRange
		|| PointPos.y > this->Pos.y + RenderRange || PointPos.y < this->Pos.y - RenderRange)
		return;

	std::tuple<Vector3, ImColor, int, float> Data(PointPos, Color, Type, Yaw);
	this->Points.push_back(Data);
}


void Base_Radar::Render()
{
	if (this->DrawList == nullptr)
		return;
	if (Width <= 0)
		return;

	// Cross
	std::pair<Vector3, Vector3> Line1;
	std::pair<Vector3, Vector3> Line2;

	Line1.first = Vector3(this->Pos.x - this->Width / 2, this->Pos.y);
	Line1.second = Vector3(this->Pos.x + this->Width / 2, this->Pos.y);
	Line2.first = Vector3(this->Pos.x, this->Pos.y - this->Width / 2);
	Line2.second = Vector3(this->Pos.x, this->Pos.y + this->Width / 2);
#ifndef clean_logs
	std::cout << "radar ent size: " << this->Points.size() << std::endl;
#endif
	if (this->Opened)
	{
		if (this->ShowCrossLine)
		{
			ImColor clr = ImColor(settings::get_color(_S("Radar crossline"))[0], settings::get_color(_S("Radar crossline"))[1], settings::get_color(_S("Radar crossline"))[2]);
			this->DrawList->AddLine(ImVec2(Line1.first.x, Line1.first.y), ImVec2(Line1.second.x, Line1.second.y), clr, 1);
			this->DrawList->AddLine(ImVec2(Line2.first.x, Line2.first.y), ImVec2(Line2.second.x, Line2.second.y), clr, 1);
		}

		for (auto PointSingle : this->Points)
		{

			Vector3	PointPos = std::get<0>(PointSingle);
			ImColor PointColor = std::get<1>(PointSingle);
			int		PointType = std::get<2>(PointSingle);
			float	PointYaw = std::get<3>(PointSingle);

#ifndef clean_logs
			std::cout << "point x " << PointPos.x << std::endl;
			std::cout << "point y " << PointPos.y << std::endl;
			std::cout << "point z " << PointPos.z << std::endl;
#endif
			ImVec2 TrianglePoint, TrianglePoint_1, TrianglePoint_2;
			float Angle = (this->LocalYaw - PointYaw) - 90;

			this->DrawList->AddCircleFilled(ImVec2(PointPos.x, PointPos.y), this->ArcArrowSize, PointColor, 30);
			this->DrawList->AddCircle(ImVec2(PointPos.x, PointPos.y), this->ArcArrowSize, ImColor(0, 0, 0, 150), 0, 0.1);

			TrianglePoint.x = PointPos.x + (this->ArcArrowSize + this->ArcArrowSize / 3) * cos(-Angle * M_PI / 180);
			TrianglePoint.y = PointPos.y - (this->ArcArrowSize + this->ArcArrowSize / 3) * sin(-Angle * M_PI / 180);

			TrianglePoint_1.x = PointPos.x + this->ArcArrowSize * cos(-(Angle - 30) * M_PI / 180);
			TrianglePoint_1.y = PointPos.y - this->ArcArrowSize * sin(-(Angle - 30) * M_PI / 180);

			TrianglePoint_2.x = PointPos.x + this->ArcArrowSize * cos(-(Angle + 30) * M_PI / 180);
			TrianglePoint_2.y = PointPos.y - this->ArcArrowSize * sin(-(Angle + 30) * M_PI / 180);

			this->DrawList->PathLineTo(TrianglePoint);
			this->DrawList->PathLineTo(TrianglePoint_1);
			this->DrawList->PathLineTo(TrianglePoint_2);
			this->DrawList->PathFillConvex(ImColor(220, 220, 220, 240));
		}
	}

	if (this->Points.size() > 0)
		this->Points.clear();
}