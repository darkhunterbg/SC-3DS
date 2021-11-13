#include "GUIImage.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Platform.h"
#include "../Util.h"

struct AnimationData {
	const Image* image = nullptr;
	int currentFrame = 0;
	int timer = 0;
};

void GUIImage::DrawTextureAbsolute(const Texture& texture, Rectangle rect, Color color)
{
	GraphicsRenderer::Draw(texture, GUI::GetPosition(rect.position), rect.size, Color32(color));
}

void GUIImage::DrawTexture(const Texture& texture, Color color)
{
	Rectangle rect = GUI::GetLayoutSpace();
	GraphicsRenderer::Draw(texture, rect.position, rect.size, Color32(color));
}
void GUIImage::DrawSubTexture(const Texture& texture, Rectangle subImage, Color color)
{
	Rectangle rect = GUI::GetLayoutSpace();
	ImageFrame imgFrame;
	imgFrame.texture = &texture;
	imgFrame.offset = Vector2Int16(subImage.position);
	imgFrame.size = Vector2Int16(subImage.size);
	imgFrame.uv = Platform::GenerateUV(texture.GetTextureId(), Rectangle_ToRectangle16(subImage));

	GraphicsRenderer::Draw(imgFrame, rect, color);
}

void GUIImage::DrawImage(const Image& image, unsigned subImage, Color color)
{
	const ImageFrame& frame = image.GetFrame(subImage);
	Rectangle rect = GUI::GetLayoutSpace();
	GraphicsRenderer::Draw(frame, rect, color);
}

void GUIImage::DrawImageFrame(const ImageFrame& frame, Color color)
{
	Rectangle rect = GUI::GetLayoutSpace();
	GraphicsRenderer::Draw(frame, rect, color);
}

void GUIImage::DrawImageFrame(const ImageFrame& frame, Vector2Int offset, Color color)
{
	GUI::AddNextElementOffset(offset);
	DrawImageFrame(frame, color);
}

void GUIImage::DrawColor(Color color)
{
	Rectangle rect = GUI::GetLayoutSpace();
	GraphicsRenderer::DrawRectangle({ rect.position, rect.size }, color);
}

void GUIImage::DrawColorAbsolute(Rectangle rect, Color color)
{
	GraphicsRenderer::DrawRectangle({ GUI::GetPosition(rect.position), rect.size }, color);
}

void GUIImage::DrawAnimatedImage(const char* id, const Image& image, int* frameState, Color color)
{
	std::string key = id;
	key += ".AnimData";

	AnimationData* data = GUI::GetResourceById<AnimationData>(key);
	if (data == nullptr)
	{
		data = new	AnimationData();
		GUI::RegisterResource(key, data, [](void* p) {delete (AnimationData*)p;  });
	}

	if (data->image != &image)
	{
		data->image = &image;
		data->currentFrame = 0;
		data->timer = GUI::GetState().ImageAnimationTimer + 1;
	}

	if (frameState)
	{
		if (data->currentFrame != *frameState)
		{
			data->currentFrame = *frameState;
			data->timer = GUI::GetState().ImageAnimationTimer + 1;
		}
	}

	data->timer--;
	if (data->timer <= 0)
	{
		data->timer = GUI::GetState().ImageAnimationTimer;
		data->currentFrame = (++data->currentFrame) % data->image->GetFrames().Size();
		if (frameState)
			*frameState = data->currentFrame;
	}

	const ImageFrame& frame = data->image->GetFrame(data->currentFrame);

	Rectangle rect = GUI::GetLayoutSpace();
	Vector2Int offset = rect.size / 2;
	rect.position += Vector2Int(data->image->GetImageFrameOffset(data->currentFrame, false));
	rect.position += rect.size / 2;
	rect.size = (Vector2Int(frame.size) * rect.size) / Vector2Int(data->image->GetSize());

	GUI::BeginAbsoluteLayout(rect);
	DrawImageFrame(frame, color);
	GUI::EndLayout();
}

void GUIImage::DrawBorder(int bordeThickness, Color color)
{
	Rectangle rect = GUI::GetLayoutSpace();
	Util::DrawTransparentRectangle(rect, bordeThickness, color);
}
