use std::collections::HashMap;
use regex::Regex;
use crate::binary::BinaryString;
use crate::capture::{parsed_line_data, split_by_org, parse_line};
use crate::constants::commands_count;
use crate::constants::{COMMAND_MAP, MODE_MAP};
use crate::types::Captured;
/// Creates a binary string representation of a command assembly.
///
/// # Arguments
/// - `command`: The command to assemble.
/// - `mode`: The addressing mode.
/// - `address`: The address.
///
/// # Returns
/// A binary string representation of the command assembly.
///
/// # Errors
/// Returns an error if the mode or address parsing fails.
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
        panic!("Error finding mode_map {mode}");
    });
    let binary_address = BinaryString::format_string_10_bytes(address.parse::<u16>().unwrap_or_else(|e| {
        panic!("Error converting address to u32: {address} {:?}", e);
    }));
    assembly_str.push_str(format!("{binary_code}{binary_mode}{binary_address}").as_str());
    assembly_str
}
pub fn command_array_to_bin(data: Vec<Captured>) -> String {
    let (instructions, end_value) = process_org_split(data);
    let mut full_inst = instructions.join("");
    full_inst = format!("{}{}", end_value, full_inst);
    full_inst
}

fn process_org_split(data: Vec<Captured>) -> (Vec<String>, String) {
    let mut end_value: String = BinaryString::format_string_10_bytes(0);
    let mut instructions = Vec::new();
    for org_split in data {
        let mut org_inst = String::new();
        let mut org_value: String = BinaryString::format_string_10_bytes(0);
        let size = BinaryString::format_string_10_bytes(commands_count(&org_split));
        for capture in org_split {
            if capture.command_name == "ORG" {
                org_value = BinaryString::from_string_u16(capture.address_value, 10);
            } else if capture.command_name == "END" {
                end_value = BinaryString::from_string_u16(capture.address_value, 10);
            } else if capture.command_name == "DATA" {
                org_inst.push_str(&capture.address_value);
            } else if COMMAND_MAP.get(&capture.command_name).is_some() {
                let inst = make_assembly_inst(capture.command_name, capture.address_mode, capture.address_value);
                org_inst.push_str(&*inst);
            }
        }
        org_inst = format!("{}{}{}", org_value, size, org_inst);
        instructions.push(org_inst);
    }
    (instructions, end_value)
}

pub fn create_command_array(file_content: Vec<String>, map: &mut HashMap<String, u16>) -> Vec<Captured> {
    let data_regex = Regex::new(r"^([A-Z]+)?\s+DATA\s*(([0-9]+)((,[0-9]+))*)\s*").unwrap();
    let mut command_vec: Captured = Vec::new();
    for line in file_content {
        match data_regex.is_match(&line) {
            true => {
                //TODO: fix data with comma
                let line_parsed = parsed_line_data(&line, &data_regex, map).unwrap_or_else(|e| panic!("Error parsed_line_data {:?}", e));
                command_vec.push(line_parsed);
            }
            _ => {
                let line_parsed =
                    parse_line(&line, map).unwrap_or_else(|e| panic!("{:?}", e));
                if line_parsed.command_name != "" {
                    command_vec.push(line_parsed);
                }
            }
        }
    }
    let mut split_vec = split_by_org(command_vec, map);
    for split in &mut split_vec {
        replace_vars(split, &map).unwrap_or_else(|e| panic!("{:?}", e));
    }
    split_vec
}
fn replace_vars(command_vec: &mut Captured, var_map: &HashMap<String, u16>) -> Result<(), String>  {
    println!("{:?}", command_vec);
    println!("{:?}", var_map);
    for command in command_vec.iter_mut() {
        if command.point == "0" {
            // if it's not a command - we get a value from var_map
            // example: START    LOAD   =1
            if let Some(var_value) = var_map.get(&command.command_name) {
                command.address_value = var_value.to_string();
            }
        } else {
            // if it's a command  => then we replace the value inside it with value in map
            // example:      END    START
            if let Some(var_value) = var_map.get(&command.address_value) {
                command.address_value = var_value.to_string();
            }
        }
    }
    Ok(())
}

