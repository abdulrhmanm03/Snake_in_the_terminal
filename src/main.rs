use crossterm::{
    cursor::MoveTo,
    event::{self, Event, KeyCode},
    execute,
    terminal::{self, Clear, ClearType},
};
use snake_game::{Direction, Food, Map, Snake};
use std::{io::stdout, process::exit, time::Duration};

fn main() {
    let width = 60;
    let height = 30;

    let mut score = 0;
    let mut snake = Snake::new(4, width, height);
    let mut food = Food::new(width, height);
    let map = Map::new(width, height, &snake, &food);

    draw_game(&map, score);

    terminal::enable_raw_mode().unwrap();
    let mut dir = snake.dir;
    loop {
        if event::poll(Duration::from_millis(100)).unwrap() {
            if let Event::Key(key_event) = event::read().unwrap() {
                match key_event.code {
                    KeyCode::Char('q') => end_game(),
                    KeyCode::Char(c) => {
                        dir = map_input_to_direction(c, dir);
                    }
                    _ => {}
                }
            }
        }
        dir = snake.move_snake(dir);
        if snake.did_we_hit_a_wall(width, height) || snake.did_we_hit_ourself() {
            end_game()
        }
        if snake.did_we_hit_food(&food) {
            score += 1;
            snake.add_cell();
            food = Food::new(width, height);
        }
        let map = Map::new(width, height, &snake, &food);
        clean_screan();
        draw_game(&map, score);
    }
}

fn clean_screan() {
    let mut stdout = stdout();
    execute!(stdout, MoveTo(0, 0), Clear(ClearType::All)).unwrap();
}

fn draw_game(map: &Map, score: u32) {
    terminal::disable_raw_mode().unwrap();
    println!();
    println!("k to move up, j to move down, l to move right, h to move left");
    println!("q to quit");
    println!();

    map.draw_map();
    println!();
    println!("Score: {score}");
    terminal::enable_raw_mode().unwrap();
}

fn map_input_to_direction(c: char, current_dir: Direction) -> Direction {
    match c {
        'k' => Direction::Up,
        'j' => Direction::Down,
        'h' => Direction::Left,
        'l' => Direction::Right,
        _ => current_dir,
    }
}

fn end_game() {
    terminal::disable_raw_mode().unwrap();
    println!();
    println!("==========================Game over==========================");
    println!();
    exit(1);
}
