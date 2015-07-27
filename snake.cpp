#include "neural_network.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h> 

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define ROW 56 //Number of rows
#define COL 76 //Number of colums
#define SQS 10 //Square side
//SQS*COL must be 760 && SQS*ROW must be 560

enum Direction {UP, RIGHT, DOWN, LEFT};

Direction current_direction = UP, temp_direction = UP;

void draw_grid();
void load_grid();
bool game_state_check();
void print_fit();

class Point{
    public:
            int x, y;

            Point(): x(28), y(38){}

            Point(int xi, int yi): x(xi), y(yi){}

            Point operator+(Point rho)
            {
                return Point(x + rho.x, y + rho.y);
            }

};

std::vector<char> grid(ROW*COL);
std::vector<Point> snake(1, Point());
std::vector<Point> velocity;
Point food;
    
bool done = false, end_simu = false;

int main()
{
    velocity.push_back(Point(-1,0)); //UP
    velocity.push_back(Point(0,1));  //RIGHT
    velocity.push_back(Point(1,0));  //DOWN
    velocity.push_back(Point(0,-1)); //LEFT

    float fps;
    bool automatic, new_pop, visual = true;
    int pop_save_size = 7, pop_size;
    std::string pop_name;
    
    std::cout << "Automatic(0/1)? ";
    std::cin >> automatic;

    if(automatic)
    {
        std::cout << "New population(0/1)? ";
        std::cin >> new_pop;
        std::cout << "Population name: ";
        std::cin >> pop_name;
        //std::cout << "Population saved size: ";
        //std::cin >> pop_save_size; 
        pop_size = pop_save_size*(2*pop_save_size+1);
        //std::cout << "Visual(0/1)? ";
        //std::cin >> visual; 
    }

    std::cout << "Game speed: ";
    std::cin >> fps;
 
    ALLEGRO_DISPLAY *display;

    if(!al_init())
    {
        std::cout << "\nError initiating AL\n";
        return -1;
    }

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);

    if(!display)
    {
        std::cout << "Error criating display\n";
        return -2;
    }

    al_install_keyboard();

    ALLEGRO_KEYBOARD_STATE key_state;
    ALLEGRO_TIMER *timer = al_create_timer(1.0/fps);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_clear_to_color(al_map_rgb(0, 0, 255));

    load_grid();
    
    draw_grid();

    al_start_timer(timer);

    if(!automatic)
    {
        bool loser = false;
        while(!done)
        {
            ALLEGRO_EVENT events;
            al_wait_for_event(event_queue, &events);

            if(events.type == ALLEGRO_EVENT_TIMER)
            {
                //Check direction before change it
                if(current_direction != (temp_direction+2)%4)
                {
                    current_direction = temp_direction;
                }
                loser = game_state_check();
                draw_grid();
            }

            else if(events.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                if(events.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    done = true;
                }
                else if(events.keyboard.keycode == ALLEGRO_KEY_UP)
                {
                        temp_direction = UP;
                }
                else if(events.keyboard.keycode == ALLEGRO_KEY_RIGHT)
                {
                        temp_direction = RIGHT;
                }
                else if(events.keyboard.keycode == ALLEGRO_KEY_DOWN)
                {
                        temp_direction = DOWN;
                }
                else if(events.keyboard.keycode == ALLEGRO_KEY_LEFT)
                {
                        temp_direction = LEFT;
                }
            }

            else if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                done = true;
            }
        }

        if(loser)
        {
            std::cout << "\nGame Over\n"; 
        }
    }

    if(automatic && visual)
    {
        int i = 0, j, current_gen = 0, dist = 1000, total_steps = 0, local_min = 0;
        float auxf, pre_error = 0, max_fitness = 0;
        clock_t t;
        bool abort = false;
        Neuron input(6);
        Neuron aux1(4);
        //Neuron aux1(15);
        Neuron aux2(4);
        std::vector<int> aux_perfect(4);
        Population pop;
        
        if(new_pop)
        {
            generation_0(&pop, input.size(), pop_save_size);
        }
        else
        {
            load_population(pop_name, &pop, &max_fitness, &current_gen, &pop_save_size);
            pop_size = pop_save_size*(2*pop_save_size+1);
        }

    
        while((i < pop_size) && !abort)
        {
            std::cout << "Generation: " << current_gen << "\nIndividual: " << i << "\nMax fitness: " << max_fitness << std::endl;
            std::cout << "==================\n";

            done = false;
            while(!done)
            {
                
                ALLEGRO_EVENT events;
                al_wait_for_event(event_queue, &events);

                if(events.type == ALLEGRO_EVENT_TIMER)
                {
                    //Calculating neural network
                    /*if(dist > abs(snake[0].x - food.x) + abs(snake[0].y - food.y))
                    {
                        dist = abs(snake[0].x - food.x) + abs(snake[0].y - food.y);
                    }*/

                    if(food.x > snake[0].x)
                    {
                        aux_perfect[UP] = 0;    
                        if(current_direction != UP)
                        {
                            aux_perfect[DOWN] = 1;
                        }
                        else
                        {
                            aux_perfect[DOWN] = 0;
                        }
                    }
                    else if(food.x < snake[0].x)
                    {
                        if(current_direction != DOWN)
                        {
                            aux_perfect[UP] = 1;
                        }
                        else
                        {
                            aux_perfect[UP] = 0;
                        }
                        aux_perfect[DOWN] = 0;
                    }
                    else
                    {
                        aux_perfect[UP] = aux_perfect[DOWN] = 0;
                    }

                    if(food.y > snake[0].y)
                    {
                        if(current_direction != LEFT)
                        {
                            aux_perfect[RIGHT] = 1;    
                        }
                        else
                        {
                            aux_perfect[RIGHT] = 0;
                        }
                        aux_perfect[LEFT] = 0;
                    }
                    else if(food.y < snake[0].y)
                    {
                        aux_perfect[RIGHT] = 0;
                        if(current_direction != RIGHT)
                        {
                            aux_perfect[LEFT] = 1;
                        }
                        else
                        {
                            aux_perfect[LEFT] = 0;
                        }
                    }
                    else
                    {
                        aux_perfect[RIGHT] = aux_perfect[LEFT] = 0;
                    }

                    /*input[0] = snake[0].x/56.0;
                    input[1] = snake[0].y/76.0;
                    input[2] = food.x/56.0;
                    input[3] = food.y/76.0;
                    input[4] = input[0] - input[2];
                    input[5] = input[1] - input[3];*/
                    input[2] = input[3] = input[4] = input[5] = 0;

                    input[0] = (snake[0].x - food.x)/56.0;
                    input[1] = (snake[0].y - food.y)/76.0;
                    input[2 + current_direction] = 1;

                    /*for(j = 0; j < pop[i].genome[0].size(); j++)
                    {
                        auxf = alt_sigmoid_function_f(input, (int)current_direction, pop[i].genome[0][j]);
                        aux1[j] = auxf;
                    }*/
                    for(j = 0; j < pop[i].genome[0].size(); j++)
                    {
                        //auxf = sigmoid_function_f(aux1, (int)current_direction, pop[i].genome[1][j]);
                        auxf = sigmoid_function_f(input, pop[i].genome[0][j]);
                        aux2[j] = auxf;
                    }

                    temp_direction = (Direction)0;
                    for(j = 1; j < 4; j++)
                    {
                        if(aux2[j] > aux2[temp_direction])
                        {
                            temp_direction = (Direction)j;
                        }
                    }
                    
                    pre_error += (fabs(aux_perfect[UP] - aux2[0]) + fabs(aux_perfect[RIGHT] - aux2[1]) + fabs(aux_perfect[DOWN] - aux2[2]) + fabs(aux_perfect[LEFT] - aux2[3]));
                    total_steps++;

//                    std::cout << "UP\t" << aux2[0] << " - " << aux_perfect[UP] << "\nRIGHT\t" << aux2[1] << " - " <<  aux_perfect[RIGHT] << "\nDOWN\t" << aux2[2] << " - " << aux_perfect[DOWN] << "\nLEFT\t" << aux2[3] << " - " <<  aux_perfect[LEFT] << "\nFIT\t" << total_steps/pre_error << std::endl << std::endl;

                    //Finished neural network        
            
                    //Check direction before change it
                    if(current_direction != (temp_direction+2)%4)
                    {
                        current_direction = temp_direction;
                    }
                    game_state_check();
                    draw_grid();
                    
                    local_min++;
                    if(local_min > 120*fps)
                    {
                        done = true;
                    }
                }

                else if(events.type == ALLEGRO_EVENT_KEY_DOWN)
                {
                    if(events.keyboard.keycode == ALLEGRO_KEY_A)
                    {
                        done = abort = true;
                    }
                    else if(events.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    {
                        std::cout << "Finishing\n";
                        std::cout << "==================\n";
                        end_simu = true;
                    }
                }

                else if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                {
                    done = abort = true;
                }
            }
            current_direction = UP;
               
            if(pre_error != 0)
            {
                /*if(total_steps/pre_error > 3)
                {
                    if(pop[i].fitness < total_steps/pre_error)
                    {
                        pop[i].fitness = snake.size()*total_steps/pre_error;
                    }
                }
                else
                {*/
                  //  if(pop[i].fitness < total_steps/pre_error)
                    //{
                        pop[i].fitness = total_steps/pre_error;
                    //}
 //               }
            }
            else
            {  
                pop[i].fitness = 0;
            }

            pre_error = 0;
            total_steps = 0;
            local_min = 0;
            dist = 1000;
            if(max_fitness < pop[i].fitness)
            {
                max_fitness = pop[i].fitness;
            }
            snake.resize(1);
            
            i++;
            if(i == pop_size)
            {
                next_generation(&pop, input.size(), pop_save_size);
                current_gen++;
            }
            if(!end_simu)
            {
                i = i%pop_size;
            }

            if((current_gen%25 == 0 || end_simu) && i == pop_size)
            {
                save_population(pop_name, &pop, max_fitness, current_gen, pop_save_size);
                std::cout << "Population Saved\n";
                std::cout << "==================\n";
            }

            load_grid();

        }
    }
    
    if(automatic && !visual)
    {
        //No visual feedback
        std::cout << "TOO LAZY TO MAKE THIS CASE YET\n";
    }
return 0;
}

void draw_grid()
{
    int i, j;

    al_draw_filled_rectangle(20, 20, 780, 580, al_map_rgb(0,0,0));

    for(i = 0; i < ROW; i++)
    {
        for(j=0; j < COL; j++)
        {
            if(grid[i*COL + j] != 'E')
            {
                if(grid[i*COL + j] == 'B')
                {
                    al_draw_filled_rectangle(20 + SQS*j, 20 + SQS*i, 20 + SQS*(j+1), 20 + SQS*(i+1), al_map_rgb(255, 255, 255));
                }
                else if(grid[i*COL + j] == 'H')
                {
                    al_draw_filled_rectangle(20 + SQS*j, 20 + SQS*i, 20 + SQS*(j+1), 20 + SQS*(i+1), al_map_rgb(0, 255, 255));
                }
                else if(grid[i*COL + j] == 'F')
                {
                    al_draw_filled_rectangle(20 + SQS*j, 20 + SQS*i, 20 + SQS*(j+1), 20 + SQS*(i+1), al_map_rgb(0, 255, 0));
                }
            }
        }
    }
    al_flip_display();
}

void load_grid()
{
    srand(time(NULL));
    //In the future it will load other kinds of map

    //B - blocked. walls and snake's body
    //E - empty
    //H - snake's head
    //F - food
    int i, j;
    for(i = 0; i < ROW; i++)
    {
        for(j=0; j < COL; j++)
        {
            //Change this part to load the maps
            if(i == 0 || i == (ROW - 1) || j == 0 || j == (COL - 1))
            {
                grid[i*COL + j] = 'B';
            }
            else
            {
                grid[i*COL + j] = 'E';
            }
        }
    }
    
    //Choose starting point
    while(grid[i*COL + j] != 'H')
    {
        i = rand()%ROW;
        j = rand()%COL;

        if(grid[i*COL + j] == 'E')
        {
            snake[0] = Point(i, j);
            grid[i*COL + j] = 'H'; 
        }
    }
    
    //Choose food point
    while(grid[i*COL + j] != 'F')
    {
        i = rand()%ROW;
        j = rand()%COL;

        if(grid[i*COL + j] == 'E')
        {
            grid[i*COL + j] = 'F'; 
            food = Point(i, j);
        }
    }

}

void walk(Point fp)
{
    int i;
    grid[snake[(snake.size()-1)].x*COL + snake[(snake.size()-1)].y] = 'E';
    for(i = (snake.size() - 1); i > 0; i--)
    {
        snake[i] = snake[i-1];
        grid[snake[i].x*COL + snake[i].y] = 'B';
    }
    snake[0] = fp;
    grid[snake[0].x*COL + snake[0].y] = 'H';
}

bool game_state_check()
{
    Point fp = snake[0]+velocity[current_direction]; //Future point

    if(grid[fp.x*COL + fp.y] == 'E')
    {
        walk(fp);
        return false;
    }

    else if(grid[fp.x*COL + fp.y] == 'F')
    {
        walk(fp);

        snake.push_back(snake[(snake.size()-1)]);

        //Next food point
        int i = 0, j = 0;
        while(grid[i*COL + j] != 'F')
        {
            i = rand()%ROW;
            j = rand()%COL;

            if(grid[i*COL + j] == 'E')
            {
                grid[i*COL + j] = 'F'; 
                food = Point(i, j);
            }
        }
        return false;
    }

    else if(grid[fp.x*COL + fp.y] == 'B')
    {
        done = true;
        return true;
    }
}

void print_fit()
{
    std::cout << "Fitness: " << snake.size();
    std::cout << "\n==================\n";
}
