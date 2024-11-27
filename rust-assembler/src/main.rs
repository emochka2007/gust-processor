use std::collections::{hash_map, HashMap};

use regex::Regex;
fn main() {
    let regex = Regex::new(r"^(LOAD|STORE|CALL|BR|BREQ|BRGE|BRLT|ADD|SUB|MUL|DIV)\s+([=@$])?([0-9]+)\s*").unwrap();
    let halt_regex = Regex::new(r"^HALT\s*$").unwrap();
    let test_str = "LOAD 42";
    // let test_str = "HALT";
    let test = regex.is_match(&test_str);
    println!("test {}", test);
    let caps = regex.captures(&test_str).unwrap();
    let all = caps.get(0).map_or("", |m| m.as_str());
    let command = caps.get(1).map_or("", |m| m.as_str());
    let address_mode = caps.get(2).map_or("", |m| m.as_str());
    let address = caps.get(3).map_or("", |m| m.as_str());
    println!("all {}, command {}, address_mode {}, address {}", all, command, address_mode, address);
    make_assembly_inst(String::from(command), String::from(address_mode), String::from(address));
}

fn make_assembly_inst(command: String, mode: String, address: String) {
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
        ("@", "11")
    ]);

    let binary_code = command_map.get(command.as_str()).unwrap();
    println!("{}", mode);
    let binary_mode = mode_map.get(mode.as_str()).unwrap();
    let binary_address = format!("{:0>10b}", address.parse::<u32>().unwrap());
    /** read from one file, write to another */
    println!("{}{}{}", binary_code, binary_mode, binary_address);
}

