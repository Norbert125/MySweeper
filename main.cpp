#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include "Board.h"

using namespace std;
using namespace sf;

enum class GameState { MENU, PLAYING };

Color getNumberColor(int val) {
    switch (val) {
        case 1: return {1, 0, 254};
        case 2: return {1, 127, 1};
        case 3: return {254, 0, 0};
        case 4: return {1, 0, 128};
        case 5: return {129, 1, 2};
        case 6: return {0, 128, 128};
        case 7: return {0, 0, 0};
        case 8: return {128, 128, 128};
        default: return Color::Black;
    }
}

int main() {
    int rows = 10, cols = 10, bombs = 10;
    string inputString = "";
    GameState currentState = GameState::MENU;

    // Start in 720p (1280x720)
    RenderWindow window(VideoMode(1280, 720), "Minesweeper Professional", Style::Default);

    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cout << "CRITICAL ERROR: Could not load 'arial.ttf'!" << endl;
        system("pause");
        return -1;
    }

    Texture bombTex, flagTex;
    bool assetsLoaded = bombTex.loadFromFile("bomb.png") && flagTex.loadFromFile("flag.png");

    Board* gameBoard = nullptr;
    const int cellSize = 32;
    const int uiHeight = 50;

    Clock gameClock;
    int timeElapsed = 0;
    bool timerRunning = false;
    const float btnWidth = 120.0f; // Slightly wider for 720p look
    const float btnHeight = 34.0f;

    float offsetX = 0;
    float offsetY = 0;

    while (window.isOpen()) {
        Event event{};
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            if (event.type == Event::Resized) {
                FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
                window.setView(View(visibleArea));

                if (currentState == GameState::PLAYING) {
                    offsetX = max(0.0f, (float)(event.size.width - (cols * cellSize)) / 2.0f);
                    offsetY = max(0.0f, (float)(event.size.height - (rows * cellSize + uiHeight)) / 2.0f);
                }
            }

            if (currentState == GameState::MENU) {
                if (event.type == Event::TextEntered) {
                    if (event.text.unicode < 128) {
                        char c = static_cast<char>(event.text.unicode);
                        if (isdigit(c)) inputString += c;
                        else if (c == '\b' && !inputString.empty()) inputString.pop_back();
                    }
                }
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                    if (!inputString.empty()) {
                        int val = stoi(inputString);

                        // Enforce Min 5 and Max 25 to keep visuals clear
                        if (val < 5) rows = 5;
                        else if (val > 25) rows = 25;
                        else rows = val;

                        cols = rows;
                        bombs = floor((rows * cols) * 0.15);
                        gameBoard = new Board(bombs, rows, cols);

                        // Resize window to fit the game grid
                        window.create(VideoMode(cols * cellSize, rows * cellSize + uiHeight), "Minesweeper Professional");
                        currentState = GameState::PLAYING;
                        timerRunning = false;
                        timeElapsed = 0;
                        offsetX = 0; offsetY = 0;
                    }
                }
            }
            else if (currentState == GameState::PLAYING && gameBoard != nullptr) {
                if (event.type == Event::MouseButtonPressed) {
                    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                    float btnX = offsetX + (float)(cols * cellSize) / 2.0f - (btnWidth / 2.0f);
                    if (mousePos.y >= offsetY + 8 && mousePos.y <= offsetY + 8 + btnHeight &&
                        mousePos.x >= btnX && mousePos.x <= btnX + btnWidth) {

                        delete gameBoard;
                        gameBoard = nullptr;
                        inputString = "";
                        currentState = GameState::MENU;
                        timerRunning = false;
                        timeElapsed = 0;
                        // Return to 720p Menu
                        window.create(VideoMode(1280, 720), "Minesweeper Professional");
                        break;
                    }

                    int c = (int)(mousePos.x - offsetX) / cellSize;
                    int r = (int)(mousePos.y - offsetY - uiHeight) / cellSize;

                    if (r >= 0 && r < rows && c >= 0 && c < cols && !gameBoard->isGameOver() && !gameBoard->checkWin()) {
                        if (!timerRunning) {
                            gameClock.restart();
                            timerRunning = true;
                        }

                        if (event.mouseButton.button == Mouse::Left) {
                            if (gameBoard->getMaskValue(r, c) == 1) gameBoard->chord(r, c);
                            else gameBoard->reveal(r, c);
                            if (gameBoard->isGameOver()) gameBoard->revealAllBombs();
                        } else if (event.mouseButton.button == Mouse::Right) {
                            gameBoard->toggleFlag(r, c);
                        }
                    }
                }
            }
        }

        window.clear(Color(40, 40, 40));

        if (currentState == GameState::MENU) {
            // Enhanced 720p Menu Positioning
            float centerX = (float)window.getSize().x / 2.0f;
            float centerY = (float)window.getSize().y / 2.0f;

            Text titleTxt("MINESWEEPER PRO", font, 50);
            titleTxt.setStyle(Text::Bold);
            titleTxt.setFillColor(Color::Cyan);
            FloatRect titleBounds = titleTxt.getLocalBounds();
            titleTxt.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
            titleTxt.setPosition(centerX, centerY - 150);
            window.draw(titleTxt);

            Text menuTxt("ENTER BOARD SIZE (5 - 25):", font, 28);
            FloatRect menuBounds = menuTxt.getLocalBounds();
            menuTxt.setOrigin(menuBounds.left + menuBounds.width / 2.0f, menuBounds.top + menuBounds.height / 2.0f);
            menuTxt.setPosition(centerX, centerY - 50);
            window.draw(menuTxt);

            Text inputTxt(inputString + "_", font, 40);
            inputTxt.setFillColor(Color::Yellow);
            FloatRect inputBounds = inputTxt.getLocalBounds();
            inputTxt.setOrigin(inputBounds.left + inputBounds.width / 2.0f, inputBounds.top + inputBounds.height / 2.0f);
            inputTxt.setPosition(centerX, centerY + 20);
            window.draw(inputTxt);

            Text hintTxt("Press ENTER to Begin", font, 18);
            hintTxt.setFillColor(Color(180, 180, 180));
            FloatRect hintBounds = hintTxt.getLocalBounds();
            hintTxt.setOrigin(hintBounds.left + hintBounds.width / 2.0f, hintBounds.top + hintBounds.height / 2.0f);
            hintTxt.setPosition(centerX, centerY + 100);
            window.draw(hintTxt);
        }
        else if (currentState == GameState::PLAYING && gameBoard != nullptr) {
            // Existing Playing State Logic...
            if (timerRunning && !gameBoard->isGameOver() && !gameBoard->checkWin()) {
                timeElapsed = (int)gameClock.getElapsedTime().asSeconds();
            }

            RectangleShape header(Vector2f((float)cols * cellSize, (float)uiHeight));
            header.setPosition(offsetX, offsetY);
            header.setFillColor(Color(30, 30, 30));
            window.draw(header);

            Text counterText("MINES: " + to_string(bombs), font, 16);
            counterText.setPosition(offsetX + 10, offsetY + 15);
            window.draw(counterText);

            float btnX = offsetX + (float)(cols * cellSize) / 2.0f - (btnWidth / 2.0f);
            RectangleShape btnRect(Vector2f(btnWidth, btnHeight));
            btnRect.setPosition(btnX, offsetY + 8);
            btnRect.setFillColor(Color(70, 70, 70));
            btnRect.setOutlineThickness(2);
            btnRect.setOutlineColor(Color::White);
            window.draw(btnRect);

            Text btnLabel("Restart", font, 18);
            FloatRect labelRect = btnLabel.getLocalBounds();
            btnLabel.setOrigin(labelRect.left + labelRect.width / 2.0f, labelRect.top + labelRect.height / 2.0f);
            btnLabel.setPosition(btnX + (btnWidth / 2.0f), offsetY + 8 + (btnHeight / 2.0f));
            window.draw(btnLabel);

            Text timerTxt("TIME: " + to_string(timeElapsed), font, 16);
            timerTxt.setPosition(offsetX + (float)cols * cellSize - 90, offsetY + 15);
            window.draw(timerTxt);

            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    RectangleShape rect(Vector2f(cellSize - 2, cellSize - 2));
                    rect.setPosition(offsetX + (float)j * cellSize + 1, offsetY + (float)(i * cellSize + 1) + uiHeight);

                    int state = gameBoard->getMaskValue(i, j);
                    int val = gameBoard->getGameBoard()[i][j];

                    if (state == 0) rect.setFillColor(Color(80, 80, 80));
                    else if (state == 2) rect.setFillColor(Color(100, 100, 100));
                    else rect.setFillColor(Color(180, 180, 180));
                    window.draw(rect);

                    if (state == 2 && assetsLoaded) {
                        Sprite flag(flagTex);
                        flag.setScale((float)cellSize/(float)flagTex.getSize().x, (float)cellSize/(float)flagTex.getSize().y);
                        flag.setPosition(offsetX + (float)j * cellSize, offsetY + (float)i * cellSize + uiHeight);
                        window.draw(flag);
                    }
                    else if (state == 1) {
                        if (val == -1 && assetsLoaded) {
                            Sprite bomb(bombTex);
                            bomb.setScale((float)cellSize/(float)bombTex.getSize().x, (float)cellSize/(float)bombTex.getSize().y);
                            bomb.setPosition(offsetX + (float)j * cellSize, offsetY + (float)i * cellSize + uiHeight);
                            window.draw(bomb);
                        } else if (val > 0) {
                            Text txt(to_string(val), font, 18);
                            txt.setStyle(Text::Bold);
                            txt.setFillColor(getNumberColor(val));
                            txt.setPosition(offsetX + (float)j * cellSize + 10, offsetY + (float)i * cellSize + 5 + uiHeight);
                            window.draw(txt);
                        }
                    }
                }
            }

            if (gameBoard->isGameOver() || gameBoard->checkWin()) {
                RectangleShape overlay(Vector2f((float)cols * cellSize, 80));
                overlay.setPosition(offsetX, offsetY + (float)((rows * cellSize) / 2 - 40) + uiHeight);
                overlay.setFillColor(Color(0, 0, 0, 220));
                window.draw(overlay);

                Text statusText;
                statusText.setFont(font);
                statusText.setCharacterSize(24);
                statusText.setString(gameBoard->isGameOver() ? "GAME OVER" : "VICTORY!");
                statusText.setFillColor(gameBoard->isGameOver() ? Color::Red : Color::Green);

                FloatRect textRect = statusText.getLocalBounds();
                statusText.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
                statusText.setPosition(Vector2f(offsetX + (float)cols*cellSize/2.0f, offsetY + ((float)rows*cellSize/2.0f) + uiHeight));
                window.draw(statusText);
            }
        }
        window.display();
    }
    delete gameBoard;
    return 0;
}