use std::sync::atomic::{AtomicU16, Ordering};
use phf::phf_map;
use crate::types::Captured;

pub static ORG_ADDRESS: AtomicU16 = AtomicU16::new(0);
pub static COMMAND_LENGTH: AtomicU16 = AtomicU16::new(0);
pub static COMMAND_MAP: phf::Map<&'static str, &str> = phf_map! {
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

pub static MODE_MAP: phf::Map<&'static str, &str> = phf_map! {
    "" => "00", //direct
    "=" => "01",
    "$" =>  "10",
    "@" => "11",
};
pub fn inc_command_len(command: &str) -> () {
    if let Some(_) = COMMAND_MAP.get(&command) {
        COMMAND_LENGTH.fetch_add(1, Ordering::SeqCst);
    }
}
pub fn commands_count(data: &Captured) -> usize {
    let mut size = 0;
    for capture in data {
        if COMMAND_MAP.get(&capture.command_name).is_some() || capture.command_name == "DATA" {
            size += 1;
        }
    }
    size
}

