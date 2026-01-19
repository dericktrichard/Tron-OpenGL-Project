#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <string>
#include <ctime>

struct Point { float x, z; };

// Game Constants
const float ARENA_SIZE = 50.0f;
const float SPEED = 0.25f; // Slightly faster for excitement
const int COLLISION_SENSITIVITY = 25; // How many points back to ignore

// Game State
enum State { PLAYING, WON, LOST };
State gameState = PLAYING;

// Entities
float pX = 0.0f, pZ = 20.0f;
int pDir = 0; // 0:N, 1:E, 2:S, 3:W
std::vector<Point> pTrail;

float aiX = 0.0f, aiZ = -20.0f;
int aiDir = 2; 
std::vector<Point> aiTrail;

void init() {
    glClearColor(0.0f, 0.05f, 0.1f, 1.0f); // Darker tech-blue background
    glEnable(GL_DEPTH_TEST);
    srand(time(NULL));
}

void drawText(float x, float y, std::string text) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(x, y);
    for (char c : text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawGrid() {
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.3f, 0.6f);
    for (int i = -ARENA_SIZE; i <= ARENA_SIZE; i += 2) {
        glVertex3f(i, 0, -ARENA_SIZE); glVertex3f(i, 0, ARENA_SIZE);
        glVertex3f(-ARENA_SIZE, 0, i); glVertex3f(ARENA_SIZE, 0, i);
    }
    glEnd();
}

void drawTrail(const std::vector<Point>& trail, float r, float g, float b) {
    if (trail.size() < 2) return;
    glBegin(GL_QUAD_STRIP);
    for (const auto& p : trail) {
        glColor3f(r*0.3, g*0.3, b*0.3); glVertex3f(p.x, 0.0f, p.z);
        glColor3f(r, g, b);           glVertex3f(p.x, 1.2f, p.z); // Slightly taller walls
    }
    glEnd();
}

bool isColliding(float x, float z, const std::vector<Point>& trail, bool self) {
    // Wall collision
    if (abs(x) >= ARENA_SIZE || abs(z) >= ARENA_SIZE) return true;
    
    // Trail collision
    if (trail.size() < COLLISION_SENSITIVITY) return false;
    
    // If checking self, skip the most recent points so we don't hit our own tail
    size_t limit = self ? trail.size() - COLLISION_SENSITIVITY : trail.size();
    for (size_t i = 0; i < limit; i++) {
        if (sqrt(pow(x - trail[i].x, 2) + pow(z - trail[i].z, 2)) < 0.6f) return true;
    }
    return false;
}

void aiDecision() {
    float nextX = aiX, nextZ = aiZ;
    if (aiDir == 0) nextZ -= SPEED * 5;
    else if (aiDir == 1) nextX += SPEED * 5;
    else if (aiDir == 2) nextZ += SPEED * 5;
    else if (aiDir == 3) nextX -= SPEED * 5;

    // If AI is about to hit something, try to turn
    if (isColliding(nextX, nextZ, aiTrail, true) || isColliding(nextX, nextZ, pTrail, false)) {
        int originalDir = aiDir;
        aiDir = (rand() % 2 == 0) ? (aiDir + 1) % 4 : (aiDir + 3) % 4;
    }
}

void update(int value) {
    if (gameState == PLAYING) {
        pTrail.push_back({pX, pZ});
        aiTrail.push_back({aiX, aiZ});

        // Move Player
        if (pDir == 0) pZ -= SPEED; else if (pDir == 1) pX += SPEED;
        else if (pDir == 2) pZ += SPEED; else if (pDir == 3) pX -= SPEED;

        // Move AI
        aiDecision();
        if (aiDir == 0) aiZ -= SPEED; else if (aiDir == 1) aiX += SPEED;
        else if (aiDir == 2) aiZ += SPEED; else if (aiDir == 3) aiX -= SPEED;

        // Check Collisions
        if (isColliding(pX, pZ, pTrail, true) || isColliding(pX, pZ, aiTrail, false)) gameState = LOST;
        if (isColliding(aiX, aiZ, aiTrail, true) || isColliding(aiX, aiZ, pTrail, false)) {
            if (gameState == LOST) gameState = LOST; // Mutual crash
            else gameState = WON;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Smoother Camera
    gluLookAt(pX, 15.0f, pZ + 25.0f, pX, 0.0f, pZ, 0.0f, 1.0f, 0.0f);

    drawGrid();
    drawTrail(pTrail, 0.0f, 1.0f, 1.0f); // Cyan
    drawTrail(aiTrail, 1.0f, 0.2f, 0.2f); // Red

    // Draw Bikes
    glPushMatrix();
    glTranslatef(pX, 0.5f, pZ); glColor3f(0, 1, 1); glutSolidCube(0.8f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(aiX, 0.5f, aiZ); glColor3f(1, 0, 0); glutSolidCube(0.8f);
    glPopMatrix();

    // UI Overlay
    if (gameState == LOST) {
        glColor3f(1, 0, 0);
        drawText(330, 320, "DE-REZZED: AI WINS");
        drawText(310, 280, "Press 'R' to Respawn");
    } else if (gameState == WON) {
        glColor3f(0, 1, 0);
        drawText(350, 320, "SYSTEM CLEARED: YOU WIN!");
        drawText(310, 280, "Press 'R' to Restart");
    }

    glutSwapBuffers();
}

void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': if (pDir != 2) pDir = 0; break;
        case 'd': if (pDir != 3) pDir = 1; break;
        case 's': if (pDir != 0) pDir = 2; break;
        case 'a': if (pDir != 1) pDir = 3; break;
        case 'r': 
            pX = 0; pZ = 20; pDir = 0; pTrail.clear();
            aiX = 0; aiZ = -20; aiDir = 2; aiTrail.clear();
            gameState = PLAYING;
            break;
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w/h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutCreateWindow("TRON 3D: High-Speed Battle");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0);
    glutKeyboardFunc(handleKeypress);
    glutMainLoop();
    return 0;
}