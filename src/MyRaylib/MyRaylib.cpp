#include "MyRaylib/MyRaylib.h"

#include "Raylib/raylib.h"
#include "Raylib/raymath.h"

#include "Log/Log.h"

#include <iomanip>
#include <sstream>

Animation::Animation(const Texture2D& textureIn, const Rectangle rectangleIn, const int frameDurationIn) : _texture(textureIn), _rectangle(rectangleIn), _frameDuration(frameDurationIn)
{
    _texture = textureIn;

    _rows = _texture.height / _rectangle.height;

    _framesNumber = _texture.width / _rectangle.width * _rows;

    _totalFrameGrid = {_texture.width / _texture.height * _rows, _rows - 1};
}

Animation::~Animation()
{

}

void Animation::Randomize() 
{
    _currentFrameIndex = GetRandomValue(0, _framesNumber - 1);
}

void Animation::Start(const bool once)
{
    _once = once;
    _running = true;
}

void Animation::Stop()
{
    _running = false;
}

void Animation::Update()
{
    if (_running)
    {
        _frameCounter++;

        if (_frameCounter >= _frameDuration)
        {
            _currentFrameIndex = (_currentFrameIndex + 1) % _framesNumber;

            if (_frameGrid.x >= _totalFrameGrid.x && _frameGrid.y >= _totalFrameGrid.y)
            {
                _frameGrid = {0, 0};

                if (_once)
                {
                    _running = false;
                }
            }

            else if (_frameGrid.x >= _totalFrameGrid.x )
            {
                _frameGrid.x = 0;
                _frameGrid.y += 1;
            }

            else
                _frameGrid.x += 1;

            _frameCounter = 0;
        }
    }
}

void Animation::Draw(const Vector2 pos, const int rot, const float scale)
{
    Rectangle rec = _rectangle;

    rec.x = rec.width * _frameGrid.x;
    rec.y = rec.height * _frameGrid.y;

    DrawTextureRotScaleSelect(_texture, rec, pos, rot, scale, WHITE);
}

Button::Button(const bool toggleIn, const Rectangle recIn, const std::string& textIn) : _toggle(toggleIn), _rectangle(recIn), _text(textIn)
{
    _textMode = true;
}

Button::Button(const bool toggleIn, const Rectangle recIn, const std::string& textIn, const Color textColorIn, const Color bodyColorIn, const Color toggleBodyColorIn, const Color edgeColorIn) : _toggle(toggleIn), _rectangle(recIn), _text(textIn), _textColor(textColorIn), _bodyColor(bodyColorIn), _toggleBodyColor(toggleBodyColorIn), _edgeColor(edgeColorIn) 
{
    _textMode = true;
}

Button::Button(const bool toggleIn, const Rectangle recIn, const Texture2D& textureIn) : _toggle(toggleIn), _rectangle(recIn), _texture(textureIn)
{
    _textMode = false;
}

Button::~Button()
{

}

bool Button::Update(const Vector2 pos, const Vector2 mousePos, const bool mouseClick)
{
    _pos = pos;

    _trueRectangle = CenteredRectangle(_rectangle, _pos);
    
    _hover = CheckCollisionPointRec(mousePos, _trueRectangle);

    if (!_toggle)
    {
        _state = _hover && mouseClick;
    }

    else
    {
        if (_hover && mouseClick)
        {
            _state = !_state;
        }
    }

    if (_state)
    {
        _bodyColor = DARKBLUE; // Clicked color
    }
    else if (_hover)
    {
        _bodyColor = BLUE; // Hover color
    }
    else
    {
        _bodyColor = LIGHTGRAY; // Default color
    }


    return _state;
}

void Button::Draw()
{
    DrawRectangleRec(_trueRectangle, _edgeColor);

    if (_textMode)
    {
        Rectangle bodyRec = {_trueRectangle.x + 5, _trueRectangle.y + 5, _trueRectangle.width - 10, _trueRectangle.height - 10};

        if (!_state)
        {
            DrawTextRec(_text, 20, _textColor, bodyRec, _bodyColor);
        }

        else
        {
            DrawTextRec(_text, 20, _textColor, bodyRec, _toggleBodyColor);
        }
    }

    else if (IsTextureValid(_texture))
    {
        DrawTextureScale(_texture, _pos, 1.0, WHITE);
    }

    else
    {
        LogColor(LOG_RED, "No button texture!");
    }
}

Text::Text(const std::string& textIn, const Vector2 posIn) : _text(textIn), _pos(posIn)
{

}

Text::Text(const std::string& textIn, const Vector2 posIn, const int textSizeIn, const Color textColorIn) : _text(textIn), _pos(posIn), _textSize(textSizeIn), _textColor(textColorIn)
{

}

Text::~Text()
{

}

void Text::Update(const std::string& text, const Vector2 pos)
{
    _text = text;
    _pos = pos;
}

void Text::Draw()
{
    DrawText(_text.c_str(), _pos.x, _pos.y, _textSize, _textColor);
}

bool ColorCompare(const Color a, const Color b)
{
    if (a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a)
    {
        return true;
    }

    else
    {
        return false;
    }
}

void DrawTextureScale(const Texture2D& texture, const Vector2 position, const float scale, const Color color)
{
    DrawTexturePro(texture, Rectangle{ 0, 0, texture.width, texture.height },
                Rectangle{ position.x, position.y, texture.width * scale, texture.height * scale},
                Vector2{ texture.width * scale / 2.0f, texture.height * scale / 2.0f },
                0,
                color);
}

void DrawTextureRot(const Texture2D& texture, const Vector2 position, const int rotation, const Color color)
{
	DrawTexturePro(texture, Rectangle{ 0, 0, texture.width, texture.height },
                Rectangle{ position.x, position.y, texture.width, texture.height },
                Vector2{ texture.width / 2.0f, texture.height / 2.0f },
                rotation,
                color);
}

void DrawTextureRotScale(const Texture2D& texture, const Vector2 position, const int rotation, const float scale, const Color color)
{
    DrawTexturePro(texture, Rectangle{ 0, 0, texture.width, texture.height },
                Rectangle{ position.x, position.y, texture.width * scale, texture.height * scale},
                Vector2{ texture.width * scale / 2.0f, texture.height * scale / 2.0f },
                rotation,
                color);
}

void DrawTextureRotScaleSelect(const Texture2D& texture, const Rectangle selection, const Vector2 position, const int rotation, const float scale, const Color color)
{
    DrawTexturePro(texture, selection,
                Rectangle{ position.x, position.y, selection.width * scale, selection.height * scale},
                Vector2{ selection.width * scale / 2.0f, selection.height * scale / 2.0f },
                rotation,
                color);
}

void DrawTextRec(const std::string& text, const int fontSize, const Color textColor, const Rectangle rec, const Color recColor)
{
    Vector2 textSize = {MeasureText(text.c_str(), fontSize), fontSize};
    Vector2 textPos;

    textPos.x = rec.x + (rec.width / 2) - (textSize.x / 2);
    textPos.y = rec.y + (rec.height / 2) - (textSize.y / 2);

    DrawRectangleRec(rec, recColor);
    DrawText(text.c_str(), textPos.x, textPos.y, fontSize, textColor);
}

Vector2 Angle2Vector(const int degrees)
{
    float radians = DEG2RAD * degrees;

    return Vector2Normalize({cos(radians), sin(radians)});
}

int Vector2Angle(const Vector2 vec)
{
    float rad = atan2(vec.y, vec.x);
    rad += PI / 2;

    float deg = rad * RAD2DEG;
    if (deg < 0)
        deg += 360;

    return deg;
}

Vector2 Vector2Rot(const int length, const int rotation)
{
    Vector2 result = {0, -1};
    result = Vector2Scale(result, length);
    result = Vector2Rotate(result, rotation * DEG2RAD);
    return result;
}

int DegreeRot(const int rot)
{
    int rotation = rot % 360;

    if (rotation < 0)
        rotation += 360;

    return rotation;
}

int AngleFromPos(const Vector2 pos1, const Vector2 pos2)
{
    Vector2 direction = Vector2Subtract(pos1, pos2);
    int angle = atan2f(direction.x, -direction.y) * RAD2DEG;

    angle -= 180;

    if (angle < 0)
        angle += 360;

    else if (angle >= 360)
        angle -= 360;

    return angle;  
}

Rectangle ScaleRectangle(const Rectangle rectangle, const float scale, const Vector2 position)
{
    float newWidth = rectangle.width * scale;
    float newHeight = rectangle.height * scale;

    Rectangle newRectangle;
    newRectangle.x = position.x - newWidth / 2.0;
    newRectangle.y = position.y - newHeight / 2.0;
    newRectangle.width = newWidth;
    newRectangle.height = newHeight;

    return newRectangle;
}

Rectangle GetCameraRectangle(const Camera2D camera)
{
    return Rectangle{
        .x = camera.target.x - camera.offset.x / camera.zoom,
        .y = camera.target.y - camera.offset.y / camera.zoom,
        .width = GetScreenWidth() / camera.zoom,
        .height = GetScreenHeight() / camera.zoom
    };
}

bool IsTextureVisible(const Texture2D texture, const float scale, const Vector2 position, const Camera2D camera)
{
    Rectangle cameraView = GetCameraRectangle(camera);

    float scaledWidth = texture.width * scale;
    float scaledHeight = texture.height * scale;

    float worstCaseSize = sqrt(scaledWidth * scaledWidth + scaledHeight * scaledHeight);

    Rectangle worstCaseRect = {
        position.x - worstCaseSize / 2.0,
        position.y - worstCaseSize / 2.0,
        worstCaseSize + 2,
        worstCaseSize + 2
    };


    return CheckCollisionRecs(cameraView, worstCaseRect);
}

bool IsRectangleVisible(const Rectangle rectangle, const float scale, const Vector2 position, const Camera2D camera)
{
    Rectangle cameraView = GetCameraRectangle(camera);

    float scaledWidth = rectangle.width * scale;
    float scaledHeight = rectangle.height * scale;

    float worstCaseSize = sqrt(scaledWidth * scaledWidth + scaledHeight * scaledHeight);

    Rectangle worstCaseRect = {
        position.x - worstCaseSize / 2.0,
        position.y - worstCaseSize / 2.0,
        worstCaseSize + 2,
        worstCaseSize + 2
    };


    return CheckCollisionRecs(cameraView, worstCaseRect);
}

std::vector<std::string> WordList(const std::string& input)
{
    std::vector<std::string> words;
    std::istringstream iss(input);
    std::string token;

    while (std::getline(iss, token, ' ') || std::getline(iss, token, '\n'))
    {
        words.push_back(token);
    }

    return words;
}

void DrawTexturePoly(Texture2D texture, Vector2 center, Vector2 *points, Vector2 *texcoords, int pointCount, Color tint)
{
    rlSetTexture(texture.id);

    // Texturing is only supported on RL_QUADS
    rlBegin(RL_QUADS);

        rlColor4ub(tint.r, tint.g, tint.b, tint.a);

        for (int i = 0; i < pointCount - 1; i++)
        {
            rlTexCoord2f(0.5f, 0.5f);
            rlVertex2f(center.x, center.y);

            rlTexCoord2f(texcoords[i].x, texcoords[i].y);
            rlVertex2f(points[i].x + center.x, points[i].y + center.y);

            rlTexCoord2f(texcoords[i + 1].x, texcoords[i + 1].y);
            rlVertex2f(points[i + 1].x + center.x, points[i + 1].y + center.y);

            rlTexCoord2f(texcoords[i + 1].x, texcoords[i + 1].y);
            rlVertex2f(points[i + 1].x + center.x, points[i + 1].y + center.y);
        }
    rlEnd();

    rlSetTexture(0);
}

std::string DoubleToRoundedString(const double num, const int precision)
{
    double roundedNum = std::round(num * std::pow(10, precision)) / std::pow(10, precision);

    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << roundedNum;

    return stream.str();
}

Rectangle CenteredRectangle(const Rectangle rec, const Vector2 pos)
{
    return Rectangle{pos.x - (rec.width / 2), pos.y - (rec.height / 2), rec.width, rec.height};
}

RenderTexture2D LoadShadowmapRenderTexture(int width, int height)
{
    RenderTexture2D target = { 0 };

    target.id = rlLoadFramebuffer(); // Load an empty framebuffer
    target.texture.width = width;
    target.texture.height = height;

    if (target.id > 0)
    {
        rlEnableFramebuffer(target.id);

        target.depth.id = rlLoadTextureDepth(width, height, false);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
        target.depth.mipmaps = 1;

        rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

        if (rlFramebufferComplete(target.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

        rlDisableFramebuffer();
    }
    else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

    return target;
}

void UnloadShadowmapRenderTexture(RenderTexture2D target)
{
    if (target.id > 0)
    {
        rlUnloadFramebuffer(target.id);
    }
}