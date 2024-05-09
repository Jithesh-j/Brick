#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// Ball properties
float ballRadius = 0.05f;
float ballX = 0.0f;  // Initial X position of the ball
float ballY = 0.0f;  // Y position of the ball

// Movement speed and direction of the ball
float moveSpeedX = 0.01f;
float moveSpeedY = 0.01f;

// Bar properties
float leftBarY = 0.0f;  // Initial Y position of the left bar
float rightBarY = 0.0f;  // Initial Y position of the right bar
float barHeight = 0.6f;  // Height of the bars
float barWidth = 0.1f;   // Width of the bars

// Movement speed of the bars
float barMoveSpeed = 0.05f;

void drawBall(float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);  // Center of the ball
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(segments);
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawRectangle(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // Move left bar up and down
        if (key == GLFW_KEY_L) {
            if (mods & GLFW_MOD_SHIFT) {
                leftBarY += barMoveSpeed; // Move up
            } else {
                leftBarY -= barMoveSpeed; // Move down
            }
        }
            // Move right bar up and down
        else if (key == GLFW_KEY_R) {
            if (mods & GLFW_MOD_SHIFT) {
                rightBarY += barMoveSpeed; // Move up
            } else {
                rightBarY -= barMoveSpeed; // Move down
            }
        }
    }
}

bool checkCollision(float ballX, float ballY, float leftBarY, float rightBarY) {
    // Check collision with left bar
    if (ballX - ballRadius <= -0.9f + barWidth && ballY >= leftBarY - barHeight / 2 && ballY <= leftBarY + barHeight / 2)
        return true;
    // Check collision with right bar
    if (ballX + ballRadius >= 0.8f && ballY >= rightBarY - barHeight / 2 && ballY <= rightBarY + barHeight / 2)
        return true;
    return false;
}

void calculateReflectionAngle(float& moveSpeedX, float& moveSpeedY, float barY, float ballY) {
    // Calculate the angle of incidence
    float angle = (ballY - barY) / (barHeight / 2);
    // Reflect the ball based on the angle of incidence
    float speed = sqrt(moveSpeedX * moveSpeedX + moveSpeedY * moveSpeedY);
    float incidentAngle = atan2(moveSpeedY, moveSpeedX);
    float reflectionAngle = -incidentAngle + 2 * angle;
    moveSpeedX = speed * cos(reflectionAngle);
    moveSpeedY = speed * sin(reflectionAngle);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Moving Bars and Ball", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set the key callback function
    glfwSetKeyCallback(window, key_callback);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Set clear color to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw left bar
        glColor3f(0.0f, 1.0f, 0.0f);  // Green color
        drawRectangle(-0.9f, leftBarY - (barHeight / 2), barWidth, barHeight); // Draw the left bar

        // Draw right bar
        glColor3f(0.0f, 0.0f, 1.0f);  // Blue color
        drawRectangle(0.8f, rightBarY - (barHeight / 2), barWidth, barHeight); // Draw the right bar

        // Update ball position
        ballX += moveSpeedX;
        ballY += moveSpeedY;

        // Check collision with left bar
        if (ballX - ballRadius <= -0.9f + barWidth && checkCollision(ballX, ballY, leftBarY, rightBarY)) {
            calculateReflectionAngle(moveSpeedX, moveSpeedY, leftBarY, ballY);
        }

        // Check collision with right bar
        if (ballX + ballRadius >= 0.8f && checkCollision(ballX, ballY, leftBarY, rightBarY)) {
            calculateReflectionAngle(moveSpeedX, moveSpeedY, rightBarY, ballY);
        }

        // Check collision with top and bottom walls
        if (ballY + ballRadius >= 0.9f || ballY - ballRadius <= -0.9f) {
            moveSpeedY = -moveSpeedY;
        }

        // Draw the ball in red
        glColor3f(1.0f, 0.0f, 0.0f);  // Red color
        glPushMatrix();
        glTranslatef(ballX, ballY, 0.0f); // Translate to the current position of the ball
        drawBall(ballRadius, 30);  // Draw the ball
        glPopMatrix();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Terminate GLFW
    glfwTerminate();
    return 0;
}
