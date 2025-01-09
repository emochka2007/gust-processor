use std::collections::HashMap;
use std::process::exit;
use std::sync::atomic::Ordering;
use regex::{Regex};
use crate::constants::{inc_command_len, COMMAND_LENGTH, ORG_ADDRESS};
use crate::types::Captured;

/**
- "1" or "0", if command => "1",
- command name
- address mode
- address/value
*/
#[derive(Debug)]
pub struct LineParsed {
    pub point: String,
    pub command_name: String,
    pub address_mode: String,
    pub address_value: String,
}
pub fn verify_and_capture(
    test_str: &String,
    var_map: &mut HashMap<String, i32>,
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

pub fn capture_and_parse_data<'a>(data: &'a String, data_regex: &Regex, var_map: &mut HashMap<String, i32>) -> LineParsed {
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
pub fn split_by_org(data: Captured, var_map: &mut HashMap<String, i32>) -> Vec<Captured> {
    let mut result: Vec<Captured> = Vec::new();
    let mut org_count = 0;
    let mut temp_vec: Captured = Vec::new();
    for capture in data {
        if capture.command_name == "ORG" {
            ORG_ADDRESS.store(capture.address_value.parse::<u16>().unwrap(), Ordering::SeqCst);
            org_count += 1;
        }
        if let Some(point_value) = var_map.get_mut(&capture.point) {
            *point_value = (COMMAND_LENGTH.load(Ordering::SeqCst) + ORG_ADDRESS.load(Ordering::SeqCst)) as i32;
        }
        if org_count > 1 && capture.command_name == "ORG" {
            COMMAND_LENGTH.store(0, Ordering::SeqCst);
            result.push(temp_vec);
            temp_vec = Vec::new();
        }
        inc_command_len(&capture.command_name);
        temp_vec.push(capture);
    }
    result.push(temp_vec);
    result
}
