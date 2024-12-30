use std::env;
use std::fs::File;
use std::io::{self, BufRead, Write};
use std::{collections::HashMap, process::exit};

use regex::Regex;
type Captured = Vec<(String, String, String)>;

fn main() {
    let args: Vec<String> = env::args().collect();
    let input_path = args.get(1).unwrap();
    let output_path = args.get(2).unwrap();
    println!("Reading assembly code from {input_path}...");
    let mut write_context = File::create(output_path.as_str()).unwrap();
    let file_content = read_from_file(input_path);

    let command_array = create_command_array(file_content);
    let bin = command_array_to_bin(command_array);
    write_context
        .write_all(bin.as_bytes())
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
fn verify_and_capture(
    test_str: &String,
) -> (&str, &str, &str) {
    let command_regex = Regex::new(
        r"^([A-Z]+)?\s*(LOAD|STORE|CALL|BR|BREQ|BRGE|BRLT|ADD|SUB|MUL|DIV|ORG|END)\s+([=@$])?(([0-9]+)|([A-Z]+))\s*",
    )
        .unwrap();

    let halt_regex = Regex::new(r"^([A-Z]+)?\s*HALT\s*$").unwrap();
    println!("{}",test_str);
    println!("{}",command_regex.is_match(&test_str));
    println!("{}",halt_regex.is_match(&test_str));
    if command_regex.is_match(&test_str) {
        let caps = command_regex.captures(&test_str).unwrap();
        let _all = caps.get(0).map_or("", |m| m.as_str());
        println!("{}", _all);
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

// [(метка, имя_коанды, способ_адресации, поле_адреса)]

// START LOAD =1
// ("START","LOAD","=","1")
// STORE LSTFIB
// ("","STORE","","LSTFIB")
// DATA 1,2,3
// ("","DATA","","1")
// ("","DATA","","2") ...

fn create_command_array(file_content: Vec<String>) -> Vec<Captured> {

    let data_regex = Regex::new(r"^([A-Z]+)?\s+DATA\s*(([0-9]+)((,[0-9]+))*)\s*").unwrap();

    let mut command_vec: Captured = Vec::new();
    for line in file_content {
        match data_regex.is_match(&line) {
            true => {
                //TODO: fix data with comma 
                let (binary_data, data_length) = capture_and_parse_data(&line, &data_regex);
                command_vec.push(("DATA".to_string(), "".to_string(), binary_data));
            }
            _ => {
                let (command, address_mode, address) =
                    verify_and_capture(&line);
                command_vec.push((command.to_string(), address_mode.to_string(), address.to_string()));
            }
        }
    }
    split_by_org(command_vec)
}
fn command_array_to_bin(data: Vec<Captured>) -> String {
    let mut instructions = Vec::new();
    let mut end_value: String = format!("{:0>10b}", 0);
    for org_split in data {
        let mut org_inst = String::new();
        let mut org_value: String = format!("{:0>10b}", 0);
        let size = format!("{:0>10b}", commands_count(&org_split));
        for capture in org_split {
            if capture.0 == "ORG" {
                org_value = format!("{:0>10b}", capture.2.parse::<i32>().unwrap());
                continue;
            }
            else if capture.0 == "END" {
                end_value = format!("{:0>10b}", capture.2.parse::<i32>().unwrap());
            }
            else if capture.0 == "DATA" {
                org_inst.push_str(&capture.2);
                continue;
            }
            else  {
                let inst = make_assembly_inst(capture.0, capture.1, capture.2);
                org_inst.push_str(&*inst);
            }
        }
        org_inst = format!("{}{}{}", org_value, size, org_inst);
        instructions.push(org_inst);
    }
    // println!("{:?}", end_value);
    // println!("{:?}", instructions);
    let mut full_inst = instructions.join("");
    full_inst = format!("{}{}", end_value, full_inst);
    full_inst
}
fn commands_count(data: &Captured) -> usize {
    let mut size = 0;
        for capture in data {
            if capture.0 == "END" || capture.0 == "ORG"{
                continue;
            } else {
                size+=1;
            }
        }
    size
}
fn split_by_org(data: Captured) -> Vec<Captured> {
    let mut result: Vec<Captured> = Vec::new();
    let mut org_count = 0;
    let mut temp_vec: Captured = Vec::new();
    println!("{:?}", data);
    for capture in data {
        if capture.0 == "ORG" {
            org_count += 1;
        }
        if org_count > 1 && capture.0 == "ORG" {
            result.push(temp_vec);
            temp_vec = Vec::new();
        }
        temp_vec.push(capture);
    }
    result.push(temp_vec);
    result
}