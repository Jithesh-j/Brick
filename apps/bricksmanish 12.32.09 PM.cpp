#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <GLUT/glut.h>


struct Brick {
    float x;
    float y;
    float width;
    float height;
    bool isVisible;

    Brick(float _x, float _y, float _width, float _height) : x(_x), y(_y), width(_width), height(_height), isVisible(true) {}
};

void drawRectangle(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawBrick(float x, float y, float width, float height) {
    glColor3f(0.0f, 1.0f, 0.0f); // Set color to green for bricks
    drawRectangle(x, y, width, height);
}

void drawBrickWall(std::vector<Brick>& bricks) {
    for (const Brick& brick : bricks) {
        if (brick.isVisible) {
            drawBrick(brick.x, brick.y, brick.width, brick.height);
        }
    }
}

void drawBall(float x, float y, float radius) {
    int segments = 30;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(segments);
        float dx = radius * cos(angle);
        float dy = radius * sin(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

float ballSpeedX = 0.01f;
float ballSpeedY = 0.01f;
bool ballIdle = true;
bool gameOver = false;

float barWidth = 0.2f;
float barHeight = 0.02f;
float barX = -barWidth / 2;
float barY = -0.9f;

bool moveLeft = false;
bool moveRight = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ENTER && ballIdle) {
            ballSpeedY = 0.01f;
            ballIdle = false;
        }
        else if (key == GLFW_KEY_LEFT) {
            moveLeft = true;
        }
        else if (key == GLFW_KEY_RIGHT) {
            moveRight = true;
        }
    }
    else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT) {
            moveLeft = false;
        }
        else if (key == GLFW_KEY_RIGHT) {
            moveRight = false;
        }
    }
}

bool checkBarCollision(float ballX, float ballY, float ballRadius) {
    return ballX + ballRadius >= barX && ballX - ballRadius <= barX + barWidth &&
           ballY + ballRadius >= barY && ballY - ballRadius <= barY + barHeight;
}

float calculateReflectionAngle(float barY, float ballY, float barHeight) {
    float relativeIntersectY = (barY + barHeight / 2) - ballY;
    float normalizedIntersectY = relativeIntersectY / (barHeight / 2);
    float bounceAngle = normalizedIntersectY * (3.14f/ 3);
    return bounceAngle;
}

bool checkBrickCollision(float& ballX, float& ballY, float ballRadius, Brick& brick) {
    if (brick.isVisible) {
        float brickLeft = brick.x;
        float brickRight = brick.x + brick.width;
        float brickTop = brick.y;
        float brickBottom = brick.y + brick.height;

        if (ballX + ballRadius >= brickLeft && ballX - ballRadius <= brickRight &&
            ballY + ballRadius >= brickTop && ballY - ballRadius <= brickBottom) {
            brick.isVisible = false;

            float hitPoint = (ballX - brickLeft) / brick.width;
            float newAngle = (hitPoint - 0.5f) * 3.14f;

            float currentSpeed = sqrt(ballSpeedX * ballSpeedX + ballSpeedY * ballSpeedY);

            ballSpeedX = currentSpeed * cos(newAngle);
            ballSpeedY = -abs(currentSpeed * sin(newAngle));

            return true;
        }
    }
    return false;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Ball on Bar", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    float ballRadius = 0.03f;
    float ballX = barX + barWidth / 2;
    float ballY = barY + barHeight + ballRadius;

    std::vector<Brick> bricks;
    int rows = 4;
    int cols = 10;
    float brickWidth = 0.1f;
    float brickHeight = 0.05f;
    float padding = 0.02f;

    float startX = -brickWidth * cols / 2.0f;
    float startY = 0.8f;

    for (int i = 0; i < rows; ++i) {
        float rowY = startY - (brickHeight + padding) * i;
        for (int j = 0; j < cols; ++j) {
            float colX = startX + (brickWidth + padding) * j;
            bricks.emplace_back(colX, rowY, brickWidth, brickHeight);
        }
    }

    while (!glfwWindowShouldClose(window) && !gameOver) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawBrickWall(bricks);

        glColor3f(1.0f, 1.0f, 1.0f);
        drawRectangle(barX, barY, barWidth, barHeight);

        const float barMoveSpeed = 0.015f;
        if (moveLeft) {
            barX -= barMoveSpeed;
        }
        if (moveRight) {
            barX += barMoveSpeed;
        }

        if (!ballIdle) {
            ballX += ballSpeedX;
            ballY += ballSpeedY;

            if (checkBarCollision(ballX, ballY, ballRadius)) {
                float hitPoint = (ballX - barX) / barWidth;
                float newAngle = (hitPoint - 0.5f) * 3.14f;

                float currentSpeed = sqrt(ballSpeedX * ballSpeedX + ballSpeedY * ballSpeedY);

                ballSpeedX = currentSpeed * cos(newAngle);
                ballSpeedY = abs(currentSpeed * sin(newAngle));
            }

            if (ballY - ballRadius <= barY + barHeight / 2) {
                gameOver = true;
            }

            for (Brick& brick : bricks) {
                if (checkBrickCollision(ballX, ballY, ballRadius, brick)) {
                    break;
                }
            }
        }
        else {
            ballX = barX + barWidth / 2;
        }

        if (ballX - ballRadius <= -1.0f || ballX + ballRadius >= 1.0f) {
            ballSpeedX = -ballSpeedX;
        }
        if (ballY + ballRadius >= 1.0f || ballY - ballRadius <= -1.0f) {
            ballSpeedY = -ballSpeedY;
        }

        glColor3f(1.0f, 0.0f, 0.0f);
        drawBall(ballX, ballY, ballRadius);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (gameOver) {
        std::cout << "You lost" << std::endl;
    }

    glfwTerminate();
    return 0;
}