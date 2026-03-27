#include <GL/glut.h>
#include <math.h>
#include <iostream>
using namespace std;

int ww = 600, wh = 400;
int xi, yi, xf, yf;

// Clipping window
const float xmin = 100, xmax = 500, ymin = 100, ymax = 500;

void putPixel(int x, int y)
{
    glBegin(GL_POINTS);
	if((x-300)^2+(x-300)^2-100^2<0){
		glColor3f(1, 0, 0);
		glVertex2i(x, y);
	}
    glVertex2i(x, y);
    glEnd();
}

void plotCircle(int h, int k, int x, int y)
{
	putPixel((x+h), (y+k)); //x,y
	putPixel((y+h), (x+k));//y, x
	putPixel((-y+h), (x+k));//-y, x
	putPixel((-x+h), (y+k));//-x, y
	putPixel((-x+h), (-y+k));//-x,-y
	putPixel((-y+h), (-x+k));//-y,-x
	putPixel((y+h), (-x+k));//y,-x
	putPixel((x+h), (-y+k));//x,-y

}

void midpointCircle(int h, int k, int r)
{
	int x=0, y=r, d= 1-r;
	while(x <= y)
	{
		plotCircle(h, k, x, y);
		if(d<0)
		{
			d = d + 2*x + 3; 
		}
		else
		{
			d = d + 2*(x-y) + 5;
			y--;
		}
		x++;
	}
}

// Bresenham Line Drawing
void bresenhamAlg(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int x = x0, y = y0;

    int sx = (x1 > x0) ? 1 : -1;
    int sy = (y1 > y0) ? 1 : -1;

    bool isSteep = dy > dx;
    if (isSteep) {
        swap(dx, dy);
    }

    int d = 2 * dy - dx;

    for (int i = 0; i <= dx; i++)
    {
        putPixel(x, y);

        while (d >= 0)
        {
            if (isSteep) x += sx;
            else y += sy;
            d -= 2 * dx;
        }

        if (isSteep) y += sy;
        else x += sx;

        d += 2 * dy;
    }
}

// Liang-Barsky Clipping
bool liangBarsky(float x0, float y0, float x1, float y1,
                 float left, float right, float bottom, float top,
                 float &xc0, float &yc0,
                 float &xc1, float &yc1)
{
    float t0 = 0.0f, t1 = 1.0f;
    float dx = x1 - x0, dy = y1 - y0;

    float p[4] = {-dx, dx, -dy, dy};
    float q[4] = {x0 - left, right - x0, y0 - bottom, top - y0};

    for (int i = 0; i < 4; i++)
    {
        if (p[i] == 0)
        {
            if (q[i] < 0) return false;
        }
        else
        {
            float r = q[i] / p[i];

            if (p[i] < 0)
                t0 = max(t0, r);
            else
                t1 = min(t1, r);

            if (t0 > t1) return false;
        }
    }

    xc0 = x0 + t0 * dx;
    yc0 = y0 + t0 * dy;
    xc1 = x0 + t1 * dx;
    yc1 = y0 + t1 * dy;

    return true;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw clipping window
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(xmin, ymin);
    glVertex2f(xmax, ymin);
    glVertex2f(xmax, ymax);
    glVertex2f(xmin, ymax);
    glEnd();

	glColor3f(0, 1, 0);
	midpointCircle(300, 300, 100);

    glFlush();
}

void mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        xi = x;
        yi = wh - y;
    }
    else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        xf = x;
        yf = wh - y;

        float xc0, yc0, xc1, yc1;

        // Apply Liang-Barsky
        if (liangBarsky(xi, yi, xf, yf,
                        xmin, xmax, ymin, ymax,
                        xc0, yc0, xc1, yc1))
        {
            // Draw clipped line (Blue)
            glColor3f(0, 0, 1);
            bresenhamAlg((int)xc0, (int)yc0, (int)xc1, (int)yc1);
        }

        glFlush();
    }

    if (btn == GLUT_RIGHT_BUTTON)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        display();
    }
}

void myinit()
{
    glClearColor(0.8, 0.9, 0.9, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, ww, 0, wh);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(ww, wh);
    glutCreateWindow("Liang-Barsky Line Clipping");

    myinit();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}