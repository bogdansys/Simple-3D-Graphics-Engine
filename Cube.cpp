#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <chrono>

const int WIDTH = 40;
const int HEIGHT = 40;
const float PI = 3.14159265;

struct Point3D {
    float x, y, z;
};

struct Edge {
    Point3D start, end;
};

// Cube vertices and edges
std::vector<Point3D> cubeVertices = {
    {-1, -1, -1},
    {1, -1, -1},
    {1, 1, -1},
    {-1, 1, -1},
    {-1, -1, 1},
    {1, -1, 1},
    {1, 1, 1},
    {-1, 1, 1}
};

std::vector<Edge> cubeEdges = {
    {cubeVertices[0], cubeVertices[1]},
    {cubeVertices[1], cubeVertices[2]},
    {cubeVertices[2], cubeVertices[3]},
    {cubeVertices[3], cubeVertices[0]},
    {cubeVertices[4], cubeVertices[5]},
    {cubeVertices[5], cubeVertices[6]},
    {cubeVertices[6], cubeVertices[7]},
    {cubeVertices[7], cubeVertices[4]},
    {cubeVertices[0], cubeVertices[4]},
    {cubeVertices[1], cubeVertices[5]},
    {cubeVertices[2], cubeVertices[6]},
    {cubeVertices[3], cubeVertices[7]}
};

// Sphere functions
std::vector<Point3D> generateSpherePoints(float radius, int divisions) {
    std::vector<Point3D> points;
    for (int i = 0; i <= divisions; i++) {
        for (int j = 0; j <= divisions; j++) {
            float theta = i * (PI / divisions);
            float phi = j * (2 * PI / divisions);
            Point3D p = {
                radius * sin(theta) * cos(phi),
                radius * sin(theta) * sin(phi),
                radius * cos(theta)
            };
            points.push_back(p);
        }
    }
    return points;
}

float dotProduct(Point3D a, Point3D b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Point3D normalize(Point3D p) {
    float magnitude = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    return { p.x / magnitude, p.y / magnitude, p.z / magnitude };
}

Point3D rotateX(Point3D p, float angle) {
    return {
        p.x,
        p.y * cos(angle) - p.z * sin(angle),
        p.y * sin(angle) + p.z * cos(angle)
    };
}

Point3D rotateY(Point3D p, float angle) {
    return {
        p.x * cos(angle) + p.z * sin(angle),
        p.y,
        -p.x * sin(angle) + p.z * cos(angle)
    };
}

Point3D project(Point3D p) {
    return {
        p.x / (p.z + 4) * WIDTH + WIDTH / 2,
        -p.y / (p.z + 4) * HEIGHT + HEIGHT / 2,
        p.z
    };
}

void drawSphere(float angleX, float angleY) {
    char screen[WIDTH][HEIGHT];
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            screen[x][y] = ' ';
        }
    }

    Point3D lightDirection = { 1, 1, -1 }; // Example light direction
    lightDirection = normalize(lightDirection);

    auto points = generateSpherePoints(1.0, 20);
    for (Point3D p : points) {
        Point3D normal = normalize(p);
        float brightness = dotProduct(normal, lightDirection);

        p = rotateX(p, angleX);
        p = rotateY(p, angleY);
        p = project(p);

        int x = static_cast<int>(p.x);
        int y = static_cast<int>(p.y);

        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            if (brightness > 0) {
                screen[x][y] = '#';
            }
            else {
                screen[x][y] = '.';
            }
        }
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            std::cout << screen[x][y];
        }
        std::cout << std::endl;
    }
}

void drawCube(float angleX, float angleY) {
    char screen[WIDTH][HEIGHT];
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            screen[x][y] = ' ';
        }
    }

    for (Edge e : cubeEdges) {
        Point3D start = rotateX(e.start, angleX);
        start = rotateY(start, angleY);
        Point3D end = rotateX(e.end, angleX);
        end = rotateY(end, angleY);

        start = project(start);
        end = project(end);

        int x0 = static_cast<int>(start.x);
        int y0 = static_cast<int>(start.y);
        int x1 = static_cast<int>(end.x);
        int y1 = static_cast<int>(end.y);

        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        while (true) {
            if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT) {
                screen[x0][y0] = '#';
            }

            if (x0 == x1 && y0 == y1) break;

            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            std::cout << screen[x][y];
        }
        std::cout << std::endl;
    }
}

// Cylinder functions
std::vector<Point3D> generateCylinderPoints(float radius, float height, int divisions) {
    std::vector<Point3D> points;
    for (int i = 0; i <= divisions; i++) {
        for (int j = 0; j <= divisions; j++) {
            float theta = j * (2 * PI / divisions);
            float y = i * (height / divisions) - height / 2;
            Point3D p = {
                radius * cos(theta),
                y,
                radius * sin(theta)
            };
            points.push_back(p);
        }
    }
    return points;
}

// Cone functions
std::vector<Point3D> generateConePoints(float radius, float height, int divisions) {
    std::vector<Point3D> points;
    for (int i = 0; i <= divisions; i++) {
        for (int j = 0; j <= divisions; j++) {
            float theta = j * (2 * PI / divisions);
            float y = i * (height / divisions) - height / 2;
            float r = radius * (1 - i / static_cast<float>(divisions));
            Point3D p = {
                r * cos(theta),
                y,
                r * sin(theta)
            };
            points.push_back(p);
        }
    }
    return points;
}

void drawShape(const std::vector<Point3D>& points, float angleX, float angleY) {
    char screen[WIDTH][HEIGHT];
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            screen[x][y] = ' ';
        }
    }

    Point3D lightDirection = { 1, 1, -1 }; // Example light direction
    lightDirection = normalize(lightDirection);

    for (Point3D p : points) {
        Point3D normal = normalize(p);
        float brightness = dotProduct(normal, lightDirection);

        p = rotateX(p, angleX);
        p = rotateY(p, angleY);
        p = project(p);

        int x = static_cast<int>(p.x);
        int y = static_cast<int>(p.y);

        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            if (brightness > 0) {
                screen[x][y] = '#';
            }
            else {
                screen[x][y] = '.';
            }
        }
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            std::cout << screen[x][y];
        }
        std::cout << std::endl;
    }
}

std::vector<Point3D> generateTorusPoints(float R, float r, int divisions) { // the donut
    std::vector<Point3D> points;
    for (int i = 0; i <= divisions; i++) {
        for (int j = 0; j <= divisions; j++) {
            float theta = i * (2 * PI / divisions);
            float phi = j * (2 * PI / divisions);
            Point3D p = {
                (R + r * cos(theta)) * cos(phi),
                r * sin(theta),
                (R + r * cos(theta)) * sin(phi)
            };
            points.push_back(p);
        }
    }
    return points;
}


// Ellipsoid functions
std::vector<Point3D> generateEllipsoidPoints(float a, float b, float c, int divisions) {
    std::vector<Point3D> points;
    for (int i = 0; i <= divisions; i++) {
        for (int j = 0; j <= divisions; j++) {
            float theta = i * (PI / divisions);
            float phi = j * (2 * PI / divisions);
            Point3D p = {
                a * sin(theta) * cos(phi),
                b * sin(theta) * sin(phi),
                c * cos(theta)
            };
            points.push_back(p);
        }
    }
    return points;
}

// Prism functions (using a hexagonal base as an example)
std::vector<Point3D> generatePrismPoints(float radius, float height) {
    std::vector<Point3D> points;
    int sides = 6; // Hexagon
    for (int i = 0; i < sides; i++) {
        float angle = i * (2 * PI / sides);
        points.push_back({ radius * cos(angle), -height / 2, radius * sin(angle) });
        points.push_back({ radius * cos(angle), height / 2, radius * sin(angle) });
    }
    return points;
}

// Octahedron functions
std::vector<Point3D> generateOctahedronPoints(float size) {
    std::vector<Point3D> points = {
        {0, size, 0},
        {0, -size, 0},
        {size, 0, 0},
        {-size, 0, 0},
        {0, 0, size},
        {0, 0, -size}
    };
    return points;
}

// Snake functions
std::vector<Point3D> generateSnakePoints(float segmentLength, int segments) {
    std::vector<Point3D> points;
    for (int i = 0; i < segments; i++) {
        Point3D p = {
            0,
            segmentLength * i,
            0
        };
        points.push_back(p);
    }
    return points;
}

void clearScreen(char screen[WIDTH][HEIGHT]) {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            screen[x][y] = ' ';
        }
    }
}

void displayScreen(char screen[WIDTH][HEIGHT]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            std::cout << screen[x][y];
        }
        std::cout << std::endl;
    }
}

int main() {
    float angleX = 0;
    float angleY = 0;

    int choice = 0;
    while (choice < 1 || choice > 9) {
        std::cout << "Choose a shape to display:" << std::endl;
        std::cout << "1. Cube" << std::endl;
        std::cout << "2. Sphere" << std::endl;
        std::cout << "3. Cylinder" << std::endl;
        std::cout << "4. Cone" << std::endl;
        std::cout << "5. Torus" << std::endl;
        std::cout << "6. Ellipsoid" << std::endl;
        std::cout << "7. Snake" << std::endl;
        std::cin >> choice;
    }

    while (true) {

        if (choice == 1) {
            drawCube(angleX, angleY);
        }
        else if (choice == 2) {
            drawShape(generateSpherePoints(1.0, 20), angleX, angleY);
        }
        else if (choice == 3) {
            drawShape(generateCylinderPoints(1.0, 2.0, 20), angleX, angleY);
        }
        else if (choice == 4) {
            drawShape(generateConePoints(1.0, 2.0, 20), angleX, angleY);
        }
        else if (choice == 5) {
            drawShape(generateTorusPoints(1.2, 0.5, 20), angleX, angleY);
        }
        else if (choice == 6) {
            drawShape(generateEllipsoidPoints(1.5, 1.0, 0.5, 20), angleX, angleY);
        }
        else if (choice == 7) {
            drawShape(generateSnakePoints(0.5, 10), angleX, angleY);
        } 
			
        angleX += 0.05;
        angleY += 0.05;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::system("cls");
    }

    return 0;
}