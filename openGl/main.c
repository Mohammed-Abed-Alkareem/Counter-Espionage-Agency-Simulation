#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

// Configuration Constants
#define MAX_CIRCLES 10  // Maximum number of circles in each box


#define MAX_KILLED_MEMBERS 50
#define MAX_INJURED_MEMBERS 30
#define AGENCY_INACTIVITY_LIMIT 20


// Static values for testing
int killed_members = 25;  // Current killed members
int injured_members = 15; // Current injured members
float inactive_time = 10.0f; // Current inactivity time (seconds)
float maxTime = 120.0f; // Maximum time (in seconds)
float elapsedTime = 0.0f; // Current elapsed time (in seconds)
float barHeight = 0.3f; // Height of the vertical bars


// Global Parameters
int rows = 3;              // Number of rows
int columns = 4;           // Number of columns
float startY = 0.0f;       // Starting Y position
float boxWidth = 0.4f;     // Width of each box
float boxHeight = 0.3f;    // Height of each box
float spacingX = 0.1f;     // Horizontal spacing between boxes
float spacingY = 0.1f;     // Vertical spacing between boxes

// Circle structure
typedef struct {
    float x, y;      // Position
    float dx, dy;    // Velocity
    float radius;    // Radius
} Circle;



// Circles for all boxes
Circle **circles;
float **boxColors; // Colors for each box

// Function prototypes
void renderText(float x, float y, const char *text, void *font, float r, float g, float b);
void drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b);
void drawCircle(float x, float y, float radius, float r, float g, float b);
void drawBoxWithCircles(float x, float y, Circle circles[], int numCircles, float r, float g, float b);
void updateCircles(Circle circles[], int numCircles, float boxX, float boxY);
void drawLoadingBar(float x, float y, float width, float height, float progress, const char *currentTimeText, const char *maxTimeText);
void drawBoxesWithCircles();
void updateAllCircles();
void initCirclesAndColors();
void display();
void timer();
void init();



// Function prototypes
void drawLoadingBar(float x, float y, float width, float height, float progress, const char *currentTimeText, const char *maxTimeText);
void drawVerticalBar(float x, float y, float width, float height, float progress, const char *currentText, const char *maxText, float r, float g, float b);


// Render text on the screen
void renderText(float x, float y, const char *text, void *font, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(font, *text);
        text++;
    }
}

// Draw a rectangle
void drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

// Draw a circle
void drawCircle(float x, float y, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 100; i++) {
        float angle = 2.0f * M_PI * i / 100;
        float dx = radius * cosf(angle);
        float dy = radius * sinf(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

// Draw a box containing moving circles
void drawBoxWithCircles(float x, float y, Circle circles[], int numCircles, float r, float g, float b) {
    drawRectangle(x - boxWidth / 2, y - boxHeight / 2, x + boxWidth / 2, y + boxHeight / 2, 0.3f, 0.3f, 0.3f); // Box background

    for (int i = 0; i < numCircles; i++) {
        drawCircle(circles[i].x, circles[i].y, circles[i].radius, r, g, b); // Circles
    }
}

// Update the positions of the circles
void updateCircles(Circle circles[], int numCircles, float boxX, float boxY) {
    for (int i = 0; i < numCircles; i++) {
        circles[i].x += circles[i].dx * 0.02f;
        circles[i].y += circles[i].dy * 0.02f;

        // Check for collisions with box boundaries
        if (circles[i].x - circles[i].radius < boxX - boxWidth / 2 || circles[i].x + circles[i].radius > boxX + boxWidth / 2) {
            circles[i].dx = -circles[i].dx;
            circles[i].x = fminf(fmaxf(circles[i].x, boxX - boxWidth / 2 + circles[i].radius), boxX + boxWidth / 2 - circles[i].radius);
        }
        if (circles[i].y - circles[i].radius < boxY - boxHeight / 2 || circles[i].y + circles[i].radius > boxY + boxHeight / 2) {
            circles[i].dy = -circles[i].dy;
            circles[i].y = fminf(fmaxf(circles[i].y, boxY - boxHeight / 2 + circles[i].radius), boxY + boxHeight / 2 - circles[i].radius);
        }
    }
}


// Draw a vertical progress bar
void drawVerticalBar(float x, float y, float width, float height, float progress, const char *currentText, const char *maxText, float r, float g, float b) {
    drawRectangle(x, y - height, x + width, y, 0.5f, 0.5f, 0.5f); // Background
    drawRectangle(x, y - height, x + width, y - height + height * progress, r, g, b); // Filled part

    if (currentText) {
        renderText(x + width / 2 - 0.05f, y - height / 2, currentText, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    }

    if (maxText) {
        renderText(x + width / 2 - 0.05f, y + 0.05f, maxText, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    }
}

// Draw a horizontal loading bar
void drawLoadingBar(float x, float y, float width, float height, float progress, const char *currentTimeText, const char *maxTimeText) {
    drawRectangle(x, y, x + width, y - height, 0.5f, 0.5f, 0.5f); // Background
    drawRectangle(x, y, x + width * progress, y - height, 0.0f, 0.0f, 1.0f); // Filled part

    if (currentTimeText) {
        renderText(x + width / 2 - 0.05f, y - height / 2, currentTimeText, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    }

    if (maxTimeText) {
        renderText(x + width + 0.05f, y - height / 2, maxTimeText, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    }
}

// Draw multiple boxes with moving circles
void drawBoxesWithCircles() {
    float startX = -1.0f + boxWidth / 2 + spacingX / 2;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            int boxIndex = i * columns + j;

            float x = startX + j * (boxWidth + spacingX);
            float y = startY - i * (boxHeight + spacingY);

            drawBoxWithCircles(x, y, circles[boxIndex], MAX_CIRCLES, boxColors[boxIndex][0], boxColors[boxIndex][1], boxColors[boxIndex][2]);
        }
    }
}

// Update all circles in all boxes
void updateAllCircles() {
    float startX = -1.0f + boxWidth / 2 + spacingX / 2;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            int boxIndex = i * columns + j;

            float x = startX + j * (boxWidth + spacingX);
            float y = startY - i * (boxHeight + spacingY);

            updateCircles(circles[boxIndex], MAX_CIRCLES, x, y);
        }
    }
}

// Initialization of all circles and colors
void initCirclesAndColors() {
    circles = malloc(rows * columns * sizeof(Circle *));
    boxColors = malloc(rows * columns * sizeof(float *));
    for (int i = 0; i < rows * columns; i++) {
        circles[i] = malloc(MAX_CIRCLES * sizeof(Circle));
        boxColors[i] = malloc(3 * sizeof(float));

        // Assign random colors to each box
        boxColors[i][0] = (rand() % 100) / 100.0f;
        boxColors[i][1] = (rand() % 100) / 100.0f;
        boxColors[i][2] = (rand() % 100) / 100.0f;

        for (int j = 0; j < MAX_CIRCLES; j++) {
            circles[i][j].x = -0.2f + (rand() % 100) / 100.0f * 0.4f;
            circles[i][j].y = 0.2f - (rand() % 100) / 100.0f * 0.3f;
            circles[i][j].dx = 0.5f * ((rand() % 2 == 0) ? 1 : -1);
            circles[i][j].dy = 0.5f * ((rand() % 2 == 0) ? 1 : -1);
            circles[i][j].radius = 0.02f;
        }
    }
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Timer bar
    float timeProgress = elapsedTime / maxTime;
    char currentTimeText[50];
    sprintf(currentTimeText, "%.1f s", elapsedTime);
    char maxTimeText[50];
    sprintf(maxTimeText, "Max: %.1f s", maxTime);
    drawLoadingBar(-0.9f, 0.9f, 1.0f, 0.1f, timeProgress, currentTimeText, maxTimeText);

    
 // Exit condition bars
    float barWidth = 0.25f;
    float spacing = 0.2f;
    float startX = -0.65f;
    float startY = 0.6f;

     // Killed members bar
    float killed_ratio = fminf((float)killed_members / MAX_KILLED_MEMBERS, 1.0f);
    char currentKilledText[50];
    sprintf(currentKilledText, "%d", killed_members);
    char maxKilledText[50];
    sprintf(maxKilledText, "Max: %d", MAX_KILLED_MEMBERS);
    drawVerticalBar(startX, startY, barWidth, barHeight, killed_ratio, currentKilledText, maxKilledText, 0.8f, 0.0f, 0.0f);
    renderText(startX + 0.02f, startY - barHeight - 0.1f, "Killed Members", GLUT_BITMAP_HELVETICA_18, 0.8f, 0.0f, 0.0f);

    // Injured members bar
    float injured_ratio = fminf((float)injured_members / MAX_INJURED_MEMBERS, 1.0f);
    char currentInjuredText[50];
    sprintf(currentInjuredText, "%d", injured_members);
    char maxInjuredText[50];
    sprintf(maxInjuredText, "Max: %d", MAX_INJURED_MEMBERS);
    drawVerticalBar(startX + barWidth + spacing, startY, barWidth, barHeight, injured_ratio, currentInjuredText, maxInjuredText, 0.9f, 0.6f, 0.0f);
    renderText(startX + barWidth + spacing + 0.02f, startY - barHeight - 0.1f, "Injured Members", GLUT_BITMAP_HELVETICA_18, 0.9f, 0.6f, 0.0f);

    // Agency inactivity bar
    float inactivity_ratio = fminf((float)inactive_time / AGENCY_INACTIVITY_LIMIT, 1.0f);
    char currentInactivityText[50];
    sprintf(currentInactivityText, "%.1f s", inactive_time);
    char maxInactivityText[50];
    sprintf(maxInactivityText, "Max: %d s", AGENCY_INACTIVITY_LIMIT);
    drawVerticalBar(startX + 2 * (barWidth + spacing), startY, barWidth, barHeight, inactivity_ratio, currentInactivityText, maxInactivityText, 0.0f, 0.4f, 0.9f);
    renderText(startX + 2 * (barWidth + spacing) + 0.02f, startY - barHeight - 0.1f, "Agency Inactivity", GLUT_BITMAP_HELVETICA_18, 0.0f, 0.4f, 0.9f);


    // Draw all boxes with circles
    drawBoxesWithCircles();

    glutSwapBuffers();
}

// Timer function
void timer() {
    elapsedTime += 0.1f;
    if (elapsedTime > maxTime) elapsedTime = maxTime;

    updateAllCircles(); // Update all circles

    glutPostRedisplay(); // Request redraw
    if (elapsedTime < maxTime) {
        glutTimerFunc(100, timer, 0); // Schedule next timer
    }
}

// Initialization
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    //seed with time and pid
    srand(time(NULL) + getpid());
    initCirclesAndColors();
}

// Main function
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Boxes with Moving Circles");

    init();

    glutDisplayFunc(display);
    glutTimerFunc(100, timer, 0);
    glutMainLoop();

    for (int i = 0; i < rows * columns; i++) {
        free(circles[i]);
        free(boxColors[i]);
    }
    free(circles);
    free(boxColors);

    return 0;
}
