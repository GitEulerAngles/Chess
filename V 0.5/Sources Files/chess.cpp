#include "chess.h"

template<typename square>
void ChessNotationToCoords(std::string notation, bas::vector2 <square>* s1, bas::vector2 <square>* s2) {
    s1->x = ((int)notation[0]-97);
    s1->y = abs(((int)notation[1]-56));
    s2->x = ((int)notation[2]-97);
    s2->y = abs(((int)notation[3]-56));
}
template<typename square>
std::string CoordsToChessNotation(bas::vector2 <square> s1, bas::vector2 <square> s2) {
    std::string notation;

    notation += (char)(s1.x + 97);
    notation += (char)((7-(int)s1.y)+49);
    notation += (char)(s2.x + 97);
    notation += (char)((7-(int)s2.y)+49);

    return notation;
}

bool chess::gameData::getPieceColor(chess::piece piece) {
    // 1 = white
    // 0 = black
    if ((int)piece < 0)
        return false;
    return true;
}
bool chess::gameData::isSquareInGame(bas::vector2 <int8_t> square) {
    if (square.x < 0 || square.x > 7)
        return false;
    if (square.y < 0 || square.y > 7)
        return false;
    return true;
}
bool chess::gameData::isPieceOppositeColor(piece p1, piece p2) {
    bool colors[2];
    colors[0] = getPieceColor(p1);
    colors[1] = getPieceColor(p2);
    if (colors[0] == colors[1])
        return false;
    return true;
}
bool chess::gameData::isSquareAttacked(bas::vector2 <int8_t> square) {
    chess::piece piece = boardData[square.y][square.x].pieceOnSquare;
    for (int8_t y = 0; y < 8; y++)
    for (int8_t x = 0; x < 8; x++) {
        chess::piece current_piece = boardData[y][x].pieceOnSquare;
        if (current_piece == chess::piece::none || getPieceColor(current_piece) == isWhitesTurn)
            continue;

        std::vector <bas::vector2 <int8_t> > squaresPieceAttacks;
        getPieceMoves(&squaresPieceAttacks, {x,y}, true);

        for (int8_t i = 0; i < squaresPieceAttacks.size(); i++) {
            bas::vector2 <int8_t> current_square = squaresPieceAttacks[i];
            if (current_square.x == square.x && current_square.y == square.y)
                return true;
        }
    }
    return false;
}

bool chess::gameData::isPromotion(bas::vector2 <int8_t> *square = nullptr) {
    for (int8_t x = 0; x < 8; x++)
    for (int8_t y = 0; y < 2; y++) {
        if (boardData[y * 7][x].pieceOnSquare == piece::wPawn || 
            boardData[y * 7][x].pieceOnSquare == piece::bPawn) {
            if (square == nullptr)
                return true;
            square->x = x;
            square->y = y * 7;
            return true;
        }
    }
    return false;
}
bool chess::gameData::isCheckmate() {
    playerData* current_player = &players[(int)isWhitesTurn];
    bool checkmate = true;
    for (int8_t y = 0; y < 8; y++)
    for (int8_t x = 0; x < 8; x++) {
        chess::piece current_piece = boardData[y][x].pieceOnSquare;
        if (current_piece == chess::piece::none || getPieceColor(current_piece) != isWhitesTurn)
            continue;

        std::vector <bas::vector2 <int8_t> > moves;
        getPieceMoves(&moves, { x,y }, false);

        for (int8_t i = 0; i < moves.size(); i++) {
            bas::vector2 <int8_t> current_square = moves[i];
            
            piece move_piece = boardData[current_square.y][current_square.x].pieceOnSquare;
            
            movePiece({ x,y }, current_square);

            if (!isInCheck())
                checkmate = false;

            boardData[current_square.y][current_square.x].pieceOnSquare = move_piece;
            boardData[y][x].pieceOnSquare = current_piece;
        }
    }

    return checkmate;
}
bool chess::gameData::isInCheck() {
    playerData* current_player = &players[(int)isWhitesTurn];
    for (int8_t y = 0; y < 8; y++)
    for (int8_t x = 0; x < 8; x++) {
        chess::piece current_piece = boardData[y][x].pieceOnSquare;
        if (abs((int)current_piece) != 6 || getPieceColor(current_piece) != isWhitesTurn)
            continue;

        if (isSquareAttacked({ x,y })) {
            return true;
        }
        return false;
    }
}

void chess::gameData::getEnPassant(std::vector <bas::vector2 <int8_t> >* piece_moves, std::string lastMove, bas::vector2 <int8_t> square) {
    bas::vector2 <int8_t> enPassantSquare = {};
    
    bas::vector2 <int8_t> s1;
    bas::vector2 <int8_t> s2;

    ChessNotationToCoords(lastMove, &s1, &s2);
    int dist = abs(s1.y - s2.y);
    if (abs((int)boardData[s2.y][s2.x].pieceOnSquare) != 1 || dist < 2)
        return;

    for (int i = -1; i < 2; i += 2) {
        if (s2.x == square.x + i && s2.y == square.y) {
            piece_moves->push_back({s2.x, s2.y + (int8_t)boardData[s2.y][s2.x].pieceOnSquare });
        }
    }
}
void chess::gameData::enPassant(bas::vector2 <int8_t> firstSquare, bas::vector2 <int8_t> secondSquare) {
    int type_of_piece = (int)boardData[firstSquare.y][firstSquare.x].pieceOnSquare;

    if (abs(firstSquare.x - secondSquare.x) == 1 &&
        boardData[secondSquare.y][secondSquare.x].pieceOnSquare == piece::none)
        boardData[secondSquare.y + type_of_piece][secondSquare.x].pieceOnSquare = piece::none;
}
void chess::gameData::castleRooks(bas::vector2 <int8_t> firstSquare, bas::vector2 <int8_t> secondSquare) {
    // White
    if (firstSquare.x == 4 && firstSquare.y == 7
        && secondSquare.x == 6 && secondSquare.y == 7)
        movePiece({ 7,7 }, { 5,7 });
    if (firstSquare.x == 4 && firstSquare.y == 7
        && secondSquare.x == 2 && secondSquare.y == 7)
        movePiece({ 0,7 }, { 3,7 });

    // Black
    if (firstSquare.x == 4 && firstSquare.y == 0
        && secondSquare.x == 6 && secondSquare.y == 0)
        movePiece({ 7,0 }, { 5,0 });
    if (firstSquare.x == 4 && firstSquare.y == 0
        && secondSquare.x == 2 && secondSquare.y == 0)
        movePiece({ 0,0 }, { 3,0 });
}
void chess::gameData::isKingCastle() {
    playerData* current_player = &players[(int)isWhitesTurn];
    if (current_player->kingMoved)
        return;

    bool piecesInWay = false;
    for (int8_t i = 1; i <= 2; i++) {
        if (boardData[7][4 + i].pieceOnSquare != piece::none && isSquareAttacked({4+i,7}))
            piecesInWay = true;
    }

    if (!current_player->rookMoved[0] && !piecesInWay)
        optionalMoves.push_back({ 6,7 });

    piecesInWay = false;
    for (int8_t i = 1; i <= 3; i++) {
        if (boardData[7][i].pieceOnSquare != piece::none && isSquareAttacked({ i,7 }))
            piecesInWay = true;;
    }

    if (!current_player->rookMoved[1] && !piecesInWay)
        optionalMoves.push_back({ 2,7 });
}
bool chess::gameData::isLegalMove(bas::vector2 <int8_t> firstSquare, bas::vector2 <int8_t> secondSquare) {
    chess::piece pieceType = boardData[firstSquare.y][firstSquare.x].pieceOnSquare;

    // Can't move a none piece.
    if (pieceType == chess::piece::none)
        return false;
    // Check if turn & right color.
    if (isWhitesTurn) {
        if ((int)pieceType < 0)
            return false;
    }
    // Check if piece is the right color.
    else if ((int)pieceType > 0)
        return false;
    // Check if piece has the abilities to
    // go to that square.

    bool optionalMove = false;
    for (int i = 0; i < optionalMoves.size(); i++) {
        bas::vector2<int8_t> currentMove = optionalMoves[i];
        if (currentMove.x == secondSquare.x && currentMove.y == secondSquare.y)
            optionalMove = true;
    }
    if (!optionalMove)
        return false;

    chess::piece pieceTwo = boardData[secondSquare.y][secondSquare.x].pieceOnSquare;

    movePiece(firstSquare, secondSquare);
    bool legal = true;
    if (isInCheck())
        legal = false;
    boardData[secondSquare.y][secondSquare.x].pieceOnSquare = pieceTwo;
    boardData[firstSquare.y][firstSquare.x].pieceOnSquare = pieceType;
    return legal;
}

void chess::gameData::getPawnMoves(bas::vector2<int8_t>& piece, chess::piece typeOfPiece, bool isAttacked, std::vector<bas::vector2<int8_t>>* piece_moves)
{
    int8_t direction = (int)typeOfPiece;
    if (!isAttacked) {
        if (moves.size() != 0)
            getEnPassant(piece_moves, moves[moves.size() - 1], piece);

        for (int8_t i = 1; i <= 2; i++) {
            // Make sure no piece is infront of pawn.
            if (boardData[piece.y + direction * -i][piece.x].pieceOnSquare != chess::piece::none)
                break;
            piece_moves->push_back({ piece.x, piece.y + direction * -i });
            // If piece has moved than it can't go two up or down.
            if (piece.y != (direction * 2.5) + 3.5)
                break;
        }
    }
    for (int8_t i = -1; i <= 1; i += 2) {
        chess::piece p_i = boardData[piece.y + direction * -1][piece.x + i].pieceOnSquare;
        chess::piece p = boardData[piece.y][piece.x].pieceOnSquare;
        // Diagonal piece is opposite color
        if ((p_i != chess::piece::none && isPieceOppositeColor(p, p_i)) || isAttacked)
            piece_moves->push_back({ piece.x + i, piece.y + direction * -1 });
    }
}
void chess::gameData::getKnightMoves(bas::vector2<int8_t>& piece, chess::piece typeOfPiece, bool isAttacked, std::vector<bas::vector2<int8_t>>* piece_moves)
{
    // Knights mathematically move (±1,±2),(±2,±1).
    // This first loop solves the separate points.
    for (int8_t i = -2; i <= -1; i++) {
        int8_t j = -i - 3;
        for (int8_t x = i; x <= abs(i); x += abs(i) * 2)
            for (int8_t y = j; y <= abs(j); y += abs(j) * 2) {
                if (x == 0 && y == 0)
                    continue;
                if (!isSquareInGame({ piece.x + x, piece.y + y }))
                    continue;
                chess::piece p = boardData[piece.y + y][piece.x + x].pieceOnSquare;
                // Piece can only move on opposite color squares and
                // squares that have no pieces...
                if (isPieceOppositeColor(typeOfPiece, p) || p == chess::piece::none || isAttacked)
                    piece_moves->push_back({ piece.x + x, piece.y + y });
            }
    }
}
void chess::gameData::getKingMoves(bas::vector2<int8_t>& piece, chess::piece typeOfPiece, bool isAttacked, std::vector<bas::vector2<int8_t>>* piece_moves)
{
    if (!isAttacked && !isInCheck())
        isKingCastle();
    for (int8_t x = -1; x <= 1; x++)
        for (int8_t y = -1; y <= 1; y++) {
            if (x == 0 && y == 0)
                continue;
            bas::vector2 <int8_t> current = { piece.x + x, piece.y + y };
            chess::piece current_piece = boardData[current.y][current.x].pieceOnSquare;
            if (!isSquareInGame(current))
                continue;
            if (!isAttacked && isSquareAttacked(current))
                continue;
            if (current_piece != chess::piece::none) {
                if (isPieceOppositeColor(typeOfPiece, current_piece) || isAttacked)
                    piece_moves->push_back(current);

                continue;
            }
            piece_moves->push_back(current);
        }
}
void chess::gameData::getRookMoves(bas::vector2<int8_t>& piece, chess::piece typeOfPiece, bool isAttacked, std::vector<bas::vector2<int8_t>>* piece_moves)
{
    int8_t moveSlope[4][2] = { { 1,0 },{ 0,1 },{ -1,0 },{ 0,-1 } };
    for (int8_t x = 0; x < 4; x++)
        for (int8_t i = 1;; i++) {
            bas::vector2 <int8_t> current = { piece.x + moveSlope[x][0] * i, piece.y + moveSlope[x][1] * i };
            chess::piece current_piece = boardData[current.y][current.x].pieceOnSquare;
            if (!isSquareInGame(current))
                break;
            if (current_piece != chess::piece::none) {
                if (isPieceOppositeColor(typeOfPiece, current_piece) || isAttacked) {
                    piece_moves->push_back(current);
                    break;
                }
                else
                    break;
            }
            piece_moves->push_back(current);
        }
}
void chess::gameData::getBishopMoves(bas::vector2<int8_t>& piece, chess::piece typeOfPiece, bool isAttacked, std::vector<bas::vector2<int8_t>>* piece_moves)
{
    for (int8_t x = -1; x <= 1; x += 2)
        for (int8_t y = -1; y <= 1; y += 2) {
            for (int8_t i = 1;; i++) {
                bas::vector2 <int8_t> current = { piece.x + x * i, piece.y + y * i };
                chess::piece current_piece = boardData[current.y][current.x].pieceOnSquare;
                if (!isSquareInGame(current))
                    break;
                if (current_piece != chess::piece::none) {
                    if (isPieceOppositeColor(typeOfPiece, current_piece) || isAttacked) {
                        piece_moves->push_back(current);
                        break;
                    }
                    else
                        break;
                }
                piece_moves->push_back(current);
            }
        }
}

void chess::gameData::getPieceMoves(std::vector <bas::vector2 <int8_t> >* piece_moves, bas::vector2 <int8_t> piece, bool isAttacked) {
    chess::piece typeOfPiece = boardData[piece.y][piece.x].pieceOnSquare;
    chess::piece normalPiece = (chess::piece)abs((int)typeOfPiece);

    piece_moves->clear();

    switch (normalPiece)
    {
    case chess::piece::wPawn:
        getPawnMoves(piece, typeOfPiece, isAttacked, piece_moves);
        break;

    case chess::piece::wKnight:
        getKnightMoves(piece, typeOfPiece, isAttacked, piece_moves);
        break;

    case chess::piece::wQueen:
    case chess::piece::wBishop:
        getBishopMoves(piece, typeOfPiece, isAttacked, piece_moves);
    if (normalPiece == chess::piece::wBishop)
        break;

    case chess::piece::wRook:
        getRookMoves(piece, typeOfPiece, isAttacked, piece_moves);
        break;

    case chess::piece::wKing:
        getKingMoves(piece, typeOfPiece, isAttacked, piece_moves);
        break;
    }
}
void chess::boardDisplay::movePiece(bas::vector2 <int8_t> firstSquare, bas::vector2 <int8_t> secondSquare) {
    boardData[secondSquare.y][secondSquare.x].pieceOnSquare = boardData[firstSquare.y][firstSquare.x].pieceOnSquare;
    boardData[firstSquare.y][firstSquare.x].pieceOnSquare = piece::none;
}
bool chess::gameData::addMove(bas::vector2 <int8_t> firstSquare, bas::vector2 <int8_t> secondSquare) {
    if (!isLegalMove(firstSquare,secondSquare))
        return false;

    int piece = (int)boardData[firstSquare.y][firstSquare.x].pieceOnSquare;
    if (abs(piece) == 1)
        enPassant(firstSquare, secondSquare);
    if (abs(piece) == 6) {
        players[(int)isWhitesTurn].kingMoved = true;
        castleRooks(firstSquare, secondSquare);
    }
    if (firstSquare.x == 7, firstSquare.y == 7)
        players[(int)isWhitesTurn].rookMoved[0] = false;
    if (firstSquare.x == 0, firstSquare.y == 7)
        players[(int)isWhitesTurn].rookMoved[1] = false;

    movePiece(firstSquare, secondSquare);
    moves.push_back(CoordsToChessNotation(firstSquare, secondSquare));

    return true;
}
std::string chess::gameData::getMoves() {
    std::string moveStr;
    for (int i = 0; i < moves.size(); i++)
        moveStr += moves[i] + ' ';
    return moveStr;
}
void chess::gameData::nextTurn() {
    if (isWhitesTurn)
        isWhitesTurn = false;
    else
        isWhitesTurn = true;
}

void chess::gameData::inputBoardPromotion(userData* User) {
    float tileWidth = Dimension.x / 8;
    float tileHeight = Dimension.y / 8;

    float OffsetX = 0, OffsetY = 0;

    if (2 * tileWidth + User->firstSquare.x * tileWidth > Dimension.x)
        OffsetX = 1;
    if (2 * tileHeight + User->firstSquare.y * tileHeight > Dimension.y)
        OffsetY = 1;

    for (int8_t y = 0; y < 2; y++)
    for (int8_t x = 0; x < 2; x++) {
        if (User->mouseBoard_pos.x == User->firstSquare.x + x - OffsetX && 
            User->mouseBoard_pos.y == User->firstSquare.y + y - OffsetY)
            User->secondSquare = {x,y};
    }

    char pieces[4] = { 'N','b','r','q' };
    if (User->left_click) {
        boardData[User->firstSquare.y][User->firstSquare.x].pieceOnSquare = (piece)(User->secondSquare.x + User->secondSquare.y * 2 + 2);
        moves[moves.size() - 1] += pieces[abs(int(boardData[User->firstSquare.y][User->firstSquare.x].pieceOnSquare)-2)];
        nextTurn();
        User->state = boardState::game;
    }
}
void chess::gameData::inputBoard() {
    playerData* current_player = &players[(int)isWhitesTurn];
    bas::vector2 <int8_t> s1, s2;
    if (current_player->isBot) {
        std::string engineMove = current_player->engineData.move(getMoves());
        std::cout << "|" <<  engineMove << "|" << std::endl;

        current_player->engineData.engineMove = engineMove;
    }
}
void chess::gameData::updateBoard() {
    if (isCheckmate()) {
        std::cout << "Checkmate" << std::endl;
        return;
    }

    bas::vector2 <int8_t> s1, s2;
    playerData* current_player = &players[(int)isWhitesTurn];
    if (current_player->isBot) {
        std::string engineMove = current_player->engineData.engineMove;

        if (engineMove == "none")
            return;

        moves.push_back(engineMove);
        ChessNotationToCoords(engineMove, &s1, &s2);

        int piece_type = (int)boardData[s1.y][s1.x].pieceOnSquare;

        if (abs(piece_type) == 6)
            castleRooks(s1, s2);
        else if (abs(piece_type) == 1)
            enPassant(s1, s2);

        movePiece(s1, s2);

        bas::vector2 <int8_t> squareOfPromotion;
        if (isPromotion(&squareOfPromotion)) {
            if (engineMove[4] == 'q')
                boardData[squareOfPromotion.y][squareOfPromotion.x].pieceOnSquare = (piece)(piece_type * 5);
        }

        current_player->engineData.engineMove = "none";
        nextTurn();;
    }
    else if (current_player->userData->firstSquare.x != -1) {
        s1 = current_player->userData->firstSquare;
        getPieceMoves(&optionalMoves, s1);
        if (current_player->userData->secondSquare.x != -1) {
            s2 = current_player->userData->secondSquare;
            if (addMove(s1, s2)) {
                if (isPromotion(&current_player->userData->firstSquare))
                    current_player->userData->state = boardState::promotion;
                else {
                    nextTurn();
                    current_player->userData->firstSquare.x = -1;
                    current_player->userData->secondSquare.x = -1;
                }
            }
                
            optionalMoves.clear();
        }
    }
}
void chess::boardDisplay::drawPieces(SDL_Renderer* Renderer, bas::vector2 <float> tileDim) {
    pTexture.screenCoords.w = tileDim.x;
    pTexture.screenCoords.h = tileDim.y;
    pTexture.imageCoords.w = 200;
    pTexture.imageCoords.h = 200;
    for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++) {
        int t = (int)boardData[y][x].pieceOnSquare;
        if (t == 0) {
            continue;
        }
        if (t < 0) {
            pTexture.imageCoords.y = pTexture.imageCoords.h;
            t = abs(t);
        }
        else {
            pTexture.imageCoords.y = 0;
        }
        t -= 1;
        pTexture.imageCoords.x = t * pTexture.imageCoords.w;
        pTexture.screenCoords.x = Position.x + (tileDim.x * x);
        pTexture.screenCoords.y = Position.y + (tileDim.y * y);
        pTexture.drawTexture(Renderer);
    }
}
void chess::boardDisplay::drawBoard(SDL_Renderer* Renderer, userData* User) {
    float tileWidth = Dimension.x / 8;
    float tileHeight = Dimension.y / 8;

    SDL_Color firstSquare = User->userSettings.boardColor[0];
    SDL_Color secondSquare = User->userSettings.boardColor[1];

    for (int8_t y = 0; y < 8; y++)
    for (int8_t x = 0; x < 8; x++) {
        if ((x + y) % 2 == 0) { //If is Even
            SDL_SetRenderDrawColor(Renderer, firstSquare.r, firstSquare.g, firstSquare.b, 255);
            if (User->mouseBoard_pos.x == x && User->mouseBoard_pos.y == y)
                SDL_SetRenderDrawColor(Renderer, firstSquare.r - 20, firstSquare.g - 20, firstSquare.b - 20, 255);
        }
        else {                //If is Odd
            SDL_SetRenderDrawColor(Renderer, secondSquare.r, secondSquare.g, secondSquare.b, 255);
            if (User->mouseBoard_pos.x == x && User->mouseBoard_pos.y == y)
                SDL_SetRenderDrawColor(Renderer, secondSquare.r - 20, secondSquare.g - 20, secondSquare.b - 20, 255);
        }

        if (User->firstSquare.x == x && User->firstSquare.y == y)
            SDL_SetRenderDrawColor(Renderer, 235, 130, 66, 255);

        SDL_Rect tile;
        tile.x = x * tileWidth + Position.x;
        tile.y = y * tileHeight + Position.y;
        tile.w = tileWidth;
        tile.h = tileHeight;

        SDL_RenderFillRect(Renderer, &tile);
    }

    drawPieces(Renderer, bas::vector2 <float>(tileWidth, tileHeight));
}
void chess::boardDisplay::drawPromotionPieces(SDL_Renderer* Renderer, userData* User, bas::vector2 <float> tileDim, bas::vector2 <float> Offset) {
    pTexture.screenCoords.w = tileDim.x;
    pTexture.screenCoords.h = tileDim.y;
    pTexture.imageCoords.w = 200;
    pTexture.imageCoords.h = 200;

    bas::vector2 <int8_t> piece_position = User->firstSquare;

    for (int8_t y = 0; y < 2; y++)
    for (int8_t x = 0; x < 2; x++) {
        int t = (int)boardData[piece_position.y][piece_position.x].pieceOnSquare;
        if (t < 0) {
            pTexture.imageCoords.y = pTexture.imageCoords.h;
        }
        else {
            pTexture.imageCoords.y = 0;
        }

        pTexture.imageCoords.x = (x + y * 2 + 1) * pTexture.imageCoords.w;
        pTexture.screenCoords.x = x * tileDim.x + User->firstSquare.x * tileDim.x + Position.x + Offset.x;
        pTexture.screenCoords.y = y * tileDim.y + User->firstSquare.y * tileDim.y + Position.y + Offset.y;
        pTexture.drawTexture(Renderer);
    }
}
void chess::boardDisplay::drawPromotion(SDL_Renderer* Renderer, userData* User) {
    float tileWidth = Dimension.x / 8;
    float tileHeight = Dimension.y / 8;

    float OffsetX = 0, OffsetY = 0;

    if (2 * tileWidth + User->firstSquare.x * tileWidth > Dimension.x)
        OffsetX = -tileWidth;
    if (2 * tileHeight + User->firstSquare.y * tileHeight > Dimension.y)
        OffsetY = -tileHeight;

    SDL_Color firstSquare = User->userSettings.boardColor[0];
    SDL_Color secondSquare = User->userSettings.boardColor[1];

    for (int8_t y = 0; y < 2; y++)
    for (int8_t x = 0; x < 2; x++) {
        if (User->mouseBoard_pos.x == User->firstSquare.x + x + (OffsetX / tileWidth) &&
            User->mouseBoard_pos.y == User->firstSquare.y + y + (OffsetY / tileHeight))
            SDL_SetRenderDrawColor(Renderer, 100, 100, 100, 60);
        else if ((x + y) % 2 == 0) //If is Even
            SDL_SetRenderDrawColor(Renderer, secondSquare.r-20, secondSquare.g, secondSquare.b, 60);
        else
            SDL_SetRenderDrawColor(Renderer, firstSquare.r-20, firstSquare.g, firstSquare.b, 60);

        SDL_Rect tile;
        tile.x = x * tileWidth + User->firstSquare.x * tileWidth + Position.x + OffsetX;
        tile.y = y * tileHeight + User->firstSquare.y * tileHeight + Position.y + OffsetY;
        tile.w = tileWidth;
        tile.h = tileHeight;

        SDL_RenderFillRect(Renderer, &tile);
    }

    drawPromotionPieces(Renderer, User, { tileWidth, tileHeight }, {OffsetX, OffsetY});
}

void chess::gameData::init(SDL_Renderer* Renderer) {
    pTexture.loadMedia(Renderer, "C:/Users/Kurt/source/repos/Chess/Project2/ChessPieces/Pieces#1.png");
    int set[8][8] ={
        {-4,-2,-3,-5,-6,-3,-2,-4},
        {-1,-1,-1,-1,-1,-1,-1,-1},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 1, 1, 1, 1, 1, 1, 1, 1},
        { 4, 2, 3, 5, 6, 3, 2, 4},
        //{ 0, 0, 0, 0, 0, 0, 0, 0},
        //{ 0, 0, 0, 0,-1, 0, 0, 0},
        //{ 0, 0, 0, 0, 0, 0, 0, 0},
        //{ 0, 0, 0, 0, 0, 1, 0, 0},
        //{ 0, 0, 0, 0, 0, 0, 0, 0},
        //{ 0, 0, 0, 0, 0, 0, 0, 0},
        //{ 1, 0, 0, 0, 1, 1, 0, 1},
        //{ 0, 0, 0, 0, 6, 0, 0, 0},
    };
    for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++) {
        boardData[x][y].pieceOnSquare = (chess::piece)set[x][y];
    }
}

void chess::userData::init() {
    state = boardState::game;
    userSettings.boardColor[0] ={ 255, 255, 255 };
    userSettings.boardColor[1] ={ 118, 150, 210 };
    //userSettings.boardColor[1] ={ 118, 150, 86 };
}

void chess::userData::getMouseBoard_pos(bas::vector2 <float> boardDimensions, bas::vector2 <Uint16> boardPosition) {
    int x = floor(((mouse_pos.x - boardPosition.x) / boardDimensions.x) * 8);
    int y = floor(((mouse_pos.y - boardPosition.y) / boardDimensions.y) * 8);

    if (x >= 0 && x < 8 && y >= 0 && y < 8) {
        mouseBoard_pos.x = x;
        mouseBoard_pos.y = y;
    }
};

void chess::userData::getTileClicked() {
    if (left_click) {
        firstSquare = mouseBoard_pos;
    }
    if (right_click && firstSquare.x != -1) {
        secondSquare = mouseBoard_pos;
    }
}
