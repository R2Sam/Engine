#pragma once
#include "Raylib/raylib.h"
#include "Raylib/rlgl.h"

#include <string>
#include <vector>
#include <cmath>

bool ColorCompare(const Color a, const Color b);

// Draw texture with scaling
void DrawTextureScale(const Texture2D& texture, const Vector2 position, const float scale, const Color color);
// Draw textures centred on the origin with rotation in degrees
void DrawTextureRot(const Texture2D& texture, const Vector2 position, const int rotation, const Color color);
// Same as above but scaling factor as well
void DrawTextureRotScale(const Texture2D& texture, const Vector2 position, const int rotation, const float scale, const Color color);
// Same as above but you input a rectangle to choose the desired sprite
void DrawTextureRotScaleSelect(const Texture2D& texture, const Rectangle selection, const Vector2 position, const int rotation, const float scale, const Color color);

// Draw text centred on a rec
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

// Draw a texture as a polygon of n points with no intersecting edges all visible to the centre
void DrawTexturePoly(Texture2D texture, Vector2 center, Vector2 *points, Vector2 *texcoords, int pointCount, Color tint);

// Convert a double to a string with a certain amount of precision
std::string DoubleToRoundedString(const double num, const int precision);

// Get a centred rectangle
Rectangle CenteredRectangle(const Rectangle rec, const Vector2 pos);

// Used for shadow maps
RenderTexture2D LoadShadowmapRenderTexture(int width, int height);
void UnloadShadowmapRenderTexture(RenderTexture2D target);