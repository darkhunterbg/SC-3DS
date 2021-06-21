#pragma once

#include "../Span.h"
#include "../MathLib.h"
#include <string>

struct ImageFrame;
class Image;

class GUIAnimator {

private:
	int frameId = 0;
	int frameTimer = 0;
	int frameDuration = 10;
	Span<ImageFrame> frames;
	const Image* image;
public:
	void SetAnimationFromImage(const std::string& imageName, int frameDuration);
	void SetAnimationFromImage(const Image& image, int frameDuration);

	const ImageFrame& GetImageFrame() const;
	Vector2Int16 GetImageFrameOffset() const;

	void Update();
};