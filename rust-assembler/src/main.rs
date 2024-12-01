use std::env;
use std::fmt::format;
use std::fs::File;
use std::io::{self, BufRead, Write};
use std::{
    collections::{hash_map, HashMap},
    process::{exit, Command},
};

use regex::Regex;

fn main() {
    let args: Vec<String> = env::args().collect();
    println!("{:?}", args);
    let input_path = args.get(1).unwrap();
    let output_path = args.get(2).unwrap();
    let command_regex =
        Regex::new(r"^(LOAD|STORE|CALL|BR|BREQ|BRGE|BRLT|ADD|SUB|MUL|DIV)\s+([=@$])?([0-9]+)\s*")
            .unwrap();
    let halt_regex = Regex::new(r"^HALT\s*$").unwrap();
    let file_content = read_from_file(input_path);
    let mut write_context = File::create(output_path.as_str()).unwrap();
    let mut instruction_all = String::from(format!("{:0>10b}", file_content.len()));
    for line in file_content {
        let (command, address_mode, address) =
            verify_and_capture(&line, &command_regex, &halt_regex);
        // println!("all {}, command {}, address_mode {}, address {}", all, command, address_mode, address);
        let instruction = make_assembly_inst(
            String::from(command),
            String::from(address_mode),
            String::from(address),
        );
        println!("{}", instruction);
        instruction_all.push_str(instruction.as_str());
    }
    write_context.write_all(instruction_all.as_bytes()).unwrap();
}

fn make_assembly_inst(command: String, mode: String, address: String) -> String {
    let command_map: HashMap<&str, &str> = HashMap::from([
        ("HALT", "0000"),
        ("LOAD", "0001"),
        ("STORE", "0010"),
        ("CALL", "0011"),
        ("BR", "0100"),
        ("BREQ", "0101"),
        ("BRGE", "0110"),
        ("BRLT", "0111"),
        ("ADD", "1000"),
        ("SUB", "1001"),
        ("MUL", "1010"),
        ("DIV", "1011"),
    ]);
    let mode_map: HashMap<&str, &str> = HashMap::from([
        ("", "00"), //direct
        ("=", "01"),
        ("$", "10"),
        ("@", "11"),
    ]);
    println!("{}", command);
    let mut assembly_str = String::from("");
    let binary_code = command_map.get(command.as_str()).unwrap();
    if *binary_code == "0000" {
        assembly_str.push_str(&binary_code.repeat(4));
        println!("Halt command, exiting...");
        return assembly_str;
    }
    let binary_mode = mode_map.get(mode.as_str()).unwrap();
    let binary_address = format!("{:0>10b}", address.parse::<u32>().unwrap());
    assembly_str.push_str(format!("{binary_code}{binary_mode}{binary_address}").as_str());
    assembly_str
}
fn read_from_file(path: &str) -> Vec<String> {
    let result = File::open(path);
    let reader = io::BufReader::new(result.unwrap());
    let mut lines = Vec::new();
    for line in reader.lines() {
        lines.push(line.unwrap());
    }
    lines
}
fn verify_and_capture<'a>(
    test_str: &'a String,
    command_regex: &Regex,
    halt_regex: &Regex,
) -> (&'a str, &'a str, &'a str) {
    if command_regex.is_match(&test_str) {
        let caps = command_regex.captures(&test_str).unwrap();
        let all = caps.get(0).map_or("", |m| m.as_str());
        let command = caps.get(1).map_or("", |m| m.as_str());
        let address_mode = caps.get(2).map_or("", |m| m.as_str());
        let address = caps.get(3).map_or("", |m| m.as_str());
        (command, address_mode, address)
    } else if halt_regex.is_match(&test_str) {
        let caps = halt_regex.captures(&test_str).unwrap();
        let command = caps.get(0).map_or("", |m| m.as_str());
        (command, "", "")
    } else {
        eprintln!("error verifying or capturing");
        exit(1);
    }
}
