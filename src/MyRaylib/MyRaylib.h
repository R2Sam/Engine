#pragma once
#include "Raylib/raylib.h"
#include "Raylib/rlgl.h"

#include <string>
#include <vector>
#include <cmath>

class Animation
{
public:

	Animation(const Texture2D& textureIn, const Rectangle rectangleIn, const int frameDurationIn);
	~Animation();

	void Randomize();

	void Start(const bool once);
	void Stop();

	void Update();
	void Draw(const Vector2 pos, const int rot, const float scale);

private:

	// Texture, base rectangle and amount of frames
	Texture2D _texture;
	Rectangle _rectangle;
	int _rows;
	int _framesNumber;


	int _currentFrameIndex = 0;
	Vector2 _frameGrid = {-1, 0};
	Vector2 _totalFrameGrid;

	// How many game frames per animation frame
	int _frameDuration;
	int _frameCounter = 0;

	// Flags
	bool _running = false;
	bool _once = false;
};

class Button
{
public:

	Button(const bool toggleIn, const Rectangle recIn, const std::string& textIn);
	Button(const bool toggleIn, const Rectangle recIn, const std::string& textIn, const Color textColorIn, const Color bodyColorIn, const Color toggleBodyColorIn, const Color edgeColorIn);
	Button(const bool toggleIn, const Rectangle recIn, const Texture2D& textureIn);
	~Button();

	bool Update(const Vector2 pos, const Vector2 mousePos, const bool mouseClick);
	void Draw();

private:

	//  Button box
	Rectangle _rectangle;

	// Center pos;
	Vector2 _pos;

	// True rectangle according to position
	Rectangle _trueRectangle;

	// Can have text and color or texture
	std::string _text;
	Color _textColor = BLACK;
	Color _bodyColor = LIGHTGRAY;
	Color _toggleBodyColor = GRAY;
	Color _edgeColor = GRAY;

	Texture2D _texture;

	// What mode
	bool _textMode;

	// If is a toggle
	bool _toggle = false;

	bool _state = false;

	bool _hover = false;
};

class Text
{
private:

	std::string _text;

	Vector2 _pos = {0, 0};

	int _textSize = 20;
	Color _textColor = BLACK;

public:

	Text(const std::string& textIn, const Vector2 posIn);
	Text(const std::string& textIn, const Vector2 posIn, const int textSizeIn, const Color textColorIn);
	~Text();

	void Update(const std::string& text, const Vector2 pos);

	void Draw();
};

bool ColorCompare(const Color a, const Color b);

// Draw texture with scaling
void DrawTextureScale(const Texture2D& texture, const Vector2 position, const float scale, const Color color);
// Draw textures centered on the origin with rotation in degrees
void DrawTextureRot(const Texture2D& texture, const Vector2 position, const int rotation, const Color color);
// Same as above but scaling factor aswell
void DrawTextureRotScale(const Texture2D& texture, const Vector2 position, const int rotation, const float scale, const Color color);
// Same as above but you input a rectangle to choose the desired sprite
void DrawTextureRotScaleSelect(const Texture2D& texture, const Rectangle selection, const Vector2 position, const int rotation, const float scale, const Color color);

// Draw text centered on a rec
void DrawTextRec(const std::string& text, const int fontSize, const Color textColor, const Rectangle rec, const Color recColor);

// Turn an angle in degrees to a vector
Vector2 Angle2Vector(const int degrees);
// Get angle of vector
int Vector2Angle(const Vector2 vec);
// Get a vector of certain length and rotation in degrees
Vector2 Vector2Rot(const int length, const int rotation);

// Convert from vector rot to bound by 360 degrees
int DegreeRot(const int rot);

// Angle from one pos to another
int AngleFromPos(const Vector2 pos1, const Vector2 pos2);

// Scale a rectangle
Rectangle ScaleRectangle(const Rectangle rectangle, const float scale, const Vector2 position);

// Get 2D camera rectangle
Rectangle GetCameraRectangle(const Camera2D camera);

// Check for texture visibility
bool IsTextureVisible(const Texture2D texture, const float scale, const Vector2 position, const Camera2D camera);
bool IsRectangleVisible(const Rectangle rectangle, const float scale, const Vector2 position, const Camera2D camera);

// String to list of words
std::vector<std::string> WordList(const std::string& input);

// Draw a texture as a polygon of n points with no intersecting edges all visible to the center
void DrawTexturePoly(Texture2D texture, Vector2 center, Vector2 *points, Vector2 *texcoords, int pointCount, Color tint);

// Convert a double to a string with a certain amout of precision
std::string DoubleToRoundedString(const double num, const int precision);

// Get a centered rectangle
Rectangle CenteredRectangle(const Rectangle rec, const Vector2 pos);

// Used for shadowmaps
RenderTexture2D LoadShadowmapRenderTexture(int width, int height);
void UnloadShadowmapRenderTexture(RenderTexture2D target);