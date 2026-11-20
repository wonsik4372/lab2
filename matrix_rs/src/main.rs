// Lab2-2: Vec<T>를 이용한 동적 행렬 덧셈 
use std::io::{self, Read, Write};

// 행 우선으로 1차원 Vec에 저장하는 행렬 
struct Matrix{
    rows: usize,
    cols: usize,
    data: Vec<i64>,
}


fn new_matrix(rows: usize, cols: usize) -> Matrix{
    // 필요한 만큼 힙에 할당 (C: malloc)
    Matrix {rows, cols, data: vec![0; rows * cols] }
}

fn print_matrix(m: &Matrix, name: &str){
    println!("[{}] ({}x{})", name, m.rows, m.cols);
    for r in 0..m.rows{
        for c in 0..m.cols{
            print!("{:4} ", m.data[r * m.cols + c]);
        }
        println!();
    }
}

// 표준 입력을 읽어 공백 단위 토큰으로 나눈 정수 목록 생성 
fn read_all_numbers() -> Vec<i64> {
    let mut text = String::new();
    io::stdin().read_to_string(&mut text).unwrap();
    let mut nums: Vec<i64> = Vec::new();
    for tok in text.split_whitespace() {
        match tok.parse::<i64>() {
            Ok(n) => nums.push(n),
            Err(_) => {
                eprintln!("정수가 아닌 값: {}", tok);
                std::process::exit(1);
            }
        }
    }
    nums
}

// nums의 pos 위치부터 행렬 하나를 읽음 -> 위치를 pos에 저장
fn read_matrix(nums: &Vec<i64>, pos: &mut usize, name: &str) -> Matrix {
    if *pos + 2 > nums.len() {
        eprintln!("행렬 {}: 행과 열 입력 부족", name);
        std::process::exit(1);
    }

    let rows = nums[*pos];
    let cols = nums[*pos + 1];
    *pos += 2;
    if rows <= 0 || cols <= 0 {
        eprintln!("행과 열은 1이상이어야 함");
        std::process::exit(1);
    }

    let mut m = new_matrix(rows as usize, cols as usize);
    let total = m.rows * m.cols;
    if *pos + total > nums.len() {
        eprintln!("행렬 {}: 원소가 부족힘  ({}개 필요)", name, total);
        std::process::exit(1);
    }

    for i in 0..total {
        m.data[i] = nums[*pos + i];
    }
    *pos += total;
    m
}

fn main() {
    print!("입력 (행 열 원소들.. 을 A, B 순서로 ): ");
    io::stdout().flush().unwrap();
    let nums = read_all_numbers();
    let mut pos: usize = 0;
    let a = read_matrix(&nums, &mut pos, "A");
    let b = read_matrix(&nums, &mut pos, "B");

    // 크기가 다르면 덧셈 불가 
    if a.rows != b.rows || a.cols != b.cols {
        eprintln!("크기 불일치: {}x{} + {}x{}", a.rows, a.cols, b.rows, b.cols);
        std::process::exit(1);
    }

    let mut c = new_matrix(a.rows, a.cols);
    for i in 0..c.data.len(){
        c.data[i] = a.data[i] + b.data[i];
    }

    println!();
    print_matrix(&a, "A");
    print_matrix(&b, "B");
    print_matrix(&c, "A + B");

    // a,b,c 는 main 종료 시 해제 
}

    






