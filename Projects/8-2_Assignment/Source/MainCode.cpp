#include <GLFW\glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>
#include <math.h>

using namespace std;

const float DEG2RAD = 3.14159f / 180.0f;

void processInput(GLFWwindow* window);

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE };
enum ONOFF { ON, OFF };

// ---------------------------------------------------------------------------
// Brick class
// Supports multi-hit destruction with color shifting per hit.
// hitPoints determines how many collisions are required to destroy the brick.
// Each hit shifts the brick color toward red to signal damage visually.
// ---------------------------------------------------------------------------
class Brick
{
public:
    float red, green, blue;       // Current display color
    float baseRed, baseGreen, baseBlue; // Original color for interpolation
    float x, y, width;
    BRICKTYPE brick_type;
    ONOFF onoff;
    int hitPoints;                // Hits remaining before destruction
    int maxHitPoints;             // Original hit point total

    Brick(BRICKTYPE bt, float xx, float yy, float ww,
        float rr, float gg, float bb, int hp = 1)
    {
        brick_type = bt;
        x = xx; y = yy; width = ww;
        red = rr; green = gg; blue = bb;
        baseRed = rr; baseGreen = gg; baseBlue = bb;
        onoff = ON;
        hitPoints = hp;
        maxHitPoints = hp;
    }

    // Called when a circle collides with this destructible brick.
    // Decrements hit points and shifts color toward red to show damage.
    // Turns the brick off when hit points reach zero.
    void TakeHit()
    {
        if (onoff == OFF) return;
        hitPoints--;

        if (hitPoints <= 0)
        {
            onoff = OFF;
        }
        else
        {
            // Interpolate color toward red based on damage fraction
            float damageFraction = 1.0f - ((float)hitPoints / (float)maxHitPoints);
            red = baseRed + (1.0f - baseRed) * damageFraction;
            green = baseGreen - baseGreen * damageFraction;
            blue = baseBlue - baseBlue * damageFraction;
        }
    }

    void drawBrick()
    {
        if (onoff == OFF) return;

        double halfside = width / 2;

        // Draw the main brick face
        glColor3f(red, green, blue);
        glBegin(GL_POLYGON);
        glVertex2f(x + halfside, y + halfside);
        glVertex2f(x + halfside, y - halfside);
        glVertex2f(x - halfside, y - halfside);
        glVertex2f(x - halfside, y + halfside);
        glEnd();

        // Draw a dark border outline to give bricks definition
        glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x + halfside, y + halfside);
        glVertex2f(x + halfside, y - halfside);
        glVertex2f(x - halfside, y - halfside);
        glVertex2f(x - halfside, y + halfside);
        glEnd();

        // Draw crack lines on destructible multi-hit bricks to show damage state
        if (brick_type == DESTRUCTABLE && maxHitPoints > 1 && hitPoints < maxHitPoints)
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(1.5f);
            int crackLevels = maxHitPoints - hitPoints;

            // First crack: diagonal from top-left toward center
            if (crackLevels >= 1)
            {
                glBegin(GL_LINES);
                glVertex2f(x - halfside * 0.8f, y + halfside * 0.8f);
                glVertex2f(x + halfside * 0.1f, y - halfside * 0.1f);
                glEnd();
            }
            // Second crack: diagonal from bottom-right toward center
            if (crackLevels >= 2)
            {
                glBegin(GL_LINES);
                glVertex2f(x + halfside * 0.8f, y - halfside * 0.8f);
                glVertex2f(x - halfside * 0.1f, y + halfside * 0.3f);
                glEnd();
            }
            // Third crack: horizontal fracture line
            if (crackLevels >= 3)
            {
                glBegin(GL_LINES);
                glVertex2f(x - halfside, y);
                glVertex2f(x + halfside, y * 0.9f);
                glEnd();
            }
            glLineWidth(1.0f);
        }
    }
};


// ---------------------------------------------------------------------------
// Paddle class
// Player-controlled horizontal paddle at the bottom of the screen.
// Moves left and right with arrow keys. Acts as a reflective surface.
// ---------------------------------------------------------------------------
class Paddle
{
public:
    float x, y, width, height;
    float speed;

    Paddle()
    {
        x = 0.0f;
        y = -0.85f;   // Near the bottom of the screen
        width = 0.3f;
        height = 0.04f;
        speed = 0.03f;
    }

    void MoveLeft()
    {
        if (x - width / 2 > -1.0f)
            x -= speed;
    }

    void MoveRight()
    {
        if (x + width / 2 < 1.0f)
            x += speed;
    }

    // Returns true if a circle at (cx, cy) with given radius overlaps the paddle.
    // Used by Circle to trigger an upward reflection off the paddle surface.
    bool CheckCircleCollision(float cx, float cy, float radius)
    {
        float halfW = width / 2;
        float halfH = height / 2;
        return (cx > x - halfW - radius && cx < x + halfW + radius &&
            cy > y - halfH - radius && cy < y + halfH + radius);
    }

    void Draw()
    {
        float halfW = width / 2;
        float halfH = height / 2;

        // Paddle body: bright white
        glColor3f(0.9f, 0.9f, 0.95f);
        glBegin(GL_POLYGON);
        glVertex2f(x - halfW, y + halfH);
        glVertex2f(x + halfW, y + halfH);
        glVertex2f(x + halfW, y - halfH);
        glVertex2f(x - halfW, y - halfH);
        glEnd();

        // Paddle border
        glColor3f(0.4f, 0.4f, 0.6f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x - halfW, y + halfH);
        glVertex2f(x + halfW, y + halfH);
        glVertex2f(x + halfW, y - halfH);
        glVertex2f(x - halfW, y - halfH);
        glEnd();
    }
};


// ---------------------------------------------------------------------------
// Circle class
// Uses vx/vy velocity components instead of integer directions so that
// physics-based reflections (wall, brick, paddle, circle) are accurate.
// ---------------------------------------------------------------------------
class Circle
{
public:
    float red, green, blue;
    float radius;
    float x, y;
    float vx, vy;           // Velocity components
    float speed;
    bool  active;           // False means the circle should be removed

    Circle(float xx, float yy, float rad, float r, float g, float b)
    {
        x = xx;
        y = yy;
        radius = rad;
        red = r;
        green = g;
        blue = b;
        speed = 0.008f;
        active = true;

        // Assign a random initial velocity direction
        float angle = ((rand() % 360)) * DEG2RAD;
        vx = cos(angle) * speed;
        vy = sin(angle) * speed;

        // Guarantee the circle moves upward initially so it reaches the bricks
        if (vy < 0) vy = -vy;
    }

    // ------------------------------------------------------------------
    // CheckCollision with a Brick
    // Uses velocity component flipping for physically accurate reflection.
    // Reflective bricks mirror the velocity; destructible bricks take a hit.
    // ------------------------------------------------------------------
    void CheckCollision(Brick* brk)
    {
        if (!active || brk->onoff == OFF) return;

        float halfW = brk->width / 2;

        // Compute closest point on brick to circle center
        float closestX = max(brk->x - halfW, min(x, brk->x + halfW));
        float closestY = max(brk->y - halfW, min(y, brk->y + halfW));

        float dx = x - closestX;
        float dy = y - closestY;
        float distSq = dx * dx + dy * dy;

        if (distSq > radius * radius) return; // No collision

        // Determine which face was hit to flip the correct velocity axis
        float overlapX = (brk->x + halfW) - (x - radius);
        float overlapXNeg = (x + radius) - (brk->x - halfW);
        float overlapY = (brk->y + halfW) - (y - radius);
        float overlapYNeg = (y + radius) - (brk->y - halfW);

        float minOverlapX = min(overlapX, overlapXNeg);
        float minOverlapY = min(overlapY, overlapYNeg);

        if (minOverlapX < minOverlapY)
            vx = -vx;   // Hit left or right face
        else
            vy = -vy;   // Hit top or bottom face

        if (brk->brick_type == DESTRUCTABLE)
        {
            brk->TakeHit();
        }
    }

    // ------------------------------------------------------------------
    // CheckPaddleCollision
    // Reflects the circle upward off the paddle surface.
    // Adds a small horizontal nudge based on where the circle hits
    // the paddle so the player has some control over the angle.
    // ------------------------------------------------------------------
    void CheckPaddleCollision(Paddle* paddle)
    {
        if (!active) return;
        if (!paddle->CheckCircleCollision(x, y, radius)) return;

        // Reflect vertically
        if (vy < 0) vy = -vy;

        // Offset angle based on hit position relative to paddle center
        float hitOffset = (x - paddle->x) / (paddle->width / 2);
        vx += hitOffset * 0.01f;

        // Clamp vx so it doesn't get out of control
        if (vx > 0.06f) vx = 0.06f;
        if (vx < -0.06f) vx = -0.06f;
    }

    // ------------------------------------------------------------------
    // CheckCircleCollision
    // When two circles overlap, reflect both and swap colors.
    // ------------------------------------------------------------------
    void CheckCircleCollision(Circle* other)
    {
        if (!active || !other->active || this == other) return;

        float dx = other->x - x;
        float dy = other->y - y;
        float distSq = dx * dx + dy * dy;
        float minDist = radius + other->radius;

        if (distSq >= minDist * minDist) return; // No collision

        // Reflect both circles along the collision normal
        float dist = sqrt(distSq);
        float nx = dx / dist;   // Collision normal x
        float ny = dy / dist;   // Collision normal y

        // Separate the circles so they don't stick together
        float overlap = minDist - dist;
        x -= nx * overlap * 0.5f;
        y -= ny * overlap * 0.5f;
        other->x += nx * overlap * 0.5f;
        other->y += ny * overlap * 0.5f;

        // Exchange velocity components along the collision normal (elastic collision)
        float dvx = vx - other->vx;
        float dvy = vy - other->vy;
        float dot = dvx * nx + dvy * ny;

        vx -= dot * nx;
        vy -= dot * ny;
        other->vx += dot * nx;
        other->vy += dot * ny;

        // Swap colors to visually signal the collision
        float tmpR = red, tmpG = green, tmpB = blue;
        red = other->red;   green = other->green; blue = other->blue;
        other->red = tmpR;  other->green = tmpG;  other->blue = tmpB;
    }

    // ------------------------------------------------------------------
    // MoveOneStep
    // Moves the circle by its velocity vector and reflects off all four
    // screen edges with a small speed boost each bounce for escalating energy.
    // ------------------------------------------------------------------
    void MoveOneStep()
    {
        if (!active) return;

        x += vx;
        y += vy;

        // Left wall reflection
        if (x - radius < -1.0f)
        {
            x = -1.0f + radius;
            vx = -vx;
            speed *= 1.05f;  // Small speed increase per wall bounce
            ScaleVelocity();
        }
        // Right wall reflection
        if (x + radius > 1.0f)
        {
            x = 1.0f - radius;
            vx = -vx;
            speed *= 1.05f;
            ScaleVelocity();
        }
        // Top wall reflection
        if (y + radius > 1.0f)
        {
            y = 1.0f - radius;
            vy = -vy;
            speed *= 1.05f;
            ScaleVelocity();
        }
        // Bottom wall: circle is destroyed if it slips past the paddle
        if (y - radius < -1.0f)
        {
            active = false;
            return;
        }

        // Cap speed so animation stays controllable
        if (speed > 0.04f) speed = 0.04f;
    }

    // Rescale vx/vy to maintain the current speed magnitude
    void ScaleVelocity()
    {
        float mag = sqrt(vx * vx + vy * vy);
        if (mag > 0.0001f)
        {
            vx = (vx / mag) * speed;
            vy = (vy / mag) * speed;
        }
    }

    void DrawCircle()
    {
        if (!active) return;

        // Draw glow ring slightly larger than the circle
        glColor4f(red * 0.5f, green * 0.5f, blue * 0.5f, 0.3f);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++)
        {
            float degInRad = i * DEG2RAD;
            glVertex2f((cos(degInRad) * (radius + 0.01f)) + x,
                (sin(degInRad) * (radius + 0.01f)) + y);
        }
        glEnd();

        // Draw the circle itself
        glColor3f(red, green, blue);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++)
        {
            float degInRad = i * DEG2RAD;
            glVertex2f((cos(degInRad) * radius) + x,
                (sin(degInRad) * radius) + y);
        }
        glEnd();
    }
};


// ---------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------
vector<Circle> world;
Paddle paddle;

// Builds the brick layout: a grid of destructible bricks across the top
// half of the screen, with a row of reflective bricks acting as a bumper
// row in the middle. Bricks vary in color and hit points for visual variety.
vector<Brick> BuildBrickLayout()
{
    vector<Brick> bricks;

    float brickWidth = 0.18f;
    float spacing = 0.22f;
    float startX = -0.88f;
    float topY = 0.85f;

    // Row colors cycling through warm and cool tones
    float rowColors[5][3] = {
        {0.95f, 0.25f, 0.25f},  // Red row
        {0.95f, 0.65f, 0.15f},  // Orange row
        {0.85f, 0.90f, 0.20f},  // Yellow row
        {0.20f, 0.80f, 0.40f},  // Green row
        {0.25f, 0.55f, 0.95f},  // Blue row
    };

    // Build 5 rows of 9 destructible bricks
    // Higher rows require more hits (adds challenge and color variety)
    for (int row = 0; row < 5; row++)
    {
        int hitsRequired = 5 - row;  // Top row takes 5 hits, bottom row takes 1
        for (int col = 0; col < 9; col++)
        {
            float bx = startX + col * spacing;
            float by = topY - row * spacing * 0.65f;
            bricks.push_back(Brick(
                DESTRUCTABLE, bx, by, brickWidth,
                rowColors[row][0], rowColors[row][1], rowColors[row][2],
                hitsRequired
            ));
        }
    }

    // Add a row of smaller reflective bumper bricks in the middle of the screen
    for (int col = 0; col < 5; col++)
    {
        float bx = -0.4f + col * 0.22f;
        bricks.push_back(Brick(
            REFLECTIVE, bx, 0.05f, 0.10f,
            0.7f, 0.3f, 0.9f   // Purple bumpers
        ));
    }

    return bricks;
}


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(void)
{
    srand(time(NULL));

    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(600, 600, "8-2 Assignment", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Build the brick layout once at startup
    vector<Brick> bricks = BuildBrickLayout();

    while (!glfwWindowShouldClose(window))
    {
        // Setup viewport
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.05f, 0.05f, 0.12f, 1.0f);  // Dark navy background
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window);

        // Remove circles that fell past the paddle (active == false)
        world.erase(
            remove_if(world.begin(), world.end(), [](const Circle& c) { return !c.active; }),
            world.end()
        );

        // Run circle-to-circle collision checks for every unique pair
        for (int i = 0; i < (int)world.size(); i++)
        {
            for (int j = i + 1; j < (int)world.size(); j++)
            {
                world[i].CheckCircleCollision(&world[j]);
            }
        }

        // Run circle-to-brick and circle-to-paddle collision checks, then move
        for (int i = 0; i < (int)world.size(); i++)
        {
            for (int b = 0; b < (int)bricks.size(); b++)
            {
                world[i].CheckCollision(&bricks[b]);
            }

            world[i].CheckPaddleCollision(&paddle);
            world[i].MoveOneStep();
            world[i].DrawCircle();
        }

        // Draw all bricks
        for (int b = 0; b < (int)bricks.size(); b++)
        {
            bricks[b].drawBrick();
        }

        // Draw the player paddle
        paddle.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}


// ---------------------------------------------------------------------------
// processInput
// SPACE: spawn a new circle at the center with a random color.
// LEFT/RIGHT arrows: move the paddle.
// ESCAPE: close the window.
// ---------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Track previous space bar state so we only spawn one circle per key press,
    // not one per frame while the key is held down
    static int prevSpaceState = GLFW_RELEASE;
    int currSpaceState = glfwGetKey(window, GLFW_KEY_SPACE);

    if (currSpaceState == GLFW_PRESS && prevSpaceState == GLFW_RELEASE)
    {
        float r = (float)(rand() % 100) / 100.0f;
        float g = (float)(rand() % 100) / 100.0f;
        float b = (float)(rand() % 100) / 100.0f;

        // Ensure the color is bright enough to be visible on a dark background
        if (r + g + b < 0.6f) r = 0.6f;

        Circle newCircle(0.0f, -0.7f, 0.04f, r, g, b);
        world.push_back(newCircle);
    }

    prevSpaceState = currSpaceState;

    // Paddle controls
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) paddle.MoveLeft();
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) paddle.MoveRight();
}