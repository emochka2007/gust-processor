use std::env;
use std::fs::File;
use std::io::{self, BufRead, Write};
use std::{collections::HashMap};
use crate::asm::{command_array_to_bin, create_command_array};
mod asm;
mod capture;
mod constants;
mod types;


fn main() {
    let args: Vec<String> = env::args().collect();
    let input_path = args.get(1).unwrap();
    let output_path = args.get(2).unwrap();
    let mut var_map: HashMap<String, i32> = HashMap::new();
    println!("Reading assembly code from {input_path}...");
    let mut write_context = File::create(output_path.as_str()).unwrap();
    let file_content = read_from_file(input_path);
    let command_array = create_command_array(file_content, &mut var_map);
    let bin = command_array_to_bin(command_array);
    write_context
        .write_all(bin.as_bytes())
        .unwrap();
}


fn read_from_file(path: &str) -> Vec<String> {
    let result = File::open(path);
    let reader = io::BufReader::new(result.unwrap());
    let mut lines = Vec::new();
    for line in reader.lines() {
        let line = line.unwrap();
        if !line.trim().is_empty() {
            lines.push(line);
        }
    }
    lines
}



