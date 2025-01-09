use std::collections::HashMap;
use std::process::exit;
use regex::Regex;
use crate::capture::{capture_and_parse_data, split_by_org, verify_and_capture};
use crate::constants::commands_count;
use crate::constants::{COMMAND_MAP, MODE_MAP};
use crate::types::Captured;

pub fn make_assembly_inst(command: String, mode: String, address: String) -> String {
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
pub fn command_array_to_bin(data: Vec<Captured>) -> String {
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
                let inst = make_assembly_inst(capture.command_name, capture.address_mode, capture.address_value);
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
pub fn create_command_array(file_content: Vec<String>, map: &mut HashMap<String, i32>) -> Vec<Captured> {
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

