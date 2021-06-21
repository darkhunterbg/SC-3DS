#include "GUIAnimator.h"

#include "Assets.h"
#include "../Data/GameDatabase.h"

void GUIAnimator::SetAnimationFromImage(const std::string& imageName, int frameDuration)
{
	SetAnimationFromImage(GameDatabase::instance->GetImage(imageName), frameDuration);
}

void GUIAnimator::SetAnimationFromImage(const Image& image, int animationTime)
{
	this->image = &image;
	frameDuration = animationTime;
	frameTimer = frameDuration;
	frameId = 0;
	frames = image.GetFrames();
}

const ImageFrame& GUIAnimator::GetImageFrame() const
{
	return frames[frameId];
}

Vector2Int16 GUIAnimator::GetImageFrameOffset() const
{
	return image->GetImageFrameOffset(GetImageFrame(), false);
}

void GUIAnimator::Update()
{
	frameTimer--;

	if (frameTimer <= 0) {
		frameTimer = frameDuration;
		frameId = (++frameId) % frames.Size();
	}

}
