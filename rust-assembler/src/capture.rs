use std::collections::HashMap;
use std::sync::atomic::Ordering;
use regex::{Error, Regex};
use crate::binary::BinaryString;
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

pub fn parse_line(
    line: &String,
    variables: &mut HashMap<String, u16>,
) -> Result<LineParsed, Error> {
    let command_regex = Regex::new(
        r"^([A-Z]+)?\s+(LOAD|STORE|CALL|BR|BREQ|BRGE|BRLT|ADD|SUB|MUL|DIV|ORG|END)\s+([=@$])?(([0-9]+)|([A-Z]+))\s*",
    )?;

    let halt_regex = Regex::new(r"^([A-Z]+)?\s*(HALT)\s*$")?;
    let var_regex = Regex::new(r"([A-Z]+)\s+([0-9]+)")?;
    if command_regex.is_match(&line) || halt_regex.is_match(&line) {
        let captures = command_regex.captures(&line).unwrap_or_else(
            || halt_regex.captures(&line).unwrap_or_else(|| panic!("Error capturing halt or command regex {line}"))
        );
        let point = extract_capture(&captures, 1);
        let command_name = extract_capture(&captures, 2);
        let address_mode = extract_capture(&captures, 3);
        let address_value = extract_capture(&captures, 4);
        variables.insert(point.clone(), COMMAND_LENGTH.load(Ordering::SeqCst));
        Ok(LineParsed {
            point,
            command_name,
            address_mode,
            address_value
        })
    }
    else if var_regex.is_match(&line) {
        let captures = var_regex.captures(&line).unwrap_or_else(|| panic!("Error capturing var regex {line}"));
        let point = extract_capture(&captures, 1);
        let point_value = extract_capture(&captures, 2).parse::<u16>().unwrap_or_else(|e| panic!("Error parsing to uint var value {:?}", e));
        variables.insert(point.clone(), point_value);
        Ok(LineParsed {
            point: "0".to_string(),
            command_name: point,
            address_mode: "".to_string(),
            address_value: "".to_string(),
        })
    } else {
        Err(Error::Syntax(format!("Error capturing {line}")))
    }
}

pub fn parsed_line_data(line: &String, data_regex: &Regex, var_map: &mut HashMap<String, u16>) -> Result<LineParsed, Error> {
    let captures = data_regex.captures(&line).unwrap_or_else(|| panic!("Error capturing parse data {line}"));

    let capture_value = extract_capture(&captures, 2);
    let data_values: Vec<&str> = capture_value.split(",").collect();
    let binary_data_values: Vec<String> = data_values
        .iter()
        .map(|n| BinaryString::from_string_u16(n.to_string(), 16))
        .collect();

    let point: String = extract_capture(&captures, 1).split(",").collect::<Vec<&str>>().join("");
    var_map.insert(point.clone(), COMMAND_LENGTH.load(Ordering::SeqCst));
    Ok(LineParsed {
        point,
        command_name: "DATA".to_string(),
        address_mode: "".to_string(),
        address_value: binary_data_values.join(""),
    })
}
pub fn split_by_org(commands: Captured, var_map: &mut HashMap<String, u16>) -> Vec<Captured> {
    let mut split_command: Vec<Captured> = Vec::new();
    let mut org_command_count = 0;
    let mut org_command_vec: Captured = Vec::new();
    for parsed_line in commands {
        if parsed_line.command_name == "ORG" {
            ORG_ADDRESS.store(parsed_line.address_value.parse::<u16>().unwrap_or_else(|e| panic!("Error parsing address_value {line} {:?}", e, line = line!())), Ordering::SeqCst, );
            org_command_count += 1;
        }
        if let Some(point_value) = var_map.get_mut(&parsed_line.point) {
            *point_value = COMMAND_LENGTH.load(Ordering::SeqCst) + ORG_ADDRESS.load(Ordering::SeqCst);
        }
        if org_command_count > 1 && parsed_line.command_name == "ORG" {
            COMMAND_LENGTH.store(0, Ordering::SeqCst);
            split_command.push(org_command_vec);
            org_command_vec = Vec::new();
        }
        inc_command_len(&parsed_line.command_name);
        org_command_vec.push(parsed_line);
    }
    split_command.push(org_command_vec);
    split_command
}

fn extract_capture(caps: &regex::Captures, index: usize) -> String {
    caps.get(index).map_or("", |m| m.as_str()).to_string()
}