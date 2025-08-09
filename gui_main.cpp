// main.cpp
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

constexpr int WINDOW_WIDTH = 1000;
constexpr int WINDOW_HEIGHT = 700;

// Layout
constexpr float GRAPH_TOP = 20.f;        // top padding for graph
constexpr float GRAPH_LEFT = 60.f;       // space for y-axis labels
constexpr float GRAPH_RIGHT_PADDING = 20.f;
constexpr float GRAPH_BOTTOM_MARGIN = 60.f; // leave room for x-axis labels + spacing
constexpr float CONTROL_PANEL_HEIGHT = 120.f;

// Simulation
struct Neuron {
    float voltage = 0.f;       // normalized [0..1]
    float dv = 0.01f;         // change per step (simple demo)
    void update() {
        voltage += dv;
        if (voltage > 1.f) voltage = 0.f;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Neuron Simulator");
    window.setFramerateLimit(60);

    // Compute dynamic graph size
    const float graphWidth = WINDOW_WIDTH - GRAPH_LEFT - GRAPH_RIGHT_PADDING;
    const float graphHeight = WINDOW_HEIGHT - GRAPH_TOP - GRAPH_BOTTOM_MARGIN - CONTROL_PANEL_HEIGHT;

    // Simulation state
    std::vector<Neuron> neurons(1);
    const int max_history = static_cast<int>(std::round(graphWidth)); // 1 px per history step
    std::vector<std::vector<float>> voltage_history;
    voltage_history.resize(neurons.size());
    for (auto& h : voltage_history) h.assign(max_history, 0.f);

    int history_index = 0;
    bool isRunning = false;
    int simStep = 0;
    const float dt = 1.0f; // ms per simulation step
    double currentSimTimeMs = 0.0;

    // UI elements
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font 'arial.ttf' (place a ttf in the executable folder or change filename)\n";
        return -1;
    }

    // Buttons: add/remove
    const float smallButtonSize = 36.f;
    sf::RectangleShape addButton(sf::Vector2f(smallButtonSize, smallButtonSize));
    addButton.setFillColor(sf::Color::Green);
    sf::Text addText("+", font, 22);
    addText.setFillColor(sf::Color::Black);

    sf::RectangleShape removeButton(sf::Vector2f(smallButtonSize, smallButtonSize));
    removeButton.setFillColor(sf::Color::Red);
    sf::Text removeText("-", font, 26);
    removeText.setFillColor(sf::Color::Black);

    // Play/pause
    sf::RectangleShape playPauseButton(sf::Vector2f(48.f, 48.f));
    playPauseButton.setFillColor(sf::Color(100, 150, 255));
    sf::Text playPauseText("Play", font, 18);
    playPauseText.setFillColor(sf::Color::White);

    // Stimuli
    const int stimuliCount = 5;
    std::vector<sf::RectangleShape> stimuliButtons(stimuliCount);
    std::vector<sf::Text> stimuliLabels(stimuliCount);
    const std::vector<std::string> stimNames = { "S1","S2","S3","S4","S5" };
    for (int i = 0; i < stimuliCount; ++i) {
        stimuliButtons[i].setSize(sf::Vector2f(44.f, 36.f));
        stimuliButtons[i].setFillColor(sf::Color(120, 120, 120));
        stimuliLabels[i].setFont(font);
        stimuliLabels[i].setString(stimNames[i]);
        stimuliLabels[i].setCharacterSize(14);
        stimuliLabels[i].setFillColor(sf::Color::Black);
    }

    // Neuron count text
    sf::Text neuronCountText("", font, 18);
    neuronCountText.setFillColor(sf::Color::White);

    // Colors for neurons
    std::vector<sf::Color> neuronColors = {
        sf::Color::Red, sf::Color::Green, sf::Color::Cyan, sf::Color::Yellow, sf::Color::Magenta, sf::Color::White
    };

    // Helper: map voltage (0..1) to y in graph
    auto voltageToY = [&](float v) {
        float y0 = GRAPH_TOP; // top pixel of graph area
        float y1 = GRAPH_TOP + graphHeight; // bottom
        // invert: v=1 => y0+smallMargin, v=0 => y1
        const float topPadding = 6.0f; // ensure top isn't clipped
        return y0 + topPadding + (1.0f - v) * (graphHeight - topPadding - 6.0f);
        };

    // Helper: draw grid + axis labels (time scrolls)
    auto drawGridAndLabels = [&](sf::RenderWindow& win) {
        const int gridLinesY = 5;
        const int gridLinesX = 6;
        sf::Color gridColor(140, 140, 140, 90);

        float plotX0 = GRAPH_LEFT;
        float plotX1 = GRAPH_LEFT + graphWidth;
        float plotY0 = GRAPH_TOP;
        float plotY1 = GRAPH_TOP + graphHeight;

        // horizontal lines + voltage labels
        for (int i = 0; i <= gridLinesY; ++i) {
            float t = i / float(gridLinesY);
            float y = plotY0 + t * (plotY1 - plotY0);
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(plotX0, y), gridColor),
                sf::Vertex(sf::Vector2f(plotX1, y), gridColor)
            };
            win.draw(line, 2, sf::Lines);

            // label (1.0 top, 0.0 bottom)
            float vLabel = 1.0f - t;
            char buf[16];
            snprintf(buf, sizeof(buf), "%.2f", vLabel);
            sf::Text lbl(buf, font, 14);
            lbl.setFillColor(sf::Color::White);
            // place to left, ensure text fully visible
            sf::FloatRect lb = lbl.getLocalBounds();
            lbl.setOrigin(0, lb.height / 2.0f);
            lbl.setPosition(plotX0 - 48.f, y - lb.height / 2.0f);
            win.draw(lbl);
        }

        // vertical lines + time labels (scrolling)
        double visibleMs = max_history * dt; // leftmost..rightmost ms displayed
        double tRight = currentSimTimeMs; // rightmost time
        double tLeft = tRight - visibleMs; // leftmost time displayed
        // choose ticks evenly across visible range
        for (int i = 0; i <= gridLinesX; ++i) {
            float tfrac = i / float(gridLinesX);
            float x = plotX0 + tfrac * (plotX1 - plotX0);
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(x, plotY0), gridColor),
                sf::Vertex(sf::Vector2f(x, plotY1), gridColor)
            };
            win.draw(line, 2, sf::Lines);

            double timeLabelMs = tLeft + tfrac * visibleMs;
            int timeMsInt = static_cast<int>(std::round(timeLabelMs));
            sf::Text tlabel(std::to_string(timeMsInt) + " ms", font, 13);
            tlabel.setFillColor(sf::Color::White);
            sf::FloatRect tb = tlabel.getLocalBounds();
            tlabel.setOrigin(tb.width / 2.f, 0.f);
            tlabel.setPosition(x, plotY1 + 6.f);
            win.draw(tlabel);
        }

        // axes lines
        sf::Vertex yAxis[] = {
            sf::Vertex(sf::Vector2f(plotX0, plotY0), sf::Color::White),
            sf::Vertex(sf::Vector2f(plotX0, plotY1), sf::Color::White)
        };
        win.draw(yAxis, 2, sf::Lines);
        sf::Vertex xAxis[] = {
            sf::Vertex(sf::Vector2f(plotX0, plotY1), sf::Color::White),
            sf::Vertex(sf::Vector2f(plotX1, plotY1), sf::Color::White)
        };
        win.draw(xAxis, 2, sf::Lines);

        // Axis titles
        sf::Text yTitle("Voltage (norm)", font, 14);
        yTitle.setFillColor(sf::Color::White);
        // rotate and position near left center
        yTitle.setRotation(-90);
        sf::FloatRect yt = yTitle.getLocalBounds();
        yTitle.setOrigin(yt.width / 2.f, yt.height / 2.f);
        yTitle.setPosition(18.f, plotY0 + (plotY1 - plotY0) / 2.f);
        win.draw(yTitle);

        sf::Text xTitle("Time", font, 14);
        xTitle.setFillColor(sf::Color::White);
        sf::FloatRect xt = xTitle.getLocalBounds();
        xTitle.setPosition(plotX0 + (plotX1 - plotX0) / 2.f - xt.width / 2.f, plotY1 + 28.f);
        win.draw(xTitle);
        };

    // layout placements in control panel (bottom)
    const float panelY = GRAPH_TOP + graphHeight + 16.f; // control panel top
    const float panelPadding = 18.f;
    // left group x
    const float leftX = panelPadding;
    // center group for play/pause
    const float centerX = WINDOW_WIDTH * 0.5f - 24.f;
    // right group for stimuli
    const float rightXStart = WINDOW_WIDTH - (stimuliCount * 54.f) - panelPadding;

    // initial positions:
    addButton.setPosition(leftX, panelY + 12.f);
    addText.setPosition(leftX + 8.f, panelY + 6.f);
    removeButton.setPosition(leftX + 50.f, panelY + 12.f);
    removeText.setPosition(leftX + 57.f, panelY + 6.f);

    playPauseButton.setPosition(centerX, panelY + 6.f);
    playPauseText.setPosition(centerX + 56.f, panelY + 14.f);

    for (int i = 0; i < stimuliCount; ++i) {
        float x = rightXStart + i * 54.f;
        stimuliButtons[i].setPosition(x, panelY + 10.f);
        // center label in button
        sf::FloatRect b = stimuliButtons[i].getLocalBounds();
        stimuliLabels[i].setPosition(x + (b.width - stimuliLabels[i].getLocalBounds().width) / 2.f - 2.f, panelY + 12.f);
    }

    // Main loop
    sf::Clock clock;
    while (window.isOpen()) {
        float dtFrame = std::min(clock.restart().asSeconds(), 0.05f);
        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mpos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                // add
                if (addButton.getGlobalBounds().contains(mpos)) {
                    neurons.emplace_back();
                    voltage_history.emplace_back(std::vector<float>(max_history, 0.f));
                }
                else if (removeButton.getGlobalBounds().contains(mpos)) {
                    if (neurons.size() > 1) {
                        neurons.pop_back();
                        voltage_history.pop_back();
                    }
                }
                else if (playPauseButton.getGlobalBounds().contains(mpos)) {
                    isRunning = !isRunning;
                    playPauseText.setString(isRunning ? "Pause" : "Play");
                }
                else {
                    // stimuli only when running
                    for (int i = 0; i < stimuliCount; ++i) {
                        if (stimuliButtons[i].getGlobalBounds().contains(mpos) && isRunning) {
                            // apply stimulus to all neurons
                            float stimAmount = 0.1f + 0.2f * i; // 0.1,0.3,0.5,0.7,0.9
                            for (auto& n : neurons) {
                                n.voltage = std::min(1.0f, n.voltage + stimAmount);
                            }
                        }
                    }
                }
            }
        }

        // Simulation step
        if (isRunning) {
            // Advance sim in steps proportional to frame dt to keep sim time consistent
            // We'll do at least one sim step per frame; can scale for speed later
            int steps = 1; // you can increase to (int)std::max(1.0f, dtFrame*60.0f) if you want
            for (int s = 0; s < steps; ++s) {
                for (size_t i = 0; i < neurons.size(); ++i) {
                    neurons[i].update();
                    voltage_history[i][history_index] = neurons[i].voltage;
                }
                history_index = (history_index + 1) % max_history;
                simStep++;
                currentSimTimeMs = simStep * dt;
            }
        }

        // update UI-dependent colors for stimuli (enabled when running)
        for (int i = 0; i < stimuliCount; ++i) {
            if (isRunning) stimuliButtons[i].setFillColor(sf::Color(200, 200, 200));
            else stimuliButtons[i].setFillColor(sf::Color(100, 100, 100));
        }

        neuronCountText.setString("Neurons: " + std::to_string(neurons.size()));
        neuronCountText.setPosition(leftX + 130.f, panelY + 18.f);

        // render
        window.clear(sf::Color(28, 28, 28));

        // draw graph background
        sf::RectangleShape graphBg(sf::Vector2f(graphWidth, graphHeight));
        graphBg.setPosition(GRAPH_LEFT, GRAPH_TOP);
        graphBg.setFillColor(sf::Color(12, 12, 12));
        window.draw(graphBg);

        // draw grid and labels
        drawGridAndLabels(window);

        // draw voltage traces
        for (size_t n = 0; n < neurons.size(); ++n) {
            if (voltage_history[n].empty()) continue;
            sf::VertexArray va(sf::LineStrip, max_history);
            for (int i = 0; i < max_history; ++i) {
                int idx = (history_index + i) % max_history;
                float v = voltage_history[n][idx];
                float x = GRAPH_LEFT + float(i);
                float y = voltageToY(v);
                va[i].position = sf::Vector2f(x, y);
                va[i].color = neuronColors[n % neuronColors.size()];
            }
            window.draw(va);
        }

        // draw control panel background
        sf::RectangleShape controlBg(sf::Vector2f(float(WINDOW_WIDTH), CONTROL_PANEL_HEIGHT));
        controlBg.setPosition(0.f, GRAPH_TOP + graphHeight + 8.f);
        controlBg.setFillColor(sf::Color(45, 45, 45));
        window.draw(controlBg);

        // draw buttons & texts (positions already set)
        window.draw(addButton);
        window.draw(addText);
        window.draw(removeButton);
        window.draw(removeText);

        window.draw(playPauseButton);
        window.draw(playPauseText);
        window.draw(neuronCountText);

        // stimuli block label on right
        sf::Text stimBlockLabel("Stimuli", font, 16);
        stimBlockLabel.setFillColor(sf::Color::White);
        stimBlockLabel.setPosition(rightXStart - 70.f, panelY + 14.f);
        window.draw(stimBlockLabel);

        for (int i = 0; i < stimuliCount; ++i) {
            window.draw(stimuliButtons[i]);
            window.draw(stimuliLabels[i]);
        }

        window.display();
    }

    return 0;
}
