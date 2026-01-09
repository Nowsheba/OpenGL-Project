include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <ctime>


float cloudX = -1.2f;
float cloudSpeed = 0.0022f;

float boatX = 1.3f;
float boatSpeed = 0.0022f;

// Rain variables
float rainX[500];
float rainY[500];
float rainSpeed[500];
const int NUM_RAINDROPS = 500;

// Snow variables
float snowX[300];
float snowY[300];
float snowSpeed[300];
const int NUM_SNOWFLAKES = 300;

bool showLightning = false;
float lightningTimer = 0.0f;
const float PI = 3.1416f;
float turbineAngle = 0.0f;
int season=0;
void initGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    srand(time(0));
    // Initialize rain
    for (int i = 0; i < NUM_RAINDROPS; i++) {
        rainX[i] = (rand() % 2000) / 1000.0f - 1.0f;
        rainY[i] = (rand() % 2000) / 1000.0f;
        rainSpeed[i] = 0.01f + (rand() % 100) / 5000.0#f;
    }
    // Initialize snow
    for (int i = 0; i < NUM_SNOWFLAKES; i++) {
        snowX[i] = (rand() % 2000) / 1000.0f - 1.0f;
        snowY[i] = (rand() % 2000) / 1000.0f;
        snowSpeed[i] = 0.003f + (rand() % 100) / 8000.0f;
    }


}
void circle(float cx, float cy, float r) {
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 360; i++) {
        float theta = i * PI / 180.0f;
        float x = cx + r * cos(theta);
        float y = cy + r * sin(theta);
        glVertex2f(x, y);
    }
    glEnd();
}

void quad(float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4) {
    glBegin(GL_QUADS);
    glVertex2f(x1,y1);
    glVertex2f(x2,y2);
    glVertex2f(x3,y3);
    glVertex2f(x4,y4);
    glEnd();
}

void triangle(float x1,float y1,float x2,float y2,float x3,float y3) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x1,y1);
    glVertex2f(x2,y2);
    glVertex2f(x3,y3);
    glEnd();
}
//  Bezier curve
void bezierCurve(float x0, float y0, float x1, float y1,
                 float x2, float y2, float x3, float y3) {
    glBegin(GL_LINE_STRIP);
    for (float t = 0.0f; t <= 1.0f; t += 0.01f) {
        float xt = pow(1-t,3)*x0 + 3*t*pow(1-t,2)*x1 + 3*pow(t,2)*(1-t)*x2 + pow(t,3)*x3;
        float yt = pow(1-t,3)*y0 + 3*t*pow(1-t,2)*y1 + 3*pow(t,2)*(1-t)*y2 + pow(t,3)*y3;
        glVertex2f(xt, yt);
    }
    glEnd();
}

const int GRID = 1000;

int toGridX(float x) { return (int)lroundf((x + 1.0f) * (GRID / 2.0f)); }
int toGridY(float y) { return (int)lroundf((y + 1.0f) * (GRID / 2.0f)); }
float toWorldX(int x) { return (x / (GRID / 2.0f)) - 1.0f; }
float toWorldY(int y) { return (y / (GRID / 2.0f)) - 1.0f; }

void plotGridPoint(int gx, int gy) {
    glVertex2f(toWorldX(gx), toWorldY(gy));
}

// DDA Line
void lineDDA(float x0, float y0, float x1, float y1) {
    int xStart = toGridX(x0);
    int yStart = toGridY(y0);
    int xEnd   = toGridX(x1);
    int yEnd   = toGridY(y1);

    int dx = xEnd - xStart;
    int dy = yEnd - yStart;

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    if (steps == 0) steps = 1;

    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;

    float x = (float)xStart;
    float y = (float)yStart;

    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++) {
        plotGridPoint((int)lroundf(x), (int)lroundf(y));
        x += xInc;
        y += yInc;
    }
    glEnd();
}

void drawLineMidpoint(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    int d = 2*dy - dx;
    int incrE = 2*dy;
    int incrNE = 2*(dy - dx);

    int x = x1;
    int y = y1;

    glBegin(GL_POINTS);
    glVertex2i(x, y);

   while (x < x2) {
        if (d <= 0) {
            d += incrE;
            x++;
        } else {
            d += incrNE;
            x++;
            y++;
        }
        glVertex2i(x, y);
    }
    glEnd();
}

// midpointline
void midpointLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    if (dy <= dx) {
        if (x1 > x2) {
            drawLineMidpoint(x2, y2, x1, y1);
        } else {
            drawLineMidpoint(x1, y1, x2, y2);
        }
    } else {
        if (y1 > y2) {
            drawLineMidpoint(y2, x2, y1, x1);
        } else {
            drawLineMidpoint(y1, x1, y2, x2);
        }
    }
}


//Scene
void summersky() {
    glColor3f(0.55f, 0.85f, 1.0f);
    quad(-1,0, 1,0, 1,1, -1,1);
}
void nightsky() {
    glColor3f(0.0f, 0.0f, 0.0f);
    quad(-1,0, 1,0, 1,1, -1,1);
}
void rainysky()
{
    if (showLightning) {
        glColor3f(0.95f, 0.95f, 0.8f);
    } else {
        glColor3f(0.3f, 0.35f, 0.4f);
    }
    quad(-1,0, 1,0, 1,1, -1,1);
}

void wintersky()
{
    glColor3f(0.87f, 0.92f, 0.98f);
    quad(-1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f);
}
void stars() {
    glColor3f(1.0f, 1.0f, 1.0f);
    circle(-0.8f, 0.7f, 0.01f);
    circle(-0.5f, 0.85f, 0.012f);
    circle(-0.2f, 0.95f, 0.01f);
    circle(0.2f, 0.88f, 0.013f);
    circle(0.5f, 0.92f, 0.012f);
    circle(0.8f, 0.86f, 0.01f);
}
void groundsummer() {
    glColor3f(0.40f, 0.80f, 0.25f);
    quad(-1,-1, 1,-1, 1,0, -1,0);
}
void groundlateautumn() {
    glColor3f(0.55f, 0.48f, 0.32f);
    quad(-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f);
}

void groundnight() {
    glColor3f(0.05f, 0.15f, 0.05f);
    quad(-1,-1, 1,-1, 1,0, -1,0);
}
void groundrain() {
    glColor3f(0.25f, 0.60f, 0.20f);
    quad(-1,-1, 1,-1, 1,0, -1,0);
}
void groundwinter() {
    glColor3f(0.95f, 0.95f, 0.95f);
    quad(-1,-1, 1,-1, 1,0, -1,0);
}
void riversummer() {
    glColor3f(0.20f, 0.60f, 0.85f);
    quad(-1,-0.10f, 1,-0.10f, 1,-0.28f, -1,-0.28f);
}

void rivernight() {
    glColor3f(0.05f, 0.15f, 0.25f);
    quad(-1,-0.10f, 1,-0.10f, 1,-0.28f, -1,-0.28f);
}
void riverrain() {
    glColor3f(0.15f, 0.45f, 0.65f);
    quad(-1,-0.10f, 1,-0.10f, 1,-0.28f, -1,-0.28f);
}
void riverwinter() {
     glColor3f(0.35f, 0.55f, 0.75f);
         quad(-1,-0.10f, 1,-0.10f, 1,-0.28f, -1,-0.28f);

            glColor4f(0.85f, 0.92f, 0.98f, 0.6f);
            quad(-1,-0.12f, 1,-0.12f, 1,-0.18f, -1,-0.18f);
            return;
    }

void sun() {
    glColor3f(1.0f, 0.9f, 0.1f);
    circle(0.65f, 0.75f, 0.08f);
     }
void moon() {
    glColor3f(0.95f, 0.95f, 0.85f);
    circle(0.65f, 0.75f, 0.08f);
}
// Turbine
void turbine(float x, float y, float s, float r, float g, float b) {
    // tower
    glColor3f(1.0f, 1.0f, 1.0f);
    quad(x - 0.010f*s, y,          x + 0.010f*s, y,
         x + 0.006f*s, y + 0.35f*s, x - 0.006f*s, y + 0.35f*s);

    // hub
    glColor3f(1.f, 1.0f, 1.0f);
    circle(x, y + 0.35f*s, 0.018f*s);

    // blades
    glColor3f(1.0f, 1.0f, 1.0f);

    for (int i = 0; i < 3; i++) {
        float angle = turbineAngle + i * 120.0f;
        float rad = angle * PI / 180.0f;

        float bx1 = x;
        float by1 = y + 0.35f*s;

        float bx2 = x + cos(rad) * (0.18f*s) - sin(rad) * (0.02f*s);
        float by2 = y + 0.35f*s + sin(rad) * (0.18f*s) + cos(rad) * (0.02f*s);

        float bx3 = x + cos(rad) * (0.18f*s) - sin(rad) * (-0.02f*s);
        float by3 = y + 0.35f*s + sin(rad) * (0.18f*s) + cos(rad) * (-0.02f*s);

        triangle(bx1, by1, bx2, by2, bx3, by3);
    }
}


void cloud(float x, float y) {
    glColor3f(0.95f, 0.95f, 0.95f);
    circle(x, y, 0.07f);
    circle(x+0.08f, y+0.02f, 0.06f);
    circle(x-0.08f, y+0.02f, 0.06f);
    circle(x+0.14f, y, 0.05f);
}
void cloudrain(float x, float y) {
    glColor3f(0.6f, 0.6f, 0.7f);
    circle(x, y, 0.07f);
    circle(x+0.08f, y+0.02f, 0.06f);
    circle(x-0.08f, y+0.02f, 0.06f);
    circle(x+0.14f, y, 0.05f);
}
void cloudwinter(float x, float y) {
    glColor3f(0.85f, 0.92f, 0.98f);
    circle(x, y, 0.07f);
    circle(x+0.08f, y+0.02f, 0.06f);
    circle(x-0.08f, y+0.02f, 0.06f);
    circle(x+0.14f, y, 0.05f);
}
void boat(float x) {
    glColor3f(0.55f, 0.30f, 0.15f);
    quad(-0.25f+x, -0.18f, 0.25f+x, -0.18f, 0.18f+x, -0.26f, -0.18f+x, -0.26f);


    // DDA used
    glPointSize(2.0f);
    glColor3f(0.30f, 0.20f, 0.10f);

    //  mast
    lineDDA(-0.02f+x, -0.18f, -0.02f+x, 0.05f);

    // rope
    lineDDA(-0.20f+x, -0.18f, -0.02f+x, 0.05f);

    glColor3f(0.95f, 0.20f, 0.15f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.02f+x, 0.05f);
    glVertex2f(-0.02f+x, -0.17f);
    glVertex2f(0.15f+x, -0.17f);
    glEnd();
}

void boatwinter(float x) {
            // Boat stuck in ice for winter
            glColor3f(0.45f, 0.35f, 0.25f);
            quad(-0.25f + x, -0.18f, 0.25f + x, -0.18f, 0.18f + x, -0.26f, -0.18f + x, -0.26f);
            glColor3f(0.95f, 0.95f, 0.95f);
            quad(-0.20f + x, -0.16f, 0.20f + x, -0.16f, 0.15f + x, -0.18f, -0.15f + x, -0.18f);
            glColor4f(0.85f, 0.92f, 0.98f, 0.7f);
            circle(x, -0.22f, 0.12f);

}

void house() {
    glColor3f(0.92f, 0.92f, 0.92f);
    quad(0.35f,-0.13f, 0.72f,-0.13f, 0.72f,-0.35f, 0.35f,-0.35f);

    glColor3f(0.70f, 0.10f, 0.10f);
    quad(0.33f,-0.35f, 0.74f,-0.35f, 0.74f,-0.39f, 0.33f,-0.39f);

    glColor3f(0.90f, 0.15f, 0.15f);
    quad(0.32f,-0.13f, 0.75f,-0.13f, 0.70f,0.02f, 0.37f,0.02f);

    glColor3f(0.95f, 0.50f, 0.10f);
    quad(0.505f,-0.20f, 0.575f,-0.20f, 0.575f,-0.35f, 0.505f,-0.35f);

    glColor3f(0.95f, 0.55f, 0.10f);
    quad(0.39f,-0.20f, 0.47f,-0.20f, 0.47f,-0.27f, 0.39f,-0.27f);
    quad(0.61f,-0.20f, 0.69f,-0.20f, 0.69f,-0.27f, 0.61f,-0.27f);
}
void housewinter() {
                glColor3f(0.82f, 0.82f, 0.82f);
            quad(0.35f, -0.13f, 0.72f, -0.13f, 0.72f, -0.35f, 0.35f, -0.35f);
            glColor3f(0.60f, 0.60f, 0.60f);
            quad(0.33f, -0.35f, 0.74f, -0.35f, 0.74f, -0.39f, 0.33f, -0.39f);
            glColor3f(0.75f, 0.15f, 0.15f);
            quad(0.32f, -0.13f, 0.75f, -0.13f, 0.70f, 0.02f, 0.37f, 0.02f);
            glColor3f(0.95f, 0.95f, 0.95f);
            quad(0.33f, -0.10f, 0.74f, -0.10f, 0.69f, 0.00f, 0.38f, 0.00f);
            glColor3f(0.65f, 0.45f, 0.25f);
            quad(0.505f, -0.20f, 0.575f, -0.20f, 0.575f, -0.35f, 0.505f, -0.35f);
            glColor3f(0.9f, 0.7f, 0.3f);
            circle(0.56f, -0.275f, 0.008f);
            glColor3f(0.75f, 0.85f, 0.95f);
            quad(0.39f, -0.20f, 0.47f, -0.20f, 0.47f, -0.27f, 0.39f, -0.27f);
            quad(0.61f, -0.20f, 0.69f, -0.20f, 0.69f, -0.27f, 0.61f, -0.27f);

    // Midpoint
        glPointSize(2.0f);
        glColor3f(0.4f, 0.3f, 0.2f);

    // left window
        midpointLine(0.43f, -0.20f, 0.43f, -0.27f);

    // right window
        midpointLine(0.65f, -0.20f, 0.65f, -0.27f);

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.39f, -0.235f); glVertex2f(0.47f, -0.235f);
    glVertex2f(0.61f, -0.235f); glVertex2f(0.69f, -0.235f);
    glEnd();

}
void hill() {
    glBegin(GL_POLYGON);
    glColor3ub(91, 131, 109);
    glVertex2f(-1.0f, 0.0f);
    glVertex2f(-0.2f, 0.0f);
    glVertex2f(-0.2f, 0.05f);
    glVertex2f(-0.4f, 0.10f);
    glVertex2f(-0.6f, 0.15f);
    glVertex2f(-0.8f, 0.20f);
    glVertex2f(-1.0f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3ub(91, 131, 109);
    glVertex2f(-0.2f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(1.0f, 0.25f);
    glVertex2f(0.8f, 0.20f);
    glVertex2f(0.6f, 0.15f);
    glVertex2f(0.4f, 0.10f);
    glVertex2f(0.2f, 0.05f);
    glVertex2f(-0.2f, 0.05f);
    glEnd();
}
void hillwinter() {
    //  (Bezier curves)
    glColor3ub(91, 131, 109);

    // left curve
    bezierCurve(-1.0f, 0.25f, -0.75f, 0.22f, -0.45f, 0.12f, 0.0f, 0.05f);

    // right curve
    bezierCurve(0.0f, 0.05f, 0.35f, 0.08f, 0.70f, 0.18f, 1.0f, 0.25f);

    // Snow overlay
    glColor3f(0.95f, 0.95f, 0.95f);

    // left curve
    bezierCurve(-1.0f, 0.20f, -0.8f, 0.18f, -0.6f, 0.13f, -0.2f, 0.03f);

    // right  curve
    bezierCurve(-0.2f, 0.05f, 0.4f, 0.08f, 0.8f, 0.20f, 1.0f, 0.25f);
}




void hilllateautumn() {
    glColor3ub(110, 100, 90);

    glBegin(GL_POLYGON);
        glVertex2f(-1.0f, 0.0f);
        glVertex2f(-0.2f, 0.0f);
        glVertex2f(-0.2f, 0.05f);
        glVertex2f(-0.4f, 0.10f);
        glVertex2f(-0.6f, 0.15f);
        glVertex2f(-0.8f, 0.20f);
        glVertex2f(-1.0f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
        glColor3ub(100, 90, 80);
        glVertex2f(-0.2f, 0.0f);
        glVertex2f(1.0f, 0.0f);
        glVertex2f(1.0f, 0.25f);
        glVertex2f(0.8f, 0.20f);
        glVertex2f(0.6f, 0.15f);
        glVertex2f(0.4f, 0.10f);
        glVertex2f(0.2f, 0.05f);
        glVertex2f(-0.2f, 0.05f);
    glEnd();
}

void tree() {
    // trunk
    glColor3f(0.28f, 0.14f, 0.06f);
    quad(-0.9f,-0.70f, -0.85f,-0.70f, -0.85f,-0.40f, -0.9f,-0.40f);

    // leaves
    glColor3f(0.06f, 0.42f, 0.14f);
    circle(-0.875f, -0.30f, 0.14f);

    glColor3f(0.08f, 0.52f, 0.16f);
    circle(-0.92f, -0.34f, 0.12f);

    glColor3f(0.10f, 0.55f, 0.15f);
    circle(-0.83f, -0.34f, 0.12f);

    glColor3f(0.15f, 0.60f, 0.18f);
    circle(-0.87f, -0.25f, 0.11f);

    glColor3f(0.20f, 0.65f, 0.20f);
    circle(-0.88f, -0.20f, 0.10f);
}

void tree1() {
    // trunk
    glColor3f(0.28f, 0.14f, 0.06f);
    quad(0.80f,-0.39f, 0.85f,-0.39f, 0.85f,-0.15f, 0.80f,-0.15f);

    // leaves
    glColor3f(0.06f, 0.42f, 0.14f);
    circle(0.825f, -0.05f, 0.12f);
    glColor3f(0.08f, 0.52f, 0.16f);
    circle(0.78f, -0.08f, 0.11f);
    glColor3f(0.10f, 0.55f, 0.15f);
    circle(0.87f, -0.08f, 0.11f);
    glColor3f(0.15f, 0.60f, 0.18f);
    circle(0.82f, 0.02f, 0.10f);
    glColor3f(0.20f, 0.65f, 0.20f);
    circle(0.84f, 0.06f, 0.09f);
}
void treeWinter() {
    // trunk
    glColor3f(0.28f, 0.14f, 0.06f);
    quad(-0.9f,-0.70f, -0.85f,-0.70f, -0.85f,-0.40f, -0.9f,-0.40f);


    glColor3f(0.15f, 0.35f, 0.15f);
    triangle(-0.92f, -0.20f, -1.15f, -0.40f, -0.69f, -0.40f);
    triangle(-0.92f, -0.10f, -1.10f, -0.25f, -0.74f, -0.25f);
    triangle(-0.92f,  0.00f, -1.05f, -0.10f, -0.79f, -0.10f);

    // snow
    glColor3f(0.95f, 0.95f, 0.95f);
    triangle(-0.92f,  0.05f, -1.02f, -0.02f, -0.82f, -0.02f);
    triangle(-0.92f, -0.05f, -1.07f, -0.18f, -0.77f, -0.18f);
    triangle(-0.92f, -0.15f, -1.12f, -0.33f, -0.72f, -0.33f);
}

void tree1Winter() {
    // trunk
    glColor3f(0.28f, 0.14f, 0.06f);
    quad(0.80f,-0.39f, 0.85f,-0.39f, 0.85f,-0.15f, 0.80f,-0.15f);


    glColor3f(0.15f, 0.35f, 0.15f);
    triangle(0.82f, -0.05f, 0.65f, -0.25f, 0.99f, -0.25f);
    triangle(0.82f,  0.05f, 0.68f, -0.10f, 0.96f, -0.10f);
    triangle(0.82f,  0.15f, 0.71f,  0.00f, 0.93f,  0.00f);

    // snow
    glColor3f(0.95f, 0.95f, 0.95f);
    triangle(0.82f,  0.20f, 0.73f,  0.08f, 0.91f,  0.08f);
    triangle(0.82f,  0.10f, 0.70f, -0.05f, 0.94f, -0.05f);
    triangle(0.82f,  0.00f, 0.67f, -0.15f, 0.97f, -0.15f);
}
void lateautumntree() {
    // Trunk
    glColor3f(0.20f, 0.10f, 0.05f);
    quad(-0.9f, -0.70f, -0.85f, -0.70f, -0.85f, -0.40f, -0.9f, -0.40f);

    // Leaves
    glColor3f(0.45f, 0.15f, 0.05f);
    circle(-0.875f, -0.35f, 0.10f);

    glColor3f(0.55f, 0.30f, 0.10f);
    circle(-0.92f, -0.38f, 0.08f);

    glColor3f(0.60f, 0.40f, 0.15f);
    circle(-0.83f, -0.38f, 0.08f);

    glColor3f(0.50f, 0.35f, 0.10f);
    circle(-0.87f, -0.28f, 0.07f);

    glColor3f(0.40f, 0.25f, 0.05f);
    circle(-0.88f, -0.24f, 0.06f);
}

void lateautumntree1() {
    // Trunk
    glColor3f(0.20f, 0.10f, 0.05f);
    quad(0.80f, -0.39f, 0.85f, -0.39f, 0.85f, -0.15f, 0.80f, -0.15f);

    // Leaves
    glColor3f(0.35f, 0.10f, 0.05f);
    circle(0.825f, -0.05f, 0.09f);

    glColor3f(0.50f, 0.20f, 0.05f);
    circle(0.78f, -0.08f, 0.08f);

    glColor3f(0.55f, 0.25f, 0.10f);
    circle(0.87f, -0.08f, 0.08f);

    glColor3f(0.45f, 0.30f, 0.15f);
    circle(0.82f, 0.01f, 0.07f);

    glColor3f(0.30f, 0.20f, 0.10f);
    circle(0.84f, 0.04f, 0.06f);
}
void road() {
    glColor3f(0.65f, 0.45f, 0.25f);
    quad(-1.0f,-0.70f, 1.0f,-0.70f, 1.0f,-0.60f, -1.0f,-0.60f);
}

void fence() {
    glColor3f(0.55f, 0.35f, 0.20f);
    for (float x = 0.30f; x <= 0.75f; x += 0.07f) {
        quad(x, -0.55f, x+0.02f, -0.55f, x+0.02f, -0.42f, x, -0.42f);
    }
    quad(0.30f, -0.45f, 0.75f, -0.45f, 0.75f, -0.47f, 0.30f, -0.47f);
    quad(0.30f, -0.52f, 0.75f, -0.52f, 0.75f, -0.54f, 0.30f, -0.54f);
}
void roadWinter() {
    // base road
    glColor3f(0.65f, 0.45f, 0.25f);
    quad(-1.0f,-0.70f, 1.0f,-0.70f, 1.0f,-0.60f, -1.0f,-0.60f);

    glColor3f(0.95f, 0.95f, 0.95f);
    quad(-1.0f,-0.70f, 1.0f,-0.70f, 1.0f,-0.63f, -1.0f,-0.63f);
}

void fenceWinter() {
    glColor3f(0.55f, 0.35f, 0.20f);
    for (float x = 0.30f; x <= 0.75f; x += 0.07f) {
        quad(x, -0.55f, x+0.02f, -0.55f, x+0.02f, -0.42f, x, -0.42f);
    }
    quad(0.30f, -0.45f, 0.75f, -0.45f, 0.75f, -0.47f, 0.30f, -0.47f);
    quad(0.30f, -0.52f, 0.75f, -0.52f, 0.75f, -0.54f, 0.30f, -0.54f);

    glColor3f(0.95f, 0.95f, 0.95f);
    quad(0.30f, -0.44f, 0.75f, -0.44f, 0.75f, -0.42f, 0.30f, -0.42f);
    quad(0.30f, -0.51f, 0.75f, -0.51f, 0.75f, -0.49f, 0.30f, -0.49f);

    for (float x = 0.30f; x <= 0.75f; x += 0.07f) {
        glColor3f(0.95f, 0.95f, 0.95f);
        quad(x, -0.42f, x+0.02f, -0.42f, x+0.02f, -0.40f, x, -0.40f);
    }
}

void lateautumnfence() {
    glColor3f(0.40f, 0.38f, 0.35f);

    for (float x = 0.30f; x <= 0.75f; x += 0.07f) {
        quad(x, -0.55f, x+0.02f, -0.55f, x+0.02f, -0.42f, x, -0.42f);
    }

    quad(0.30f, -0.45f, 0.75f, -0.45f, 0.75f, -0.47f, 0.30f, -0.47f);
    quad(0.30f, -0.52f, 0.75f, -0.52f, 0.75f, -0.54f, 0.30f, -0.54f);
}

void ricefield() {
    glColor3f(0.45f, 0.40f, 0.25f);
    quad(0.3f, -1.0f,  1.0f, -1.0f,  1.0f, -0.70f,  0.3f, -0.70f);

    // plants
    glColor3f(0.8f, 0.7f, 0.2f);
    for (float x = 0.35f; x < 0.95f; x += 0.08f) {
        for (float y = -0.95f; y < -0.72f; y += 0.08f) {
            glBegin(GL_LINES);
                glVertex2f(x, y);
                glVertex2f(x, y + 0.04f);
                glVertex2f(x, y + 0.04f);
                glVertex2f(x + 0.01f, y + 0.03f);
            glEnd();
        }
    }
}
void kansgrass() {
    for (float x = -0.77f; x < 0.35f; x += 0.05f) {
        if (x > -0.95f && x < -0.80f) continue;

        // Stalk
        glColor3f(0.3f, 0.6f, 0.2f);
        glBegin(GL_LINES);
            glVertex2f(x, -0.28f);
            glVertex2f(x, -0.18f);
        glEnd();

        // White
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x, -0.18f);
            glVertex2f(x - 0.015f, -0.12f);
            glVertex2f(x + 0.015f, -0.12f);
        glEnd();
    }
    for (float x = 0.88f; x < 1.0f; x += 0.05f) {
        // Stalk
        glColor3f(0.3f, 0.6f, 0.2f);
        glBegin(GL_LINES);
            glVertex2f(x, -0.28f);
            glVertex2f(x, -0.18f);
        glEnd();

        // White
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x, -0.18f);
            glVertex2f(x - 0.015f, -0.12f);
            glVertex2f(x + 0.015f, -0.12f);
        glEnd();
    }
    for (float x = 0.4f; x < 0.95f; x += 0.07f) {
        // Stalk
        glColor3f(0.3f, 0.6f, 0.2f);
        glBegin(GL_LINES);
            glVertex2f(x, -0.95f); glVertex2f(x, -0.82f);
        glEnd();
        // White
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x, -0.82f); glVertex2f(x-0.02f, -0.75f); glVertex2f(x+0.02f, -0.75f);
        glEnd();
    }
}
void flowers() {
    for (float x = -0.98f; x < -0.75f; x += 0.06f) {
        float y = -0.72f;
        glColor3f(0.2f, 0.7f, 0.2f);
        glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x, y + 0.03f);
        glEnd();
        glColor3f(0.6f, 0.4f, 0.9f); // Purple
        circle(x, y + 0.04f, 0.015f);
        glColor3f(1.0f, 1.0f, 1.0f);
         circle(x, y + 0.04f, 0.005f);
    }

    for (float x = 0.35f; x < 0.72f; x += 0.07f) {
        float y = -0.45f;

        // Stalk
        glColor3f(0.2f, 0.7f, 0.2f);
        glBegin(GL_LINES);
         glVertex2f(x, y);
         glVertex2f(x, y + 0.03f);
          glEnd();

        glColor3f(1.0f, 0.6f, 0.0f);
        circle(x, y + 0.04f, 0.015f);

        // Center Dot
        glColor3f(1.0f, 1.0f, 1.0f); circle(x, y + 0.04f, 0.005f);
    }
    for (float x = 0.35f; x < 0.95f; x += 0.08f) {
        for (float y = -0.98f; y < -0.75f; y += 0.08f) {
            glColor3f(0.2f, 0.7f, 0.2f);
            glBegin(GL_LINES);
            glVertex2f(x, y);
            glVertex2f(x, y + 0.03f);
            glEnd();
            if (x < 0.55f)      glColor3f(1.0f, 0.2f, 0.2f); // Red
            else if (x < 0.75f) glColor3f(1.0f, 1.0f, 0.0f); // Yellow
            else                glColor3f(1.0f, 0.5f, 0.8f); // Pink
            circle(x, y + 0.04f, 0.02f);
            glColor3f(1.0f, 1.0f, 1.0f);
            circle(x, y + 0.04f, 0.005f);
        }
    }
}
void butterfly(float x, float y) {
    // Wings
    glColor3f(0.0f, 0.7f, 1.0f);
    glBegin(GL_TRIANGLES);
        // Left Wing
        glVertex2f(x, y);
        glVertex2f(x - 0.04f, y + 0.03f);
        glVertex2f(x - 0.04f, y - 0.03f);
        // Right Wing
        glVertex2f(x, y);
        glVertex2f(x + 0.04f, y + 0.03f);
        glVertex2f(x + 0.04f, y - 0.03f);
    glEnd();

    // Body
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex2f(x, y - 0.03f);
        glVertex2f(x, y + 0.03f);
    glEnd();
}
void cow() {
    // Body )
    glColor3f(0.9f, 0.9f, 0.9f);
    circle(-0.73f, -0.53f, 0.045f); // Front
    circle(-0.68f, -0.53f, 0.045f); // Back

    //  Head
    circle(-0.79f, -0.50f, 0.028f);

    //  Tail
    glColor3f(0.8f, 0.8f, 0.8f);
    quad(-0.64f, -0.52f, -0.63f, -0.52f, -0.63f, -0.57f, -0.64f, -0.57f);
    glColor3f(0.2f, 0.2f, 0.2f);
    circle(-0.635f, -0.58f, 0.008f);

    //  Legs
    glColor3f(0.2f, 0.2f, 0.2f);
    // Front Leg
    quad(-0.75f, -0.56f, -0.73f, -0.56f, -0.73f, -0.61f, -0.75f, -0.61f);
    // Back Leg
    quad(-0.69f, -0.56f, -0.67f, -0.56f, -0.67f, -0.61f, -0.69f, -0.61f);

    // 5. Eye
    glColor3f(0.0f, 0.0f, 0.0f);
    circle(-0.805f, -0.50f, 0.006f);
}
void boy() {
    // body
    glColor3f(0.2f, 0.4f, 0.8f);
    quad(-0.60f, -0.45f, -0.55f, -0.45f, -0.55f, -0.55f, -0.60f, -0.55f);

    // head
    glColor3f(1.0f, 0.8f, 0.6f);
    circle(-0.575f, -0.44f, 0.03f);

    // legs
    glColor3f(0.1f, 0.1f, 0.1f);
    quad(-0.60f, -0.55f, -0.58f, -0.55f, -0.58f, -0.62f, -0.60f, -0.62f);
    quad(-0.57f, -0.55f, -0.55f, -0.55f, -0.55f, -0.62f, -0.57f, -0.62f);

    // arms
    glColor3f(0.2f, 0.4f, 0.8f);
    quad(-0.62f, -0.47f, -0.60f, -0.47f, -0.60f, -0.52f, -0.62f, -0.52f); // left arm
    quad(-0.55f, -0.47f, -0.53f, -0.47f, -0.53f, -0.52f, -0.55f, -0.52f); // right arm

    //rope
    glLineWidth(2);
    glColor3f(0.3f, 0.2f, 0.1f);
    glBegin(GL_LINES);
    glVertex2f(-0.53f, -0.50f);
    glVertex2f(-0.76f, -0.54f);
    glEnd();
}
void boyRain() {
    // body
    glColor3f(0.2f, 0.4f, 0.8f);
    quad(-0.60f, -0.45f, -0.55f, -0.45f, -0.55f, -0.55f, -0.60f, -0.55f);

    // head
    glColor3f(1.0f, 0.8f, 0.6f);
    circle(-0.575f, -0.44f, 0.03f);

    // legs
    glColor3f(0.1f, 0.1f, 0.1f);
    quad(-0.60f, -0.55f, -0.58f, -0.55f, -0.58f, -0.62f, -0.60f, -0.62f);
    quad(-0.57f, -0.55f, -0.55f, -0.55f, -0.55f, -0.62f, -0.57f, -0.62f);


    glColor3f(0.2f, 0.4f, 0.8f);
    quad(-0.62f, -0.47f, -0.60f, -0.47f, -0.60f, -0.52f, -0.62f, -0.52f);
    quad(-0.55f, -0.47f, -0.53f, -0.47f, -0.53f, -0.52f, -0.55f, -0.52f);

    glLineWidth(2);
    glColor3f(0.3f, 0.2f, 0.1f);
    glBegin(GL_LINES);
    glVertex2f(-0.53f, -0.50f);
    glVertex2f(-0.76f, -0.54f);
    glEnd();


    glColor3f(0.8f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-0.575f, -0.32f);

    for (int i = 0; i <= 180; i += 10) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(-0.575f + 0.12f * cos(rad), -0.32f + 0.08f * sin(rad));
    }
    glEnd();

    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(-0.575f, -0.32f);
    glVertex2f(-0.575f, -0.47f);
    glEnd();


    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(3);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 180; i += 15) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(-0.62f + 0.05f * cos(rad), -0.47f - 0.05f * sin(rad));
    }
    glEnd();
}

// Rain effects
void drawRain() {
    glLineWidth(1.5f);
    glColor3f(0.7f, 0.8f, 1.0f);

    for (int i = 0; i < NUM_RAINDROPS; i++) {
        glBegin(GL_LINES);
        glVertex2f(rainX[i], rainY[i]);
        glVertex2f(rainX[i] - 0.02f, rainY[i] - 0.05f);
        glEnd();
    }
}

void updateRain() {
    for (int i = 0; i < NUM_RAINDROPS; i++) {
        rainY[i] -= rainSpeed[i];
        if (rainY[i] < -1.0f) {
            rainY[i] = 1.0f + (rand() % 100) / 1000.0f;
            rainX[i] = (rand() % 2000) / 1000.0f - 1.0f;
        }
    }
}

void drawLightning() {
    if (!showLightning) return;

    glLineWidth(3.0f);
    glColor3f(1.0f, 1.0f, 0.8f);

    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 0.8f);
    glVertex2f(-0.1f, 0.6f);
    glVertex2f(0.05f, 0.5f);
    glVertex2f(-0.05f, 0.3f);
    glVertex2f(0.1f, 0.2f);
    glVertex2f(0.0f, 0.0f);
    glEnd();
}

// Snow
void drawSnow() {
    for (int i = 0; i < NUM_SNOWFLAKES; i++) {
        glPointSize(2.5f + (rand() % 100) / 100.0f);
        glColor3f(0.95f, 0.95f, 0.98f);

        glBegin(GL_POINTS);
        glVertex2f(snowX[i], snowY[i]);
        glEnd();

        if (i % 5 == 0) {
            glLineWidth(1.0f);
            glBegin(GL_LINES);
            glVertex2f(snowX[i] - 0.005f, snowY[i]); glVertex2f(snowX[i] + 0.005f, snowY[i]);
            glVertex2f(snowX[i], snowY[i] - 0.005f); glVertex2f(snowX[i], snowY[i] + 0.005f);
            glVertex2f(snowX[i] - 0.0035f, snowY[i] - 0.0035f); glVertex2f(snowX[i] + 0.0035f, snowY[i] + 0.0035f);
            glVertex2f(snowX[i] - 0.0035f, snowY[i] + 0.0035f); glVertex2f(snowX[i] + 0.0035f, snowY[i] - 0.0035f);
            glEnd();
        }
    }
}

void updateSnow() {
    for (int i = 0; i < NUM_SNOWFLAKES; i++) {
        snowY[i] -= snowSpeed[i];
        snowX[i] += (rand() % 100 - 50) / 50000.0f;

        if (snowY[i] < -1.0f) {
            snowY[i] = 1.0f + (rand() % 100) / 1000.0f;
            snowX[i] = (rand() % 2000) / 1000.0f - 1.0f;
        }
    }
}

void drawSnowPiles() {
    glColor3f(0.95f, 0.95f, 0.98f);
    circle(-0.90f, -0.35f, 0.08f);
    circle(-0.82f, -0.38f, 0.06f);
    circle(-0.98f, -0.38f, 0.07f);
    circle(0.54f, -0.35f, 0.05f);
    circle(0.48f, -0.38f, 0.04f);
    circle(0.60f, -0.38f, 0.04f);
    quad(-1.0f, -0.10f, -0.95f, -0.10f, -0.95f, -0.14f, -1.0f, -0.14f);
    quad(0.95f, -0.10f, 1.0f, -0.10f, 1.0f, -0.14f, 0.95f, -0.14f);
}
void drawTurbinesLeft() {
    turbine(-0.78f, -0.08f, 0.55f, 0.95f, 0.35f, 0.20f);
    turbine(-0.62f, -0.05f, 0.45f, 0.20f, 0.75f, 0.95f);
    turbine(-0.48f, -0.04f, 0.35f, 0.80f, 0.30f, 0.90f);
}



void summer() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    summersky();
    hill();
    sun();
    cloud(cloudX-0.2f, 0.75f);
    cloud(cloudX+0.4f, 0.68f);
    groundsummer();
    riversummer();
    boat(boatX);
    tree();
    house();
    tree1();
    road();
    fence();
    boy();
    cow();
    drawTurbinesLeft();

    glFlush();
}
void night() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    nightsky();
    hill();
    moon();
    stars();
    cloud(cloudX-0.2f, 0.75f);
    cloud(cloudX+0.4f, 0.68f);
    groundnight();
    rivernight();
    boat(boatX);
    tree();
    house();
    tree1();
    road();
    fence();
    drawTurbinesLeft();

    glFlush();
}
void rain() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    rainysky();
    hill();
    cloudrain(cloudX - 0.2f, 0.75f);
    cloudrain(cloudX + 0.4f, 0.68f);
    groundrain();
    drawLightning();
    riversummer();
    boat(boatX);
    drawRain();
    tree();
    house();
    tree1();
    road();
    fence();
    boyRain();
    cow();
    drawTurbinesLeft();

    glFlush();
}
void winter() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    wintersky();
    hillwinter();
    moon();
    cloud(cloudX-0.2f, 0.75f);
    cloud(cloudX+0.4f, 0.68f);
    cloudwinter(cloudX - 0.2f, 0.75f);
    cloudwinter(cloudX + 0.4f, 0.68f);
    groundwinter();
    drawSnowPiles();
    riverwinter();
    boatwinter(boatX);
    drawSnow();
    treeWinter();
    housewinter();
    tree1Winter();
    roadWinter();
    fenceWinter();
    drawTurbinesLeft();

    glFlush();
}

void LateAutumn() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    summersky();
    hilllateautumn();
    sun();
    cloud(cloudX-0.2f, 0.75f);
    cloud(cloudX+0.4f, 0.68f);
    groundlateautumn();
    riversummer();
    boat(boatX);
    lateautumntree();
    house();
    lateautumntree1();
    road();
    lateautumnfence();
    boy();
    cow();
    ricefield();
    drawTurbinesLeft();

    glFlush();
}
void autumn() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    summersky();
    hill();
    sun();
    cloud(cloudX-0.6f, 0.85f);
    cloud(cloudX+0.5f, 0.75f);
    cloud(cloudX+0.1f, 0.90f);

    cloud(cloudX-0.1f, 0.55f);
    cloud(cloudX-0.8f, 0.45f);
    cloud(cloudX+0.7f, 0.40f);

    cloud(cloudX-0.4f, 0.30f);
    cloud(cloudX+0.3f, 0.25f);
    groundsummer();
    riversummer();
    boat(boatX);
    tree();
    house();
    tree1();
    road();
    fence();
    boy();
    cow();
    kansgrass();
    drawTurbinesLeft();

    glFlush();
}
void spring() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    summersky();
    hill();
    sun();
    cloud(cloudX-0.2f, 0.75f);
    cloud(cloudX+0.4f, 0.68f);
    cloud(cloudX-0.4f, 0.30f);
    cloud(cloudX+0.3f, 0.25f);
    groundsummer();
    riversummer();
    boat(boatX);
    tree();
    house();
    tree1();
    road();
    fence();
    boy();
    cow();
    flowers();
    butterfly(0.6f, -0.72f);
    butterfly(0.8f, -0.75f);
    drawTurbinesLeft();

    glFlush();
}
// Timer
void timer(int value)
{
    // Move cloud
    cloudX += cloudSpeed;
    if (cloudX > 1.6f) cloudX = -1.6f;

    // turbine rotate
    turbineAngle += 2.0f;
    if (turbineAngle > 360.0f) turbineAngle -= 360.0f;



    if (season == 3) { // Winter
        boatX -= boatSpeed * 0.5f;
    } else {
        boatX -= boatSpeed;
    }
    if (boatX < -1.6f) boatX = 1.3f;

    if (season == 2) { // Rainy
        // Update rain
        for (int i = 0; i < NUM_RAINDROPS; i++) {
            rainY[i] -= rainSpeed[i];
            if (rainY[i] < -1.0f) {
                rainY[i] = 1.0f;
                rainX[i] = (rand() % 2000) / 1000.0f - 1.0f;
            }
        }

        // Lightning effect
        lightningTimer += 0.016f;
        if (lightningTimer > 3.0f) showLightning = true;
        if (lightningTimer > 3.1f) {
            showLightning = false;
            lightningTimer = 0.0f;
        }
    }
    else if (season == 3) { // Winter
        for (int i = 0; i < NUM_SNOWFLAKES; i++) {
            snowY[i] -= snowSpeed[i];
            if (snowY[i] < -1.0f) {
                snowY[i] = 1.0f;
                snowX[i] = (rand() % 2000) / 1000.0f - 1.0f;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}
//keyboard
void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case '1':
            season = 0; // summer
            break;
        case '2':
            season = 1; // Night
            break;
         case '3':
              season = 2; //rain
          break;
        case '4':
             season = 3; //winter
         break;
          case '5':
             season = 4; //lateautumn
         break;
          case '6':
             season = 5; //autumn
         break;
         case '7':
             season = 6; // spring
         break;



    }
    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

   switch(season) {
        case 0:
            summer();
            break;
        case 1:
            night();
            break;
       case 2:
           rain();
            break;
        case 3:
             winter();
             break;
        case 4:
             LateAutumn();
             break;
        case 5:
             autumn();
             break;
        case 6:
             spring();
             break;
    }



    glFlush();
}

// Main
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 700);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Season Change and night mode");

    initGL();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
