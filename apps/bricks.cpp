#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "GLUT/glut.h"
const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
const int BRICK_ROWS = 5, BRICK_COLS = 10;
const int BRICK_WIDTH = 58, BRICK_HEIGHT = 20;
const int BRICK_SPACING = 2;
const int PADDLE_WIDTH = 100, PADDLE_HEIGHT = 20;
const int BALL_RADIUS = 10;

struct Brick {
    float x, y, width, height;
    int type;  // Different types for different resistances
    int hitsRemaining;
    bool isVisible;

    Brick(float x, float y, float w, float h, int t)
            : x(x), y(y), width(w), height(h), type(t), isVisible(true) {
        switch (type) {
            case 1: hitsRemaining = 4; break; // Bright Orange
            case 2: hitsRemaining = 3; break; // Dark Orange
            case 3: hitsRemaining = 2; break; // Maroon
            case 4: hitsRemaining = 1; break; // Brown
            case 5: hitsRemaining = 5; break; // Steel Box (special case)
        }
    }
};

struct Ball {
    float x, y, dx, dy, radius;
    bool idle;
    Ball() : x(SCREEN_WIDTH / 2), y(SCREEN_HEIGHT - 50), dx(0.0f), dy(0.0f), radius(BALL_RADIUS), idle(true) {}
};

struct Paddle {
    float x, y, width, height;
    bool moveLeft, moveRight;
    Paddle() : x(SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2), y(SCREEN_HEIGHT - 30), width(PADDLE_WIDTH), height(PADDLE_HEIGHT), moveLeft(false), moveRight(false) {}
};

std::vector<Brick> bricks;
Ball ball;
Paddle paddle;
int score = 0; // Initialize score variable

void setupBricks() {
    float startX = (SCREEN_WIDTH - (BRICK_COLS * (BRICK_WIDTH + BRICK_SPACING))) / 2.0f;
    float startY = 50;
    for (int i = 0; i < BRICK_ROWS; ++i) {
        for (int j = 0; j < BRICK_COLS; ++j) {
            bricks.emplace_back(startX + j * (BRICK_WIDTH + BRICK_SPACING), startY + i * (BRICK_HEIGHT + BRICK_SPACING), BRICK_WIDTH, BRICK_HEIGHT, 1 + rand() % 5);
        }
    }
}

void drawBrick(const Brick& brick) {
    if (!brick.isVisible) return;
    glColor3f(0.5f * brick.type, 0.1f * brick.hitsRemaining, 0.05f * brick.type);
    glBegin(GL_QUADS);
    glVertex2f(brick.x, brick.y);
    glVertex2f(brick.x + brick.width, brick.y);
    glVertex2f(brick.x + brick.width, brick.y + brick.height);
    glVertex2f(brick.x, brick.y + brick.height);
    glEnd();
}

void drawBall() {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 360; i++) {
        float degInRad = i * M_PI / 180;
        glVertex2f(cos(degInRad) * ball.radius + ball.x, sin(degInRad) * ball.radius + ball.y);
    }
    glEnd();
}

void drawPaddle() {
    glBegin(GL_QUADS);
    glVertex2f(paddle.x, paddle.y);
    glVertex2f(paddle.x + paddle.width, paddle.y);
    glVertex2f(paddle.x + paddle.width, paddle.y + paddle.height);
    glVertex2f(paddle.x, paddle.y + paddle.height);
    glEnd();
}

void checkCollisions() {
    // Check ball and brick collisions
    for (auto& brick : bricks) {
        if (brick.isVisible && ball.x + ball.radius >= brick.x && ball.x - ball.radius <= brick.x + brick.width &&
            ball.y + ball.radius >= brick.y && ball.y - ball.radius <= brick.y + brick.height) {
            brick.hitsRemaining--;
            ball.dy = -ball.dy;
            if (brick.hitsRemaining <= 0) {
                brick.isVisible = false;
                score += 10; // Increase score when a brick is destroyed
            }
            break;
        }
    }

    // Check ball and paddle collisions
    if (ball.y + ball.radius >= paddle.y && ball.x >= paddle.x && ball.x <= paddle.x + paddle.width) {
        ball.dy = -ball.dy;
    }

    // Check for walls
    if (ball.x < 0 || ball.x > SCREEN_WIDTH) ball.dx = -ball.dx;
    if (ball.y < 0 || ball.y > SCREEN_HEIGHT) ball.dy = -ball.dy;
}

void updateBall() {
    ball.x += ball.dx;
    ball.y += ball.dy;
    checkCollisions();
    if (ball.y > paddle.y + paddle.height) {
        // End the game
        glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
    }
}

void updatePaddle() {
    const float paddleSpeed = 5.0f; // Adjust this value for different speeds
    if (paddle.moveLeft && paddle.x > 0) {
        paddle.x -= paddleSpeed;
    }
    if (paddle.moveRight && paddle.x + paddle.width < SCREEN_WIDTH) {
        paddle.x += paddleSpeed;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_LEFT:
            paddle.moveLeft = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_RIGHT:
            paddle.moveRight = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_ENTER:
            if (action == GLFW_PRESS && ball.idle) {
                ball.idle = false;
                ball.dx = 4.0f; // Initial velocity
                ball.dy = -4.0f; // Initial velocity
            }
            break;
    }
}

void initOpenGL() {
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void drawScore() {
    glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white
    glRasterPos2f(10, SCREEN_HEIGHT - 30); // Set the position of the text

    std::string scoreString = "Score: " + std::to_string(score); // Convert the score to string
    for (char ch : scoreString) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ch); // Draw each character of the score string
    }
}


int main(void) {
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Brick Breaker Game", NULL, NULL);
    if (!window) {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window\n";
        return -1;
    }

    // Center the window on the screen
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int centerX = (mode->width - SCREEN_WIDTH) / 2;
    int centerY = (mode->height - SCREEN_HEIGHT) / 2;
    glfwSetWindowPos(window, centerX, centerY);

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    initOpenGL();
    setupBricks();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColor3f(1.0f, 1.0f, 1.0f);

        // Update and render game elements
        updatePaddle();
        drawBall();
        drawPaddle();
        for (const auto& brick : bricks) drawBrick(brick);
        drawScore(); // Draw the score

        if (!ball.idle) {
            updateBall();
        }

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Clean up
    glfwTerminate();
    return 0;
}
