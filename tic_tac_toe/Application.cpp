#include "Application.h"
#include "imgui/imgui.h"
#include "classes/tic_tac_toe.h"

namespace ClassGame {
        //
        // our global variables
        //
        tic_tac_toe *game = nullptr;
        int gameWinner = -1;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = new tic_tac_toe();
            game->setUpBoard();
            gameWinner = -1;
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

                ImGui::Begin("Settings");
                ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                ImGui::Text("Current Board State: %s", game->stateString().c_str());
                if (game->checkForDraw()) {
                    ImGui::Text("Game Over!");
                    ImGui::Text("Draw!");
                } else {
                    if (gameWinner != -1) {
                        ImGui::Text("Game Over!");
                        ImGui::Text("Winner: %d", gameWinner+1);
                    }
                }
                if (ImGui::Button("Reset Game")) {
                    game->stopGame();
                    game->setUpBoard();
                    gameWinner = -1;
                }
                ImGui::End();

                if (game->gameHasAI() && game->getCurrentPlayer()->isAIPlayer())
                {
                    game->updateAI();
                }

                ImGui::Begin("GameWindow");
                game->drawFrame();
                ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameWinner = winner->playerNumber();
            }
        }
}
