#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <stack>
#include <utility>

#define WIDTH 800
#define HEIGHT 600
#define LINE_LENGTH 40
#define M_PI 3.14159265358979323846

sf::Vector2u windowSize = { WIDTH, HEIGHT };
#define NUM_LINES 50
sf::Vector2u origin = { 10, windowSize.y / 2 };

float angleToRadian(float angle) {
    return angle * M_PI / 180.0f;
}

std::vector<Line> lines;
bool sorting = false;
int left = 0, right = -1, pivotIndex = -1, i = 0, j = 0;
enum class State { PARTITION, SWAP, RECURSE };
State state = State::PARTITION;
std::stack<std::pair<int, int>> stack;

struct Line {
    float angle;
    sf::Vertex vertices[2];
    Line(float a) : angle(a) {
        vertices[0] = sf::Vertex(sf::Vector2f(origin.x, origin.y), sf::Color::White);
        vertices[1] = sf::Vertex(
            sf::Vector2f(origin.x + cos(angleToRadian(angle)) * LINE_LENGTH,
                origin.y - sin(angleToRadian(angle)) * LINE_LENGTH),
            sf::Color::White
        );
    }
    friend void swap(Line& a, Line& b) noexcept {
        std::swap(a.angle, b.angle);
        std::swap(a.vertices[0], b.vertices[0]);
        std::swap(a.vertices[1], b.vertices[1]);
    }
};

void swap(Line l1, Line l2) {
    
}

void generateLines() {
    lines.reserve(NUM_LINES);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(60.0f, 120.0f);
    for (int k = 0; k < NUM_LINES; ++k) {
        lines[k] = Line(dist(gen));
    }
}

void quicksort(int leftBound, int rightBound) {
    if (leftBound < rightBound) {
        left = leftBound;
        right = rightBound;
        pivotIndex = rightBound;
        i = left - 1;
        j = left;
        state = State::PARTITION;
    }
    else {
        sorting = false;
    }
}

void updateSort() {
    if (!sorting) return;

    if (state == State::PARTITION) {
        if (j < right) {
            if (lines[j].angle <= lines[pivotIndex].angle) {
                ++i;
                swap(lines[i], lines[j]);
            }
            ++j;
        }
        else {
            ++i;
            swap(lines[i], lines[pivotIndex]);
            state = State::SWAP;
        }
    }
    else if (state == State::SWAP) {
        state = State::RECURSE;
    }
    else if (state == State::RECURSE) {
        if (i - 1 - left > right - i) {
            if (i + 1 < right) stack.push({ i + 1, right });
            if (left < i - 1) quicksort(left, i - 1);
            else if (!stack.empty()) {
                std::pair<int, int> bounds = stack.top();
                stack.pop();
                quicksort(bounds.first, bounds.second);
            }
            else {
                sorting = false;
            }
        }
        else {
            if (left < i - 1) stack.push({ left, i - 1 });
            if (i + 1 < right) quicksort(i + 1, right);
            else if (!stack.empty()) {
                std::pair<int, int> bounds = stack.top();
                stack.pop();
                quicksort(bounds.first, bounds.second);
            }
            else {
                sorting = false;
            }
        }
    }
}

void drawLines(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    for (int k = 0; k < NUM_LINES; ++k) {
        if (sorting) {
            if (k == pivotIndex) {
                lines[k].vertices[0].color = lines[k].vertices[1].color = sf::Color::Red;
            }
            else if (k == i || k == j) {
                lines[k].vertices[0].color = lines[k].vertices[1].color = sf::Color::Green;
            }
            else {
                lines[k].vertices[0].color = lines[k].vertices[1].color = sf::Color::White;
            }
        }
        window.draw(lines[k].vertices, 2, sf::Lines);
    }
    window.display();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "QUICKSORT");
    window.setFramerateLimit(60);
    sf::View view(sf::FloatRect(0, 0, WIDTH, HEIGHT));
    window.setView(view);

    generateLines();
    sf::Clock clock;
    float updateInterval = 0.05f; // Update every 50ms( this is the whole reason we have divided our sorting algo to 3 stages)

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                if (!sorting) {
                    stack = std::stack<std::pair<int, int>>(); // Clear stack
                    generateLines(); 
                    sorting = true;
                    quicksort(0, NUM_LINES - 1);
                }
            }
        }

        if (sorting && clock.getElapsedTime().asSeconds() >= updateInterval) {
            updateSort();
            clock.restart();
        }

        drawLines(window);
    }

    return 0;
}