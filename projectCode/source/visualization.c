#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <shared_memory.h>
// Shared data pointer
SharedData *shared_data = NULL;

// Visualization Parameters
float maxTime = 20.0f;
float barHeight = 0.5f;

void renderText(float x, float y, const char *text, void *font, float r, float g, float b);
void drawVerticalBar(float x, float y, float width, float height, float progress, const char *currentText, const char *maxText, float r, float g, float b);
void drawVisualization();
void timer();

// Render text on the screen
void renderText(float x, float y, const char *text, void *font, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(font, *text);
        text++;
    }
}

// Draw a vertical progress bar
void drawVerticalBar(float x, float y, float width, float height, float progress, const char *currentText, const char *maxText, float r, float g, float b) {
    glColor3f(0.5f, 0.5f, 0.5f); // Background
    glBegin(GL_QUADS);
    glVertex2f(x, y - height);
    glVertex2f(x + width, y - height);
    glVertex2f(x + width, y);
    glVertex2f(x, y);
    glEnd();

    glColor3f(r, g, b); // Filled part
    glBegin(GL_QUADS);
    glVertex2f(x, y - height);
    glVertex2f(x + width, y - height);
    glVertex2f(x + width, y - height + height * progress);
    glVertex2f(x, y - height + height * progress);
    glEnd();

    renderText(x + width + 0.02f, y - height + height * progress, currentText, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    renderText(x + width + 0.02f, y, maxText, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
}

// Draw the visualization
void drawVisualization() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Killed Members Bar
    float killedProgress = (float)shared_data->number_killed_members / 50.0f;
    char currentKilled[20], maxKilled[20];
    sprintf(currentKilled, "%d", shared_data->number_killed_members);
    sprintf(maxKilled, "Max: 50");
    drawVerticalBar(-0.8f, 0.8f, 0.1f, 0.5f, killedProgress, currentKilled, maxKilled, 1.0f, 0.0f, 0.0f);

    // Injured Members Bar
    float injuredProgress = (float)shared_data->number_injured_members / 30.0f;
    char currentInjured[20], maxInjured[20];
    sprintf(currentInjured, "%d", shared_data->number_injured_members);
    sprintf(maxInjured, "Max: 30");
    drawVerticalBar(-0.6f, 0.8f, 0.1f, 0.5f, injuredProgress, currentInjured, maxInjured, 1.0f, 1.0f, 0.0f);

    // Inactivity Time Bar
    float inactiveProgress = shared_data->agency_inactivity_time / maxTime;
    char currentInactive[20], maxInactive[20];
    sprintf(currentInactive, "%.1f", shared_data->agency_inactivity_time);
    sprintf(maxInactive, "Max: 20.0s");
    drawVerticalBar(-0.4f, 0.8f, 0.1f, 0.5f, inactiveProgress, currentInactive, maxInactive, 0.0f, 0.0f, 1.0f);

    glutSwapBuffers();
}

// Timer function
void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(100, timer, 0);
}

// Initialization
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <shm_key>\n", argv[0]);
        exit(1);
    }
    // Attach to shared memory
    key_t shm_key = atoi(argv[1]);
    int shm_id = shmget(shm_key, sizeof(SharedData), 0666);
    if (shm_id == -1) {
        perror("Failed to attach to shared memory");
        exit(1);
    }

    shared_data = (SharedData *)shmat(shm_id, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("Failed to map shared memory");
        exit(1);
    }

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Dynamic Visualization");

    init();
    glutDisplayFunc(drawVisualization);
    glutTimerFunc(100, timer, 0);
    glutMainLoop();

    return 0;
}
