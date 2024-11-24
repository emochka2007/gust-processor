use regex::Regex;
fn main() {
    let regex = Regex::new(r"^(LOAD|STORE|CALL|BR|BREQ|BRGE|BRLT|ADD|SUB|MUL|DIV)\s+([=@$])?([0-9]+)\s*").unwrap();
    let halt_regex = Regex::new(r"^HALT\s*$").unwrap();
    // let test_str = "LOAD =42";
    let test_str = "HALT";
    let test = halt_regex.is_match(&test_str);
    println!("test {}", test);
    let caps = regex.captures(&test_str).unwrap();
    let all = caps.get(0).map_or("", |m| m.as_str());
    let left = caps.get(1).map_or("", |m| m.as_str());
    let command = caps.get(2).map_or("", |m| m.as_str());
    let address_mode = caps.get(3).map_or("", |m| m.as_str());
    let address = caps.get(4).map_or("", |m| m.as_str());
    println!("all {}, command {}, address_mode {}, address {}", all, command, address_mode, address);
    // let test_str = "HALT     ";
    // let test = regex.is_match(&test_str);
    // println!("test {}", test);
}


// a+b  == b+a
// f(g,h)

// haskell, lisp (scheme), scala, joy, clojure, erlang

// a=42
// a=a+1
// 43