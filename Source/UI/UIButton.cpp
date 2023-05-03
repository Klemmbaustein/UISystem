#include <UI/UIButton.h>
#include <GL/glew.h>
#include "../Rendering/Shader.h"
#include "../Rendering/VertexBuffer.h"
#include <Application.h>
#include <Input.h>
#include <Math/MathHelpers.h>
#include <Rendering/ScrollObject.h>

namespace UI
{
	extern Shader* UIShader;
}

void UIButton::ScrollTick(Shader* UsedShader)
{
	if (CurrentScrollObject != nullptr)
	{
		glUniform3f(glGetUniformLocation(UsedShader->GetShaderID(), "u_offset"), -CurrentScrollObject->Percentage, CurrentScrollObject->Position.Y, CurrentScrollObject->Position.Y - CurrentScrollObject->Scale.Y);
	}
	else
		glUniform3f(glGetUniformLocation(UsedShader->GetShaderID(), "u_offset"), 0, -1000, 1000);
}

void UIButton::MakeGLBuffers()
{
	if (ButtonVertexBuffer)
		delete ButtonVertexBuffer;
	ButtonVertexBuffer = new VertexBuffer(
		{
			Vertex(Vector2f32(0, 0), Vector2f32(0, 0)),
			Vertex(Vector2f32(0, 1), Vector2f32(0, 1)),
			Vertex(Vector2f32(1, 0), Vector2f32(1, 0)),
			Vertex(Vector2f32(1, 1), Vector2f32(1, 1))
		},
		{
			0u, 1u, 2u,
			1u, 2u, 3u
		});
}

void UIButton::Tick()
{
	if (!IsVisible)
	{
		return;
	}
	ButtonColorMultiplier = 1.0f;
	if (UI::HoveredButton == this && !IsHovered)
	{
		RedrawUI();
		IsHovered = true;
	}
	if (IsHovered && UI::HoveredButton != this)
	{
		RedrawUI();
		IsHovered = false;
	}
	//if (CurrentScrollObject != nullptr)
	//{
	//	Offset.Y = CurrentScrollObject->Percentage;
	//}


	if (IsBeingHovered()) // do some very questionable math to check if the mouse is inside the scroll area
	{
		UI::NewHoveredButton = this;
		ButtonColorMultiplier = 0.8f;
	}
	else if (IsPressed && UI::HoveredButton != this)
	{
		IsSelected = false;
		IsPressed = false;
		RedrawUI();
		//if (CanBeDragged)
		//{
		//	if (ParentUI) ParentUI->OnButtonDragged(ButtonIndex);
		//}
	}

	if (IsSelected)
	{
		ButtonColorMultiplier = 0.6f;
	}

	if (UI::HoveredButton == this)
	{
		if (Input::IsLMBDown)
		{
			ButtonColorMultiplier = 0.5f;
			if (!IsPressed)
			{
				RedrawUI();
				IsPressed = true;
			}
		}
		else if (IsPressed)
		{
			if (!NeedsToBeSelected || IsSelected)
			{
				if (PressedFunc) Application::ButtonEvents.push_back(Application::ButtonEvent(PressedFunc));
				//if (Parent || ParentOverride)
				//{
				//	Application::ButtonEvents.insert(ButtonEvent(ParentOverride ? ParentOverride : Parent, nullptr, ButtonIndex));
				//}
				IsPressed = false;
				IsSelected = false;
				RedrawUI();
			}
			else
			{
				IsPressed = false;
				IsSelected = true;
			}
		}
	}
	else if (Input::IsLMBDown)
	{
		IsSelected = false;
	}
}


UIButton* UIButton::SetOpacity(float NewOpacity)
{
	if (NewOpacity != Opacity)
	{
		Opacity = NewOpacity;
		RedrawUI();
	}
	return this;
}

float UIButton::GetOpacity()
{
	return Opacity;
}

void UIButton::SetCanBeDragged(bool NewCanBeDragged)
{
	CanBeDragged = NewCanBeDragged;
}

bool UIButton::GetIsSelected()
{
	return IsSelected;
}

void UIButton::SetNeedsToBeSelected(bool NeedsToBeSelected)
{
	this->NeedsToBeSelected = NeedsToBeSelected;
}

bool UIButton::GetIsHovered()
{
	return IsHovered;
}

bool UIButton::GetIsPressed()
{
	return IsPressed;
}

UIButton* UIButton::SetUseTexture(bool UseTexture, unsigned int TextureID)
{
	this->UseTexture = UseTexture;
	this->TextureID = TextureID;
	return this;
}

UIButton* UIButton::SetColor(Vector3f32 NewColor)
{
	if (NewColor != Color)
	{
		Color = NewColor;
		RedrawUI();
	}
	return this;
}

Vector3f32 UIButton::GetColor()
{
	return Color;
}

UIButton::UIButton(bool Horizontal, Vector2f Position, Vector3f32 Color, void(*PressedFunc)()) : UIBox(Horizontal, Position)
{
	if (UI::UIShader == nullptr) UI::UIShader = new Shader("Shaders/uishader.vert", "Shaders/uishader.frag");
	this->PressedFunc = PressedFunc;
	this->Color = Color;
	MakeGLBuffers();
}

UIButton::~UIButton()
{
	delete ButtonVertexBuffer;
}

void UIButton::Update()
{
}

void UIButton::Draw()
{
	UI::UIShader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	ButtonVertexBuffer->Bind();
	ScrollTick(UI::UIShader);
	glUniform4f(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_transform"), OffsetPosition.X, OffsetPosition.Y, Size.X, Size.Y);
	glUniform4f(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_color"),
		ButtonColorMultiplier * Color.X, ButtonColorMultiplier * Color.Y, ButtonColorMultiplier * Color.Z, 1.f);
	glUniform1f(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_opacity"), Opacity);
	glUniform1i(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_borderType"), BorderType);
	glUniform1f(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_borderScale"), BorderRadius / 20.0f);
	glUniform1f(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_aspectratio"), Application::AspectRatio);

	if (UseTexture)
		glUniform1i(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_usetexture"), 1);
	else
		glUniform1i(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_usetexture"), 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	ButtonVertexBuffer->Unbind();
}
