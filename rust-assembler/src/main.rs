use std::env;
use std::fs::File;
use std::io::{self, BufRead, Write};
use std::{collections::HashMap, process::exit};

use regex::Regex;

fn main() {
    let args: Vec<String> = env::args().collect();
    let input_path = args.get(1).unwrap();
    let output_path = args.get(2).unwrap();
    println!("Reading assembly code from {input_path}...");
    let command_regex = Regex::new(
        r"^(LOAD|STORE|CALL|BR|BREQ|BRGE|BRLT|ADD|SUB|MUL|DIV|ORG|END)\s+([=@$])?([0-9]+)\s*",
    )
    .unwrap();
    let halt_regex = Regex::new(r"^HALT\s*$").unwrap();
    let data_regex = Regex::new(r"^DATA\s*(([0-9]+)((,[0-9]+))*)\s*").unwrap();
    let file_content = read_from_file(input_path);
    let mut write_context = File::create(output_path.as_str()).unwrap();
    let mut instruction_all = String::from("");
    let mut full_instruction = String::from("");
    let mut commands_count = 0;
    let mut org_value: String = format!("{:0>10b}", 0);
    let mut end_value: String = format!("{:0>10b}", 0);
    let mut command_vec: Vec<Vec<String>> = Vec::new();
    let mut is_first_org = true;
    for line in file_content {
        match data_regex.is_match(&line) {
            true => {
                // ORG 0
                // LOAD 20
                // BREQ 9
                // MUL 21
                // STORE 21
                // LOAD 20
                // SUB =1
                // STORE 21
                // BRGE 0
                // HALT
                // ORG 20
                // DATA 5
                // DATA 1
                // END 0
                // END - ORG - COUNT - INST[] - ORG - COUNT - INST[]
                let (binary_data, data_length) = capture_and_parse_data(&line, &data_regex);
                instruction_all.push_str(&binary_data);
                commands_count += data_length;
            }
            _ => {
                let mut command_vec: Vec<String> = Vec::new();
                while is_first_org {
                    let (command, address_mode, address) =
                        verify_and_capture(&line, &command_regex, &halt_regex);
                    command_vec.push(command.to_string());
                    if command == "ORG" {
                        is_first_org = false
                    }
                }

                // if command == "ORG" {
                //     if address_mode != "" {
                //         panic!("Address mode cannot be {} for ORG", address_mode);
                //     }
                //     org_value = format!("{:0>10b}", address.parse::<i32>().unwrap());
                //     continue;
                // } else if command == "END" {
                //     if address_mode != "" {
                //         panic!("Address mode cannot be {} for END", address_mode);
                //     }
                //     end_value = format!("{:0>10b}", address.parse::<i32>().unwrap());
                //     continue;
                // }
                // commands_count += 1;
                // let instruction = make_assembly_inst(
                //     String::from(command),
                //     String::from(address_mode),
                //     String::from(address),
                // );
                // instruction_all.push_str(instruction.as_str());
            }
        }
    }
    let full_instruction = format!(
        "{}{}{:0>10b}{}",
        end_value, org_value, commands_count, instruction_all
    );
    write_context
        .write_all(full_instruction.as_bytes())
        .unwrap();
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
    let mut assembly_str = String::from("");
    let binary_code = command_map.get(command.as_str()).unwrap();
    if *binary_code == "0000" {
        assembly_str.push_str(&binary_code.repeat(4));
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
        let line = line.unwrap();
        if !line.trim().is_empty() {
            lines.push(line);
        }
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
        let _all = caps.get(0).map_or("", |m| m.as_str());
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
fn capture_and_parse_data<'a>(data: &'a String, data_regex: &Regex) -> (String, usize) {
    let caps = data_regex.captures(&data).unwrap();
    let parsed_data: Vec<&'a str> = caps.get(1).map_or("", |m| m.as_str()).split(",").collect();
    let binary_data: Vec<String> = parsed_data
        .iter()
        .map(|n| format!("{:0>16b}", n.parse::<u32>().unwrap()))
        .collect();
    (binary_data.join(""), binary_data.len())
}
