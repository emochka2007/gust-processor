use std::env;
use std::fs::File;
use std::io::{self, BufRead, Error, Write};
use std::{collections::HashMap, process::exit};
use std::sync::atomic::{AtomicU16, Ordering};
use phf::phf_map;
use regex::Regex;

//TODO !!!
// move tuples to structs
// multiple orgs
/**
- "1" or "0", if command => "1",
- command name
- address mode
- address/value
*/
#[derive(Debug)]
struct LineParsed {
    point: String,
    command_name: String,
    address_mode: String,
    address_value: String,
}

type Captured = Vec<LineParsed>;
static ORG_ADDRESS: AtomicU16 = AtomicU16::new(0);
static COMMAND_LENGTH: AtomicU16 = AtomicU16::new(0);
static COMMAND_MAP: phf::Map<&'static str, &str> = phf_map! {
    "HALT" => "0000",
    "LOAD" => "0001",
    "STORE" => "0010",
    "CALL" => "0011",
    "BR" => "0100",
    "BREQ" => "0101",
    "BRGE" => "0110",
    "BRLT" => "0111",
    "ADD" => "1000",
    "SUB" => "1001",
    "MUL" => "1010",
    "DIV" => "1011",
};

static MODE_MAP: phf::Map<&'static str, &str> = phf_map! {
    "" => "00", //direct
    "=" => "01",
    "$" =>  "10",
    "@" => "11",
};

fn main() {
    let args: Vec<String> = env::args().collect();
    let input_path = args.get(1).unwrap();
    let output_path = args.get(2).unwrap();
    let mut var_map: HashMap<String, i32> = HashMap::new();
    println!("Reading assembly code from {input_path}...");
    let mut write_context = File::create(output_path.as_str()).unwrap();
    let file_content = read_from_file(input_path);

    let command_array = create_command_array(file_content, &mut var_map);
    // println!("{:?}", command_array);
    let bin = command_array_to_bin(command_array, var_map);
    write_context
        .write_all(bin.as_bytes())
        .unwrap();
}

fn make_assembly_inst(command: String, mode: String, address: String, var_map: &HashMap<String, i32>) -> String {
    let mut assembly_str = String::from("");
    let mut binary_code = "";
    if let Some(code) = COMMAND_MAP.get(command.as_str()) {
        if *code == "0000" {
            assembly_str.push_str(&code.repeat(4));
            return assembly_str;
        }
        binary_code = code;
    }
    println!("{command} {mode} {address}");
    let binary_mode = MODE_MAP.get(mode.as_str()).unwrap_or_else(|| {
        eprintln!("Error finding mode_map {mode}");
        exit(1);
    });
    let binary_address = format!("{:0>10b}", address.parse::<u32>().unwrap_or_else(|e| {
        eprintln!("Error converting address to u32: {address} {:?}", e);
        exit(1);
    }));
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
fn inc_command_len(command: &str) -> Result<(), Error> {
    if let Some(_) = COMMAND_MAP.get(&command) {
        COMMAND_LENGTH.fetch_add(1, Ordering::SeqCst);
        Ok(())
    } else {
        Ok(())
    }
}
fn verify_and_capture(
    test_str: &String,
    mut var_map: &mut HashMap<String, i32>,
) -> LineParsed {
    let command_regex = Regex::new(
        r"^([A-Z]+)?\s+(LOAD|STORE|CALL|BR|BREQ|BRGE|BRLT|ADD|SUB|MUL|DIV|ORG|END)\s+([=@$])?(([0-9]+)|([A-Z]+))\s*",
    )
        .unwrap();

    let halt_regex = Regex::new(r"^([A-Z]+)?\s*(HALT)\s*$").unwrap();
    let var_regex = Regex::new(r"([A-Z]+)\s+([0-9]+)").unwrap();
    if command_regex.is_match(&test_str) {
        let caps = command_regex.captures(&test_str).unwrap();
        let _all = caps.get(0).map_or("", |m| m.as_str());
        let variable = caps.get(1).map_or("", |m| m.as_str());
        let command = caps.get(2).map_or("", |m| m.as_str());
        let address_mode = caps.get(3).map_or("", |m| m.as_str());
        let address = caps.get(4).map_or("", |m| m.as_str());
        var_map.insert(variable.to_string(), COMMAND_LENGTH.load(Ordering::SeqCst) as i32);
        // inc_command_len(command).expect("TODO: panic message");
        LineParsed {
            point: variable.to_string(),
            command_name: command.to_string(),
            address_mode: address_mode.to_string(),
            address_value: address.to_string(),
        }
    } else if halt_regex.is_match(&test_str) {
        let caps = halt_regex.captures(&test_str).unwrap();
        let variable = caps.get(1).map_or("", |m| m.as_str());
        let command = caps.get(2).map_or("", |m| m.as_str());
        var_map.insert(variable.to_string(), COMMAND_LENGTH.load(Ordering::SeqCst) as i32);
        // inc_command_len(command).expect("TODO: panic message");
        LineParsed {
            point: variable.to_string(),
            command_name: command.to_string(),
            address_mode: "".to_string(),
            address_value: "".to_string(),
        }
    } else if var_regex.is_match(&test_str) {
        let caps = var_regex.captures(&test_str).unwrap();
        let var = caps.get(1).map_or("", |m| m.as_str());
        let var_value = caps.get(2).map_or("", |m| m.as_str()).parse::<i32>().unwrap();
        var_map.insert(var.to_string(), var_value);
        LineParsed {
            point: "0".to_string(),
            command_name: var.to_string(),
            address_mode: "".to_string(),
            address_value: "".to_string(),
        }
    } else {
        eprintln!("error verifying or capturing {test_str}");
        exit(1);
    }
}
fn capture_and_parse_data<'a>(data: &'a String, data_regex: &Regex, var_map: &mut HashMap<String, i32>) -> LineParsed {
    let caps = data_regex.captures(&data).unwrap();
    let parsed_var: Vec<&'a str> = caps.get(1).map_or("", |m| m.as_str()).split(",").collect();
    let parsed_data_value: Vec<&'a str> = caps.get(2).map_or("", |m| m.as_str()).split(",").collect();
    let binary_data: Vec<String> = parsed_data_value
        .iter()
        .map(|n| format!("{:0>16b}", n.parse::<u32>().unwrap()))
        .collect();
    var_map.insert(parsed_var.join(""), COMMAND_LENGTH.load(Ordering::SeqCst) as i32);
    // inc_command_len("DATA").unwrap();
    // (binary_data.join(""), binary_data.len())
    LineParsed {
        point: parsed_var.join(""),
        command_name: "DATA".to_string(),
        address_mode: "".to_string(),
        address_value: binary_data.join(""),
    }
}

fn create_command_array(file_content: Vec<String>, map: &mut HashMap<String, i32>) -> Vec<Captured> {
    let data_regex = Regex::new(r"^([A-Z]+)?\s+DATA\s*(([0-9]+)((,[0-9]+))*)\s*").unwrap();

    let mut command_vec: Captured = Vec::new();
    for line in file_content {
        match data_regex.is_match(&line) {
            true => {
                //TODO: fix data with comma
                let line_parsed = capture_and_parse_data(&line, &data_regex, map);
                println!("{:?}", line_parsed);
                command_vec.push(line_parsed);
            }
            _ => {
                let line_parsed =
                    verify_and_capture(&line, map);
                if line_parsed.command_name != "" {
                    command_vec.push(line_parsed);
                }
            }
        }
    }
    let mut split_vec = split_by_org(command_vec, map);

    for split in &mut split_vec {
        replace_vars(split, &map);
    }
    split_vec
}
fn command_array_to_bin(data: Vec<Captured>, command_map: HashMap<String, i32>) -> String {
    let mut instructions = Vec::new();
    let mut end_value: String = format!("{:0>10b}", 0);
    for org_split in data {
        let mut org_inst = String::new();
        let mut org_value: String = format!("{:0>10b}", 0);
        let size = format!("{:0>10b}", commands_count(&org_split));
        for capture in org_split {
            if capture.command_name == "ORG" {
                org_value = format!("{:0>10b}", capture.address_value.parse::<i32>().unwrap());
                continue;
            } else if capture.command_name == "END" {
                end_value = format!("{:0>10b}", capture.address_value.parse::<i32>().unwrap());
            } else if capture.command_name == "DATA" {
                org_inst.push_str(&capture.address_value);
                continue;
            } else if COMMAND_MAP.get(&capture.command_name).is_some() {
                let inst = make_assembly_inst(capture.command_name, capture.address_mode, capture.address_value, &command_map);
                org_inst.push_str(&*inst);
            }
        }
        org_inst = format!("{}{}{}", org_value, size, org_inst);
        instructions.push(org_inst);
    }
    let mut full_inst = instructions.join("");
    full_inst = format!("{}{}", end_value, full_inst);
    full_inst
}
fn commands_count(data: &Captured) -> usize {
    let mut size = 0;
    for capture in data {
        if COMMAND_MAP.get(&capture.command_name).is_some() || capture.command_name == "DATA" {
            size += 1;
        }
    }
    size
}
fn split_by_org(data: Captured, var_map: &mut HashMap<String, i32>) -> Vec<Captured> {
    let mut result: Vec<Captured> = Vec::new();
    let mut org_count = 0;
    let mut temp_vec: Captured = Vec::new();
    for capture in data {
        if capture.command_name == "ORG" {
            ORG_ADDRESS.store(capture.address_value.parse::<u16>().unwrap(), Ordering::SeqCst);
            org_count += 1;
        }
        if let Some(mut point_value) = var_map.get_mut(&capture.point) {
            *point_value = (COMMAND_LENGTH.load(Ordering::SeqCst) + ORG_ADDRESS.load(Ordering::SeqCst)) as i32;
        }
        if org_count > 1 && capture.command_name == "ORG" {
            COMMAND_LENGTH.store(0, Ordering::SeqCst);
            result.push(temp_vec);
            temp_vec = Vec::new();
        }
        inc_command_len(&capture.command_name).unwrap();
        temp_vec.push(capture);
    }
    result.push(temp_vec);
    result
}
fn replace_vars(command_vec: &mut Captured, var_map: &HashMap<String, i32>) {
    for command in command_vec.iter_mut() {
        if command.point == "0" {
            if let Some(var_value) = var_map.get(&command.command_name) {
                command.address_value = var_value.to_string();
            } else {
                continue;
            }
        }
        if let Some(var_value) = var_map.get(&command.address_value) {
            command.address_value = var_value.to_string();
        } else {
            continue;
        }
    }
}