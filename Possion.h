#pragma once
#include <iostream>
#include <random>
#include <vector>
#include <SFML/Graphics.hpp>
#include <cmath>

#define WIDTH 800   
#define HEIGHT 600  
#define RADIUS 5.0f 
#define MAX_ATTEMPTS 20
#define M_PI 3.14159265358979323846

struct Point {
    float x, y;
    bool active;
};

std::vector<Point> poissonDiscSampling(float radius, int k, sf::Vector2u windowSize) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist_X(0, windowSize.x);
    std::uniform_real_distribution<float> dist_Y(0, windowSize.y);

    float cellSize = radius / std::sqrt(2.0f);
    int gridWidth = static_cast<int>(std::ceil(windowSize.x / cellSize));
    int gridHeight = static_cast<int>(std::ceil(windowSize.y / cellSize));
    std::vector<std::vector<int>> grid(gridWidth, std::vector<int>(gridHeight, -1)); // -1 means empty

    std::vector<Point> points;
    std::vector<Point> active;

    // Start with one random point
    Point first = { dist_X(gen), dist_Y(gen), true };
    points.push_back(first);
    active.push_back(first);
    int firstGridX = static_cast<int>(first.x / cellSize);
    int firstGridY = static_cast<int>(first.y / cellSize);
    grid[firstGridX][firstGridY] = 0; // Index of first point

    while (!active.empty()) {
        int idx = std::uniform_int_distribution<int>(0, active.size() - 1)(gen);
        Point& p = active[idx];
        bool found = false;

        for (int i = 0; i < k; ++i) {
            float angle = std::uniform_real_distribution<float>(0, 2 * M_PI)(gen);
            float dist = std::uniform_real_distribution<float>(radius, 2 * radius)(gen);
            float newX = p.x + dist * std::cos(angle*1.61803)*M_PI;
            float newY = p.y + dist * std::sin(angle* 1.61803)*M_PI;

            if (newX < 0 || newX >= windowSize.x || newY < 0 || newY >= windowSize.y) {
                continue;
            }

            int gridX = static_cast<int>(newX / cellSize);
            int gridY = static_cast<int>(newY / cellSize);
            bool valid = true;

            for (int dx = -2; dx <= 2 && valid; ++dx) {
                for (int dy = -2; dy <= 2; ++dy) {
                    int checkX = gridX + dx;
                    int checkY = gridY + dy;
                    if (checkX >= 0 && checkX < gridWidth && checkY >= 0 && checkY < gridHeight) {
                        int pointIdx = grid[checkX][checkY];
                        if (pointIdx != -1) {
                            const Point& existing = points[pointIdx];
                            float dx = existing.x - newX;
                            float dy = existing.y - newY;
                            if (dx * dx + dy * dy < radius * radius) {
                                valid = false;
                                break;
                            }
                        }
                    }
                }
            }

            if (valid) {
                Point newPoint = { newX, newY, true };
                points.push_back(newPoint);
                active.push_back(newPoint);
                grid[gridX][gridY] = points.size() - 1; 
                found = true;
                break;
            }
        }

        if (!found) {
            p.active = false;
            active.erase(active.begin() + idx);
        }
    }

    return points;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Poisson Disc Sampling");
    window.setFramerateLimit(60);

    
    std::vector<Point> points = poissonDiscSampling(RADIUS, MAX_ATTEMPTS, { WIDTH, HEIGHT });
    std::cout << "Generated " << points.size() << " points\n"; 

    std::vector<sf::CircleShape> circles;
    for (const auto& p : points) {
        sf::CircleShape circle(1.0f);
        circle.setPosition(p.x - 1.0f, p.y - 1.0f);
        circle.setFillColor(sf::Color::White);
        circles.push_back(circle);
    }

    size_t currentPoint = 0;
    sf::Clock clock;
    float spawnInterval = 0.01f; 

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (clock.getElapsedTime().asSeconds() > spawnInterval && currentPoint < points.size()) {
            currentPoint++;
            clock.restart();
        }

        window.clear(sf::Color::Black);
        for (size_t i = 0; i < currentPoint && i < circles.size(); ++i) {
            window.draw(circles[i]);
        }
        window.display();
    }

    return 0;
}