#ifndef CHESS_H
#define CHESS_H

#include "basic.h"
#include "SDL.h"

#include <string>
#include <algorithm>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include <boost/spirit/include/qi.hpp>

namespace chess {
    enum struct piece {
        none = 0,
        wPawn = 1,
        wKnight = 2,
        wBishop = 3,
        wRook = 4,
        wQueen = 5,
        wKing = 6,
        bPawn = -1,
        bKnight = -2,
        bBishop = -3,
        bRook = -4,
        bQueen = -5,
        bKing = -6
    };

    enum struct boardState {
        game = 0,
        promotion = 1
    };

    struct square {
        piece pieceOnSquare;
    };

    struct settings {
        SDL_Color boardColor[2];
    };

    class engineData {
    public:
        std::string engineMove = "none";

        engineData() {};
        engineData(std::string path) {
            // I feel this is a important bit of code to have... BUT...
            // It will sometimes break the application and it will sometimes
            // not... It works fine on komodo and stockfish but completely
            // snaps on UCI's like lc0...

            //int max_lines = 5;
            //for (std::string line; std::getline(_source, line); max_lines--) {
            //    std::cout << "Echo: " << _source.tellg() << " " << line << std::endl;
            //    return;
            //}

            _engine = boost::process::child{ path,
            boost::process::std_in<_sink, boost::process::std_out>_source };
        }
        engineData& operator=(const engineData& other) {
            setPath(other.enginePath);
            return *this;
        }
        void setPath(std::string path) {
            enginePath = path;
            _engine = boost::process::child{ path,
            boost::process::std_in<_sink, boost::process::std_out>_source };
        }
        std::string move(std::string moves) {
            //_sink << "setoption name Skill Level value 1";
            //_sink << std::endl;
            
            _sink << "position startpos moves " + moves;
            _sink << std::endl;
            
            _sink << "go depth 1";
            _sink << std::endl;
            
            std::string line;
            while (std::getline(_source, line)) {
                std::cout << line << std::endl;
                if (line.find("bestmove") != std::string::npos) {
                    return line.substr(9, line.length()-10);
                }

            }
        }
    private:
        std::string enginePath;
        boost::process::opstream _sink;
        boost::process::ipstream _source;
        boost::process::child    _engine;
    };

    class userData {
    public:
        bas::vector2 <int> mouse_pos;
        bas::vector2 <int8_t> mouseBoard_pos;
        bas::vector2 <int8_t> firstSquare  ={ -1,-1 };
        bas::vector2 <int8_t> secondSquare ={ -1,-1 };
        bool left_click  = false;
        bool right_click = false;
        boardState state;
        settings userSettings;
        void getMouseBoard_pos(bas::vector2 <float> boardDimensions, bas::vector2 <Uint16> boardPosition);
        void getTileClicked();
        void init();
    };

    class playerData {
    public:
        playerData(std::string path) {
            isBot = true;
            engineData.setPath(path);
        }
        playerData(userData* user = nullptr) {
            userData = user;
            for (int i = 0; i < 2; i++)
                rookMoved[i] = false;
        };
        playerData& operator=(const playerData& other) {
            isBot = other.isBot;
            kingMoved = other.kingMoved;
            if (isBot)
                engineData = other.engineData;
            else
                userData = other.userData;
            return *this;

        };

        bool isBot = false;
        bool kingMoved = false;
        bool inCheck = false;
        bool rookMoved[2];
        engineData engineData;
        userData* userData;
        bas::timer time;
    };

    class boardDisplay {
    public:
        std::vector<bas::vector2<int8_t>> optionalMoves;

        square boardData[8][8];
        bas::texture pTexture;
        bas::vector2 <float>  Dimension = bas::vector2 <float> (1024, 1024);
        bas::vector2 <Uint16> Position  = bas::vector2 <Uint16>(28, 28);
        void drawBoard(SDL_Renderer* Renderer, userData* User);
        void drawPieces(SDL_Renderer* Renderer, bas::vector2 <float> tileDim);
        void drawPromotionPieces(SDL_Renderer* Renderer, userData* User, bas::vector2 <float> tileDim, bas::vector2 <float> Offset);
        void drawPromotion(SDL_Renderer* Renderer, userData* User);
        void movePiece(bas::vector2 <int8_t> firstSquare, bas::vector2 <int8_t> secondSquare);
    };

    class gameData : public boardDisplay {
        std::vector<std::string> moves;

        void nextTurn();
        void getPawnMoves(bas::vector2<int8_t>& piece, chess::piece typeOfPiece, bool isAttacked, std::vector<bas::vector2<int8_t>>* piece_moves);
        void getKnightMoves(bas::vector2<int8_t>& piece, chess::piece typeOfPiece, bool isAttacked, std::vector<bas::vector2<int8_t>>* piece_moves);
        void getPieceMoves(std::vector <bas::vector2 <int8_t> >* moves, bas::vector2 <int8_t> piece, bool isAttacked = false);
        void getKingMoves(bas::vector2<int8_t>& piece, chess::piece typeOfPiece, bool isAttacked, std::vector<bas::vector2<int8_t>>* piece_moves);
        void getRookMoves(bas::vector2<int8_t>& piece, chess::piece typeOfPiece, bool isAttacked, std::vector<bas::vector2<int8_t>>* piece_moves);
        void getBishopMoves(bas::vector2<int8_t>& piece, chess::piece typeOfPiece, bool isAttacked, std::vector<bas::vector2<int8_t>>* piece_moves);
        bool getPieceColor(piece piece);
        bool isSquareAttacked(bas::vector2 <int8_t> square);
        bool isPromotion(bas::vector2 <int8_t>* square);
        bool isCheckmate();
        bool isInCheck();
        void getEnPassant(std::vector <bas::vector2 <int8_t> >* piece_moves, std::string lastMove, bas::vector2 <int8_t> square);
        void enPassant(bas::vector2 <int8_t> firstSquare, bas::vector2 <int8_t> secondSquare);
        void castleRooks(bas::vector2 <int8_t> firstSquare, bas::vector2 <int8_t> secondSquare);
        void isKingCastle();
        bool isLegalMove(bas::vector2 <int8_t> firstSquare, bas::vector2 <int8_t> secondSquare);
        bool isPieceOppositeColor(piece p1, piece p2);
        bool isSquareInGame(bas::vector2 <int8_t> square);
    public:
        gameData() {};

        bool isWhitesTurn = true;
        playerData players[2];
        std::string getMoves();
        void inputBoardPromotion(userData* User);
        void inputBoard();
        void updateBoard();
        bool addMove(bas::vector2 <int8_t> firstSquare, bas::vector2 <int8_t> secondSquare);
        void init(SDL_Renderer* Renderer);
    };
}

#endif
