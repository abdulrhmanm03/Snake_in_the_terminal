use rand::Rng;

pub struct Map {
    grid: Vec<Vec<CellContent>>,
}

impl Map {
    pub fn new(width: usize, height: usize, snake: &Snake, food: &Food) -> Map {
        let mut grid = vec![vec![CellContent::Empty; width]; height];

        for i in 0..width {
            grid[0][i] = CellContent::Border;
            grid[height - 1][i] = CellContent::Border;
        }

        for row in grid.iter_mut() {
            row[0] = CellContent::Border;
            row[width - 1] = CellContent::Border;
        }

        for cell in &snake.body {
            grid[cell.y][cell.x] = CellContent::SnakeFragment;
        }

        grid[food.cell.y][food.cell.x] = CellContent::Food;

        Map { grid }
    }

    pub fn draw_map(&self) {
        for row in &self.grid {
            for cell in row {
                cell.print_cell();
            }
            println!();
        }
    }
}

#[derive(Clone)]
enum CellContent {
    Empty,
    Border,
    Food,
    SnakeFragment,
}

impl CellContent {
    fn print_cell(&self) {
        match self {
            CellContent::Empty => print!(" "),
            CellContent::Border => print!("#"),
            CellContent::Food => print!("@"),
            CellContent::SnakeFragment => print!("O"),
        }
    }
}

struct Cell {
    x: usize,
    y: usize,
}

impl Cell {
    fn new(x: usize, y: usize) -> Cell {
        Cell { x, y }
    }
    pub fn is_the_same_pos(&self, other: &Cell) -> bool {
        if self.x == other.x && self.y == other.y {
            return true;
        }
        false
    }
}

pub struct Food {
    cell: Cell,
}

impl Food {
    pub fn new(width: usize, height: usize) -> Food {
        let mut rng = rand::thread_rng();

        let x = rng.gen_range(1..width - 2);
        let y = rng.gen_range(1..height - 2);

        let cell = Cell::new(x, y);
        Food { cell }
    }
}

pub struct Snake {
    pub dir: Direction,
    body: Vec<Cell>,
}

impl Snake {
    pub fn new(length: usize, width: usize, height: usize) -> Snake {
        let head_x = width / 2;
        let head_y = height / 2;

        let mut snake = vec![];
        for i in 0..length {
            let x = head_x - i;
            let y = head_y;
            snake.push(Cell::new(x, y));
        }
        let dir = Direction::Right;

        Snake { dir, body: snake }
    }

    pub fn move_snake(&mut self, mut dir: Direction) -> Direction {
        match (&self.dir, &mut dir) {
            (Direction::Up, Direction::Down) => dir = self.dir,
            (Direction::Down, Direction::Up) => dir = self.dir,
            (Direction::Left, Direction::Right) => dir = self.dir,
            (Direction::Right, Direction::Left) => dir = self.dir,
            _ => {}
        }

        let mut x = self.body[0].x;
        let mut y = self.body[0].y;

        match dir {
            Direction::Up => y -= 1,
            Direction::Down => y += 1,
            Direction::Left => x -= 1,
            Direction::Right => x += 1,
        }

        for cell in self.body.iter_mut() {
            let temp_x = cell.x;
            let temp_y = cell.y;
            cell.x = x;
            cell.y = y;
            x = temp_x;
            y = temp_y
        }

        self.dir = dir;
        self.dir
    }

    pub fn did_we_hit_a_wall(&self, width: usize, height: usize) -> bool {
        let head = &self.body[0];
        if head.x == 0 || head.y == 0 || head.x == width - 1 || head.y == height - 1 {
            return true;
        }
        false
    }

    pub fn did_we_hit_ourself(&self) -> bool {
        let head = &self.body[0];
        for cell in &self.body[1..] {
            if head.is_the_same_pos(cell) {
                return true;
            }
        }
        false
    }

    pub fn add_cell(&mut self) {
        let tail = &self.body[self.body.len() - 1];
        let new_cell = Cell::new(tail.x, tail.y);
        self.body.push(new_cell);
    }

    pub fn did_we_hit_food(&self, food: &Food) -> bool {
        let head = &self.body[0];
        head.is_the_same_pos(&food.cell)
    }
}

#[derive(Clone, Copy)]
pub enum Direction {
    Up,
    Down,
    Left,
    Right,
}
