#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>



class Button {
private:
    float x, y;
    int  w, h;

protected:
    sf::Texture texture,tic,tac;
    sf::Sprite sprite , state_sp;
    sf::Rect<int> rect;



    void Set_texture(bool side);
public:
    void Set_pos(float x, float y);
    void Draw(sf::RenderWindow& window) const;
    bool Mous_on(float x, float y) const;


    void Set_texture(std::string file);
    void Set_size(int sx, int sy, int w, int h);


    sf::Sprite& Get_sprite();
    std::pair<int, int> Get_pos() const;
    std::pair<int, int> Get_size() const;

    Button(int w, int h, int x, int y, std::string texture_file);
    Button();

};

Button::Button(){}

Button::Button(int w, int h, int x, int y, std::string texture_file) {
    Set_pos(x, y);
    Set_size(0, 0, w, h);
    Set_texture(texture_file);
}

class Cell : public Button {
private:
    char state ='n';


public:
    Cell(int w, int h, int x, int y, std::string texture_file,std::string tic_file, std::string tac_file);
    
    bool Set_state(const char state);

    char Get_state() const;

    void Draw(sf::RenderWindow& window) const;


};



bool Button::Mous_on(float x, float y) const {
    return (x > this->x && x<this->x + h && y>this->y && y < this->y + w);
}



void Button::Draw(sf::RenderWindow& window) const {

    window.draw(sprite);
}


void Button::Set_pos(float x, float y) {
    
    if (&sprite == nullptr) {
        throw std::exception("Trying to set pos to null sprite");
    }

    this->x = x;
    this->y = y;

    sprite.setPosition(x, y);
    state_sp.setPosition(x, y);
}

void Button::Set_size(int sx,int sy, int w, int h) {

    if (&sprite == nullptr) {
        throw std::exception("Trying to set size to null sprite");
    }

    this->w = w;
    this->h = h;

    sprite.setTextureRect(sf::IntRect(sx, sy, w, h));
    state_sp.setTextureRect(sf::IntRect(sx, sy, w, h));

}


void Button::Set_texture(std::string file) {
    if (!texture.loadFromFile(file)) throw std::exception("File is not defined");
    sprite.setTexture(texture);
}

void Button::Set_texture(bool side)
{
    if (side) {
        state_sp.setTexture(tic);
    }
    else {
        state_sp.setTexture(tac);
    }
}
sf::Sprite& Button::Get_sprite() {
    return sprite;
}


std::pair<int, int> Button::Get_pos() const {
    std::pair<int, int> res(x, y);
    return res;
}

std::pair<int, int> Button::Get_size() const {
    std::pair<int, int> res(this->w, this->h);
    return res;
}

bool Cell::Set_state(const char state)
{
    if (this->state != 'n'|| (state != 'X' && state != 'O' && state != 'n')) return 0;

    this->state = state;

    if (state == 'X') {
        Set_texture(1);
    }
    else if (state == 'O'){
        Set_texture(0);
    }

    return 1;
    
}
Cell::Cell(int w, int h, int x, int y, std::string texture_file, std::string tic_file, std::string tac_file)
{
    Set_pos(x, y);
    Set_texture(texture_file);
    Set_size(0, 0, w, h);

    if (!tic.loadFromFile(tic_file)) throw std::exception("Undefined tic texture");
    if (!tac.loadFromFile(tac_file)) throw std::exception("Undefined tac texture");

    Set_state('n');
}

char Cell::Get_state() const {
    return state;
}
void Cell::Draw(sf::RenderWindow& window) const{
    window.draw(sprite);

    if (state == 'n') return;

    window.draw(state_sp);
}




Cell* cells[3][3];

const int cel_size = 226, indent_btw = 15, indent_brd = (800-(cel_size*3 + indent_btw*2))/2;


void Change_state(Cell& cell, char new_state) {
    if (cell.Get_state() != 'n') return;

    cell.Set_state(new_state);
}

void Clear_board() {
    for(int i =0;i<3;++i)
        for (int j = 0; j < 3; ++j) {
            cells[i][j]->Set_state('n');
        }
}

sf::Mouse mouse;

bool move = true;

bool Same_state(Cell& cell1, Cell& cell2, Cell& cell3) {
    char state1 = cell1.Get_state(), state2 = cell2.Get_state(), state3 = cell3.Get_state();
    if (state1 == 'n' or state2 == 'n' or state3 == 'n') return 0;
    if (state1 == state2 && state1 == state3) return 1;
    return 0;
}

bool Is_game_end() {
    for (int i = 0; i < 3; ++i) {
        if (Same_state(*cells[i][0],*cells[i][1],*cells[i][2])) {
            return 1;
        }
    }
    for (int i = 0; i < 3; ++i) {
        if (Same_state(*cells[0][i], *cells[1][i], *cells[2][i])) {
            return 1;
        }
    }
    if ((Same_state(*cells[0][0], *cells[1][1], *cells[2][2])) || (Same_state(*cells[2][0], *cells[1][1], *cells[0][2]))) {
        return 1;
    }

    return 0;
}

bool game = 1;


int main()
{


    std::string bg_file = "textures/background.png", tic_file = "textures/tic.png", tac_file = "textures/tac.png";
    std::string cell_txt_file = "textures/cell.png",tic_txt_file ="", tac_txt_file = "";
    sf::RenderWindow window(sf::VideoMode(800, 800), "TicTacToe");

    for(int i=0;i<3;++i)
    for (int j = 0; j < 3; ++j) {
        Cell* new_cell = new Cell(cel_size, cel_size, indent_brd + (cel_size + indent_btw) * j, indent_brd + (cel_size + indent_btw)*i, cell_txt_file,tic_file,tac_file);
        cells[i][j] = new_cell;
    }
    sf::Texture bg_texture;

    if (!bg_texture.loadFromFile(bg_file)) {
        return EXIT_FAILURE;
    }

    sf::Sprite backgruound(bg_texture);


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                if (!game) continue;
                sf::Vector2i mouse_pos = mouse.getPosition(window);
                int x = mouse_pos.x, y = mouse_pos.y;
                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j) {
                        if (cells[i][j]->Mous_on(x, y)) {
                            if (cells[i][j]->Get_state() != 'n') continue;
                            if (move) cells[i][j]->Set_state('X');
                            else cells[i][j]->Set_state('O');
                            if (Is_game_end()) {
                                game = 0;
                                continue;
                            }
                            move = !move;
                        }
                    }
            }
        }

        window.clear();
        window.draw(backgruound);
        for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            cells[i][j]->Draw(window);
        }
        window.display();
    }

    return EXIT_SUCCESS;
}